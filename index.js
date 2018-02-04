'use strict';

const Transform = require('stream').Transform;
const PP = require('polygon-points');

class PamDiff extends Transform {
    constructor(options, callback) {
        super(options);
        Transform.call(this, {objectMode: true});
        this.difference = PamDiff.parseOptions('difference', options);//global option, can be overridden per region
        this.percent = PamDiff.parseOptions('percent', options);//global option, can be overridden per region
        this.regions = PamDiff.parseOptions('regions', options);//can be no regions or a single region or multiple regions. if no regions, all pixels will be compared.
        this.callback = callback;//callback function to be called when pixel difference is detected
        this._parseChunk = this._parseFirstChunk;//first parsing will be reading settings and configuring internal pixel reading
    }

    //static methods

    static parseOptions(option, options) {
        if (options && options.hasOwnProperty(option)) {
            return options[option];
        }
        return null;
    }

    static validateNumber(number, def, low, high) {
        if (isNaN(number)) {
            return def;
        } else if (number < low) {
            return low;
        } else if (number > high) {
            return high;
        } else {
            return number;
        }
    }

    //public methods

    setGrayscale(string) {
        console.warn('grayscale option has been removed, averaging has proven to most accurate');
    }

    set difference(number) {
        this._difference = PamDiff.validateNumber(parseInt(number), 5, 1, 255);
    }

    setDifference(number) {
        console.warn('setDifference deprecated and will be removed in next minor version, use difference = number');
        this.difference = number;
    }

    set percent(number) {
        this._percent = PamDiff.validateNumber(parseInt(number), 5, 1, 100);
    }

    setPercent(number) {
        console.warn('setPercent deprecated and will be removed in next minor version, use difference = number');
        this.percent = number;
    }

    set regions(array) {
        if (!array) {
            if (this._regions) {
                delete this._regions;
                delete this._regionsLength;
                delete this._minDiff;
            }
            this._diffs = 0;
            return;
        } else if (!Array.isArray(array) || array.length < 1) {
            throw new Error(`Regions must be an array of at least 1 region object {name: 'region1', difference: 10, percent: 10, polygon: [[0, 0], [0, 50], [50, 50], [50, 0]]}`);
        }
        this._regions = [];
        this._minDiff = 255;
        for (const region of array) {
            if (!region.hasOwnProperty('name') || !region.hasOwnProperty('polygon')) {
                throw new Error('Region must include a name and a polygon property');
            }
            const polygonPoints = new PP(region.polygon);
            const difference = PamDiff.validateNumber(parseInt(region.difference), this._difference, 1, 255);
            const percent = PamDiff.validateNumber(parseInt(region.percent), this._percent, 1, 100);
            this._minDiff = Math.min(this._minDiff, difference);
            this._regions.push(
                {
                    name: region.name,
                    polygon: polygonPoints,
                    difference: difference,
                    percent: percent,
                    diffs: 0
                }
            );
        }
        this._regionsLength = this._regions.length;
        this._createPointsInPolygons(this._regions, this._width, this._height);
    }

    setRegions(array) {
        console.warn('setRegions deprecated and will be removed in next minor version, use regions = value');
        this.regions = array;
    }

    set callback(func) {
        if (typeof func === 'function') {
            if (func.length !== 1) {
                throw new Error('Callback function must only accept 1 argument');
            }
            this._callback = func;
        } else {
            delete this._callback;
        }
    }

    setCallback(func) {
        console.warn('setCallback deprecated and will be removed in next minor version, use callback = function');
        this.callback = func;
    }

    resetCache() {
        delete this._oldPix;
        delete this._newPix;
        delete this._width;
        delete this._length;
        this._parseChunk = this._parseFirstChunk;
    }

    //private methods

    _createPointsInPolygons(regions, width, height) {
        if (regions && width && height) {
            this._pointsInPolygons = [];
            for (const region of regions) {
                const bitset = region.polygon.getBitset(this._width, this._height);
                region.pointsLength = bitset.count;
                this._pointsInPolygons.push(bitset.buffer);
            }
        }
    }

