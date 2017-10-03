// jshint esversion: 6, globalstrict: true, strict: true
'use strict';

const util = require('util');
const Transform = require('stream').Transform;

function PamDiff(options) {
    if (!(this instanceof PamDiff)) {
        return new PamDiff(options);
    }
    Transform.call(this, {objectMode: true});
    this.setGrayscale(this._parseOptions('grayscale', options));
    this.setDifference(this._parseOptions('difference', options));
    this.setPercent(this._parseOptions('percent', options));
    this._parseChunk = this._parseFirstChunk;
}

util.inherits(PamDiff, Transform);

PamDiff.prototype.setGrayscale = function (value) {
    switch (value) {
        case 'red' :
            this._grayscale = this._redToGray;
            break;
        case 'green' :
            this._grayscale = this._greenToGray;
            break;
        case 'blue' :
            this._grayscale = this._blueToGray;
            break;
        case 'desaturation' :
            this._grayscale = this._desaturationToGray;
            break;
        case 'luminosity' :
            this._grayscale = this._luminosityToGray;
            break;
        default :
            this._grayscale = this._averageToGray;
    }
};

PamDiff.prototype.setDifference = function (value) {
    this._difference = this._validateNumber(value, 5, 1, 255);
};

PamDiff.prototype.setPercent = function (value) {
    this._percent = this._validateNumber(value, 5, 1, 100);
};

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

//convert rgb to gray
PamDiff.prototype._averageToGray = function (r, g, b) {
    return (r + g + b) / 3;
};

//convert rgb to gray
PamDiff.prototype._desaturationToGray = function (r, g, b) {
    return (Math.max(r, g, b) + Math.min(r, g, b)) / 2;
};

//convert rgb to gray
PamDiff.prototype._luminosityToGray = function (r, g, b) {
    //return 0.299 * r + 0.587 * g + 0.114 * b;
    return 0.3 * r + 0.59 * g + 0.11 * b;
};

//convert rgb to gray
PamDiff.prototype._redToGray = function (r, g, b) {
    return r;
};

//convert rgb to gray
PamDiff.prototype._greenToGray = function (r, g, b) {
    return g;
};

//convert rgb to gray
PamDiff.prototype._blueToGray = function (r, g, b) {
    return b;
};

PamDiff.prototype._blackAndWhitePixelDiff = function (chunk) {
    this._newPix = chunk.pixels;
    let diffPix = [];
    for (let i = 0, x = 0, y = 0; i < this._length; i++, x++) {
        if (x === this._width) {
            x = 0;
            y++;
        }
        if (this._oldPix[i] !== this._newPix[i]) {
            diffPix.push({x: x, y: y});
        }
    }
    let percent = Math.ceil(diffPix.length / this._length * 100);
    if (percent >= this._percent) {
        this.emit('diff', {diffPix: diffPix, percent: percent});
    }
    this._oldPix = this._newPix;
};

PamDiff.prototype._grayScalePixelDiff = function (chunk) {
    this._newPix = chunk.pixels;
    let diffPix = [];
    for (let i = 0, x = 0, y = 0; i < this._length; i++, x++) {
        if (x === this._width) {
            x = 0;
            y++;
        }
        if (Math.abs(this._oldPix[i] - this._newPix[i]) >= this._difference) {
            diffPix.push({x: x, y: y});
        }
    }
    let percent = Math.ceil(diffPix.length / this._length * 100);
    if (percent >= this._percent) {
        this.emit('diff', {diffPix: diffPix, percent: percent});
    }
    this._oldPix = this._newPix;
};

PamDiff.prototype._rgbPixelDiff = function (chunk) {
    this._newPix = chunk.pixels;
    let diffPix = [];
    for (let i = 0, x = 0, y = 0; i < this._length; i+=3, x++) {
        if (x === this._width) {
            x = 0;
            y++;
        }
        if (Math.abs(this._grayscale(this._oldPix[i], this._oldPix[i + 1], this._oldPix[i + 2]) - this._grayscale(this._newPix[i], this._newPix[i + 1], this._newPix[i + 2])) >= this._difference) {
            diffPix.push({x: x, y: y});
        }
    }
    let percent = Math.ceil(diffPix.length / this._length * 100);
    if (percent >= this._percent) {
        this.emit('diff', {diffPix: diffPix, percent: percent});
    }
    this._oldPix = this._newPix;
};

PamDiff.prototype._rgbAlphaPixelDiff = function (chunk) {
    this._newPix = chunk.pixels;
    let diffPix = [];
    for (let i = 0, x = 0, y = 0; i < this._length; i+=4, x++) {
        if (x === this._width) {
            x = 0;
            y++;
        }
        if (Math.abs(this._grayscale(this._oldPix[i], this._oldPix[i + 1], this._oldPix[i + 2]) - this._grayscale(this._newPix[i], this._newPix[i + 1], this._newPix[i + 2])) >= this._difference) {
            diffPix.push({x: x, y: y});
        }
    }
    let percent = Math.ceil(diffPix.length / this._length * 100);
    if (percent >= this._percent) {
        this.emit('diff', {diffPix: diffPix, percent: percent});
    }
    this._oldPix = this._newPix;
};

PamDiff.prototype._parseFirstChunk = function (chunk) {
    this._width = parseInt(chunk.width);
    this._oldPix = chunk.pixels;
    this._length = this._oldPix.length;
    switch (chunk.tupltype) {
        case 'blackandwhite' :
            this._parseChunk = this._blackAndWhitePixelDiff;
            break;
        case 'grayscale' :
            this._parseChunk = this._grayScalePixelDiff;
            break;
        case 'rgb' :
            this._parseChunk = this._rgbPixelDiff;
            break;
        case 'rgb_alpha' :
            this._parseChunk = this._rgbAlphaPixelDiff;
            break;
        default :
            throw Error(`Unsupported tupltype: ${chunk.tupltype}. Supported tupltypes include grayscale(gray), blackandwhite(monob), rgb(rgb24), and rgb_alpha(rgba).`);
    }
};

PamDiff.prototype._transform = function (chunk, encoding, callback) {
    this._parseChunk(chunk);
    callback();
};

PamDiff.prototype._flush = function (callback) {
    delete this._oldPix;
    delete this._newPix;
    delete this._width;
    delete this._length;
    this._parseChunk = this._parseFirstChunk;
    callback();
};

module.exports = PamDiff;