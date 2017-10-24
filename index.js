// jshint esversion: 6, globalstrict: true, strict: true
'use strict';

const util = require('util');
const Transform = require('stream').Transform;
const PP = require('polygon-points');

function PamDiff(options) {
    if (!(this instanceof PamDiff)) {
        return new PamDiff(options);
    }
    Transform.call(this, {objectMode: true});
    this.setGrayscale(this._parseOptions('grayscale', options));//global option
    this.setDifference(this._parseOptions('difference', options));//global option, can be overridden per region
    this.setPercent(this._parseOptions('percent', options));//global option, can be overridden per region
    this.setRegions(this._parseOptions('regions', options));//can be no regions or a single region or multiple regions. if no regions, all pixels will be compared.
    this._parseChunk = this._parseFirstChunk;//first parsing will be reading settings and configuring internal pixel reading
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
    this._difference = this._validateNumber(parseInt(value), 5, 1, 255);
};

PamDiff.prototype.setPercent = function (value) {
    this._percent = this._validateNumber(parseInt(value), 5, 1, 100);
};

PamDiff.prototype.setRegions = function (regions) {
    if (!regions) {
        if (this._regions) {
            delete this._regions;
            delete this._regionsLength;
        }
        this._diffs = 0;
        return;
    } else if (!Array.isArray(regions) || regions.length < 1) {
        throw new Error(`Regions must be an array of at least 1 region object {name: 'region1', difference: 10, percent: 10, polygon: [[0, 0], [0, 50], [50, 50], [50, 0]]}`);
    }
    this._regions = [];
    for (const region of regions) {
        if (!region.hasOwnProperty('name') || !region.hasOwnProperty('polygon')) {
            throw new Error('Region must include a name and a polygon property');
        }
        const polygonPoints = new PP(region.polygon);
        this._regions.push(
            {
                name: region.name,
                polygon: polygonPoints,
                pointsLength: polygonPoints.pointsLength,
                difference: this._validateNumber(parseInt(region.difference), this._difference, 1, 255),
                percent: this._validateNumber(parseInt(region.percent), this._percent, 1, 100),
                diffs: 0
            }
        );
    }
    this._regionsLength = this._regions.length;
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
    for (let y = 0, i = 0; y < this._height; y++) {
        for (let x = 0; x < this._width; x++, i++) {
            const diff = this._oldPix[i] !== this._newPix[i];
            if (this._regions) {
                for (let j = 0; j < this._regionsLength; j++) {
                    if (diff === true && this._regions[j].polygon.containsPoint(x, y) === true) {
                            this._regions[j].diffs++;
                    }
                }
            } else {
                if (diff === true) {
                    this._diffs++;
                }
            }
        }
    }
    if (this._regions) {
        const regionDiffArray = [];
        for (let i = 0; i < this._regionsLength; i++) {
            const percent = Math.ceil(this._regions[i].diffs / this._regions[i].pointsLength * 100);
            if (percent >= this._regions[i].percent) {
                regionDiffArray.push({name: this._regions[i].name, percent: percent});
            }
            this._regions[i].diffs = 0;
        }
        if (regionDiffArray.length > 0) {
            this.emit('diff', {trigger: regionDiffArray, pam: chunk.pam});
        }
    } else {
        const percent = Math.ceil(this._diffs / this._length * 100);
        if (percent >= this._percent) {
            this.emit('diff', {trigger: [{name: 'percent', percent: percent}], pam: chunk.pam});
        }
        this._diffs = 0;
    }
    this._oldPix = this._newPix;
};

PamDiff.prototype._grayScalePixelDiff = function (chunk) {
    this._newPix = chunk.pixels;
    for (let y = 0, i = 0; y < this._height; y++) {
        for (let x = 0; x < this._width; x++, i++) {
            const diff = Math.abs(this._oldPix[i] - this._newPix[i]);
            if (this._regions) {
                for (let j = 0; j < this._regionsLength; j++) {
                    if (diff >= this._regions[j].difference && this._regions[j].polygon.containsPoint(x, y) === true) {
                        this._regions[j].diffs++;
                    }
                }
            } else {
                if (diff >= this._difference) {
                    this._diffs++;
                }
            }
        }
    }
    if (this._regions) {
        const regionDiffArray = [];
        for (let i = 0; i < this._regionsLength; i++) {
            const percent = Math.ceil(this._regions[i].diffs / this._regions[i].pointsLength * 100);
            if (percent >= this._regions[i].percent) {
                regionDiffArray.push({name: this._regions[i].name, percent: percent});
            }
            this._regions[i].diffs = 0;
        }
        if (regionDiffArray.length > 0) {
            this.emit('diff', {trigger: regionDiffArray, pam: chunk.pam});
        }
    } else {
        const percent = Math.ceil(this._diffs / this._length * 100);
        if (percent >= this._percent) {
            this.emit('diff', {trigger: [{name: 'percent', percent: percent}], pam: chunk.pam});
        }
        this._diffs = 0;
    }
    this._oldPix = this._newPix;
};

