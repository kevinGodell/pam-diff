// jshint esversion: 6, globalstrict: true, strict: true
'use strict';

const util = require('util');
const Transform = require('stream').Transform;

function PamDiff(options) {
    if (!(this instanceof PamDiff)) {
        return new PamDiff(options);
    }
    Transform.call(this, {objectMode: true});
    this._rgb2gray = this._parseOptions('rgb2gray', options);
    this._sensitivity = this._validateNumber(this._parseOptions('sensitivity', options), 30, 1, 255);
    this._threshold = this._validateNumber(this._parseOptions('threshold', options), 300, 1, 20000);
    this._newPix = null;
    this._oldPix = null;
    this._comparePixels = null;//pixel difference engine, to be determined
}

util.inherits(PamDiff, Transform);

PamDiff.prototype._parseOptions = function (option, options) {
    if (options && options.hasOwnProperty(option)) {
        return options[option];
    }
    return null;
};

PamDiff.prototype._validateNumber = function (number, def, low, high) {
    if (isNaN(number)) {
        return def;
    } else if (number < low) {
        return low;
    } else if (number > high) {
        return high;
    } else {
        return number;
    }
};

PamDiff.prototype._getGrayEngine = function (type) {
    if (type === 'lightness') {
        return PamDiff.prototype._lightness;
    } else if (type === 'luminosity') {
        return PamDiff.prototype._luminosity;
    } else {
        return PamDiff.prototype._average;
    }
};

//convert rgb to gray
PamDiff.prototype._average = function (r, g, b) {
    return (r + g + b) / 3;
};

//convert rgb to gray
PamDiff.prototype._lightness = function (r, g, b) {
    return (Math.max(r, g, b) + Math.min(r, g, b)) / 2;
};

//convert rgb to gray
PamDiff.prototype._luminosity = function (r, g, b) {
    return 0.299 * r + 0.587 * g + 0.114 * b;
};

PamDiff.prototype._transform = function (chunk, encoding, callback) {
    //console.log('transform', chunk.pixels.length);
    this._oldPix = this._newPix;
    this._newPix = chunk.pixels;
    if (this._oldPix !== null) {
        this._comparePixels(this._oldPix, this._newPix);
    } else {
        let width = parseInt(chunk.width);
        let length = this._newPix.length;
        let sensitivity = this._sensitivity;//amount of difference between pixels
        let threshold = this._threshold;//number of pixels that are different
        //let height = parseInt(chunk.height);
        //let maxval = parseInt(chunk.maxval);
        //this._diffPixels = new Uint8ClampedArray(width * height * 4);//size it once based on rgba output, reuse for each diff
        switch (chunk.tupltype) {
            case 'blackandwhite' ://each pixel will only be 1 byte with value 0 or 1, depth is 1
                this._comparePixels = (oldPix, newPix) => {
                    let coords = [];
                    for (let i = 0, x = 0, y = 0, p = 0; i < length; i++, x++, p+=4) {
                        if (x === width) {
                            x = 0;
                            y++;
                        }
                        if (oldPix[i] !== newPix[i]) {
                            coords.push({p: p, x: x, y: y});
                        }
                    }
                    if (coords.length > threshold) {
                        this.emit('diff', {coords: coords});
                    }
                };
                break;
            case 'grayscale' ://each pixel will only be 1 byte with value 0 to 255, depth is 1
                this._comparePixels = (oldPix, newPix) => {
                    let coords = [];
                    for (let i = 0, x = 0, y = 0, p = 0; i < length; i++, x++, p+=4) {
                        if (x === width) {
                            x = 0;
                            y++;
                        }
                        if (Math.abs(oldPix[i] - newPix[i]) > sensitivity) {
                            coords.push({p: p, x: x, y: y});
                        }
                    }
                    if (coords.length > threshold) {
                        this.emit('diff', {coords: coords, percent: Math.round(coords.length/length * 100)});
                    }
                };
                break;
            case 'rgb' ://each pixel will be 3 bytes with value 0 to 255, depth is 3
            case 'rgb_alpha' ://each pixel will be 4 bytes with value 0 to 255, depth is 4
                let depth = parseInt(chunk.depth);
                let toGray = this._getGrayEngine(this._rgb2gray);
                this._comparePixels = (oldPix, newPix) => {
                    let coords = [];
                    for (let i = 0, x = 0, y = 0, p = 0; i < length; i+=depth, x++, p+=4) {
                        if (x === width) {
                            x = 0;
                            y++;
                        }
                        if (Math.abs(toGray(oldPix[i], oldPix[i + 1], oldPix[i + 2]) - toGray(newPix[i], newPix[i + 1], newPix[i + 2])) > sensitivity) {
                            coords.push({p: p, x: x, y: y});
                        }
                    }
                    if (coords.length > threshold) {
                        this.emit('diff', {coords: coords});
                    }
                };
                break;
            default :
                throw Error(`Unsupported tupltype: ${chunk.tupltype}. Supported tupltypes include grayscale(gray), blackandwhite(monob), rgb(rgb24), and rgb_alpha(rgba).`);
        }
    }
    callback();
};

PamDiff.prototype._flush = function (callback) {
    this._newPix = null;
    this._oldPix = null;
    this._comparePixels = null;
    callback();
};

module.exports = PamDiff;