    _blackAndWhitePixelDiff(chunk) {
        this._newPix = chunk.pixels;
        for (let y = 0, i = 0; y < this._height; y++) {
            for (let x = 0; x < this._width; x++, i++) {
                const diff = this._oldPix[i] !== this._newPix[i];
                if (this._regions && diff === true) {
                    for (let j = 0; j < this._regionsLength; j++) {
                        if (this._pointsInPolygons[j][i]) {
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
                const percent = Math.floor(100 * this._regions[i].diffs / this._regions[i].pointsLength);
                if (percent >= this._regions[i].percent) {
                    regionDiffArray.push({name: this._regions[i].name, percent: percent});
                }
                this._regions[i].diffs = 0;
            }
            if (regionDiffArray.length > 0) {
                const data = {trigger: regionDiffArray, pam: chunk.pam};
                if (this._callback) {
                    this._callback(data);
                }
                if (this._readableState.pipesCount > 0) {
                    this.push(data);
                }
                if (this.listenerCount('diff') > 0) {
                    this.emit('diff', data);
                }
            }
        } else {
            const percent = Math.floor(100 * this._diffs / this._length);
            if (percent >= this._percent) {
                const data = {trigger: [{name: 'percent', percent: percent}], pam: chunk.pam};
                if (this._callback) {
                    this._callback(data);
                }
                if (this._readableState.pipesCount > 0) {
                    this.push(data);
                }
                if (this.listenerCount('diff') > 0) {
                    this.emit('diff', data);
                }
            }
            this._diffs = 0;
        }
        this._oldPix = this._newPix;
    }

    _grayScalePixelDiff(chunk) {
        this._newPix = chunk.pixels;
        for (let y = 0, i = 0; y < this._height; y++) {
            for (let x = 0; x < this._width; x++, i++) {
                if (this._oldPix[i] !== this._newPix[i]) {
                    const diff = Math.abs(this._oldPix[i] - this._newPix[i]);
                    if (this._regions && diff >= this._minDiff) {
                        for (let j = 0; j < this._regionsLength; j++) {
                            if (this._pointsInPolygons[j][i] && diff >= this._regions[j].difference) {
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
        }
        if (this._regions) {
            const regionDiffArray = [];
            for (let i = 0; i < this._regionsLength; i++) {
                const percent = Math.floor(100 * this._regions[i].diffs / this._regions[i].pointsLength);
                if (percent >= this._regions[i].percent) {
                    regionDiffArray.push({name: this._regions[i].name, percent: percent});
                }
                this._regions[i].diffs = 0;
            }
            if (regionDiffArray.length > 0) {
                const data = {trigger: regionDiffArray, pam: chunk.pam};
                if (this._callback) {
                    this._callback(data);
                }
                if (this._readableState.pipesCount > 0) {
                    this.push(data);
                }
                if (this.listenerCount('diff') > 0) {
                    this.emit('diff', data);
                }
            }
        } else {
            const percent = Math.floor(100 * this._diffs / this._length);
            if (percent >= this._percent) {
                const data = {trigger: [{name: 'percent', percent: percent}], pam: chunk.pam};
                if (this._callback) {
                    this._callback(data);
                }
                if (this._readableState.pipesCount > 0) {
                    this.push(data);
                }
                if (this.listenerCount('diff') > 0) {
                    this.emit('diff', data);
                }
            }
            this._diffs = 0;
        }
        this._oldPix = this._newPix;
    }

    _rgbPixelDiff(chunk) {
        this._newPix = chunk.pixels;
        for (let y = 0, i = 0, p = 0; y < this._height; y++) {
            for (let x = 0; x < this._width; x++, i += 3, p++) {
                if (this._oldPix[i] !== this._newPix[i] || this._oldPix[i + 1] !== this._newPix[i + 1] || this._oldPix[i + 2] !== this._newPix[i + 2]) {
                    const diff = Math.abs(this._oldPix[i] + this._oldPix[i + 1] + this._oldPix[i + 2] - this._newPix[i] - this._newPix[i + 1] - this._newPix[i + 2])/3;
                    if (this._regions && diff >= this._minDiff) {
                        for (let j = 0; j < this._regionsLength; j++) {
                            if (this._pointsInPolygons[j][p] && diff >= this._regions[j].difference) {
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
        }
        if (this._regions) {
            const regionDiffArray = [];
            for (let i = 0; i < this._regionsLength; i++) {
                const percent = Math.floor(100 * this._regions[i].diffs / this._regions[i].pointsLength);
                if (percent >= this._regions[i].percent) {
                    regionDiffArray.push({name: this._regions[i].name, percent: percent});
                }
                this._regions[i].diffs = 0;
            }
            if (regionDiffArray.length > 0) {
                const data = {trigger: regionDiffArray, pam: chunk.pam};
                if (this._callback) {
                    this._callback(data);
                }
                if (this._readableState.pipesCount > 0) {
                    this.push(data);
                }
                if (this.listenerCount('diff') > 0) {
                    this.emit('diff', data);
                }
            }
        } else {
            const percent = Math.floor(100 * this._diffs / this._length);
            if (percent >= this._percent) {
                const data = {trigger: [{name: 'percent', percent: percent}], pam: chunk.pam};
                if (this._callback) {
                    this._callback(data);
                }
                if (this._readableState.pipesCount > 0) {
                    this.push(data);
                }
                if (this.listenerCount('diff') > 0) {
                    this.emit('diff', data);
                }
            }
            this._diffs = 0;
        }
        this._oldPix = this._newPix;
    }

    _rgbAlphaPixelDiff(chunk) {
        this._newPix = chunk.pixels;
        for (let y = 0, i = 0, p = 0; y < this._height; y++) {
            for (let x = 0; x < this._width; x++, i += 4, p++) {
                if (this._oldPix[i] !== this._newPix[i] || this._oldPix[i + 1] !== this._newPix[i + 1] || this._oldPix[i + 2] !== this._newPix[i + 2]) {
                    const diff = Math.abs(this._oldPix[i] + this._oldPix[i + 1] + this._oldPix[i + 2] - this._newPix[i] - this._newPix[i + 1] - this._newPix[i + 2])/3;
                    if (this._regions && diff >= this._minDiff) {
                        for (let j = 0; j < this._regionsLength; j++) {
                            if (this._pointsInPolygons[j][p] && diff >= this._regions[j].difference) {
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
        }
        if (this._regions) {
            const regionDiffArray = [];
            for (let i = 0; i < this._regionsLength; i++) {
                const percent = Math.floor(100 * this._regions[i].diffs / this._regions[i].pointsLength);
                if (percent >= this._regions[i].percent) {
                    regionDiffArray.push({name: this._regions[i].name, percent: percent});
                }
                this._regions[i].diffs = 0;
            }
            if (regionDiffArray.length > 0) {
                const data = {trigger: regionDiffArray, pam: chunk.pam};
                if (this._callback) {
                    this._callback(data);
                }
                if (this._readableState.pipesCount > 0) {
                    this.push(data);
                }
                if (this.listenerCount('diff') > 0) {
                    this.emit('diff', data);
                }
            }
        } else {
            const percent = Math.floor(100 * this._diffs / this._length);
            if (percent >= this._percent) {
                const data = {trigger: [{name: 'percent', percent: percent}], pam: chunk.pam};
                if (this._callback) {
                    this._callback(data);
                }
                if (this._readableState.pipesCount > 0) {
                    this.push(data);
                }
                if (this.listenerCount('diff') > 0) {
                    this.emit('diff', data);
                }
            }
            this._diffs = 0;
        }
        this._oldPix = this._newPix;
    }

    _parseFirstChunk(chunk) {
        this._width = parseInt(chunk.width);
        this._height = parseInt(chunk.height);
        this._oldPix = chunk.pixels;
        this._length = this._width * this._height;
        this._createPointsInPolygons(this._regions, this._width, this._height);
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
    }

    _transform(chunk, encoding, callback) {
        this._parseChunk(chunk);
        callback();
    }

    _flush(callback) {
        this.resetCache();
        callback();
    }
}

module.exports = PamDiff;
//todo get bounding box of all regions combined to exclude some pixels before checking if they exist inside specific regions