PamDiff.prototype._rgbPixelDiff = function (chunk) {
    this._newPix = chunk.pixels;
    for (let y = 0, i = 0; y < this._height; y++) {
        for (let x = 0; x < this._width; x++, i += 3) {
            const diff = Math.abs(this._grayscale(this._oldPix[i], this._oldPix[i + 1], this._oldPix[i + 2]) - this._grayscale(this._newPix[i], this._newPix[i + 1], this._newPix[i + 2]));
            if (this._regions) {
                for (let j = 0; j < this._regionsLength; j++) {
                    if (diff >= this._regions[j].difference && this._regions[j].polygon.containsPoint(x, y) === true) {
                        this._regions[j].diffs++;
                    }
                }
            } else {
                if (diff >= this._difference) {
                    this._diffs++;
                }
            }
        }
    }
    if (this._regions) {
        const regionDiffArray = [];
        for (let i = 0; i < this._regionsLength; i++) {
            const percent = Math.ceil(this._regions[i].diffs / this._regions[i].pointsLength * 100);
            if (percent >= this._regions[i].percent) {
                regionDiffArray.push({name: this._regions[i].name, percent: percent});
            }
            this._regions[i].diffs = 0;
        }
        if (regionDiffArray.length > 0) {
            this.emit('diff', {trigger: regionDiffArray, pam: chunk.pam});
        }
    } else {
        const percent = Math.ceil(this._diffs / this._length * 100);
        if (percent >= this._percent) {
            this.emit('diff', {trigger: [{name: 'percent', percent: percent}], pam: chunk.pam});
        }
        this._diffs = 0;
    }
    this._oldPix = this._newPix;
};

PamDiff.prototype._rgbAlphaPixelDiff = function (chunk) {
    this._newPix = chunk.pixels;
    for (let y = 0, i = 0; y < this._height; y++) {
        for (let x = 0; x < this._width; x++, i += 4) {
            const diff = Math.abs(this._grayscale(this._oldPix[i], this._oldPix[i + 1], this._oldPix[i + 2]) - this._grayscale(this._newPix[i], this._newPix[i + 1], this._newPix[i + 2]));
            if (this._regions) {
                for (let j = 0; j < this._regionsLength; j++) {
                    if (diff >= this._regions[j].difference && this._regions[j].polygon.containsPoint(x, y) === true) {
                        this._regions[j].diffs++;
                    }
                }
            } else {
                if (diff >= this._difference) {
                    this._diffs++;
                }
            }
        }
    }
    if (this._regions) {
        const regionDiffArray = [];
        for (let i = 0; i < this._regionsLength; i++) {
            const percent = Math.ceil(this._regions[i].diffs / this._regions[i].pointsLength * 100);
            if (percent >= this._regions[i].percent) {
                regionDiffArray.push({name: this._regions[i].name, percent: percent});
            }
            this._regions[i].diffs = 0;
        }
        if (regionDiffArray.length > 0) {
            this.emit('diff', {trigger: regionDiffArray, pam: chunk.pam});
        }
    } else {
        const percent = Math.ceil(this._diffs / this._length * 100);
        if (percent >= this._percent) {
            this.emit('diff', {trigger: [{name: 'percent', percent: percent}], pam: chunk.pam});
        }
        this._diffs = 0;
    }
    this._oldPix = this._newPix;
};

PamDiff.prototype._parseFirstChunk = function (chunk) {
    this._width = parseInt(chunk.width);
    this._height = parseInt(chunk.height);
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
            //this._increment = 3;//future use
            break;
        case 'rgb_alpha' :
            this._parseChunk = this._rgbAlphaPixelDiff;
            //this._increment = 4;//future use
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
//todo get bounding box of all regions combined to exclude some pixels before checking if they exist inside specific regions
//todo add option for break on first region so that pixel is not measured in multiple overlapping regions