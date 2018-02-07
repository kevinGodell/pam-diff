'use strict';

const { Transform } = require('stream');

const PP = require('polygon-points');

//const PC = require('./lib/pixel-change');

const PC = require('./build/Release/pixel_change');

class PamDiff extends Transform {

    /**
     *
     * @param [options] {Object}
     * @param [callback] {Function}
     */
    constructor(options, callback) {
        super(options);
        Transform.call(this, {objectMode: true});
        this.difference = PamDiff._parseOptions('difference', options);//global option, can be overridden per region
        this.percent = PamDiff._parseOptions('percent', options);//global option, can be overridden per region
        this.regions = PamDiff._parseOptions('regions', options);//can be no regions or a single region or multiple regions. if no regions, all pixels will be compared.
        this.callback = callback;//callback function to be called when pixel difference is detected
        this._parseChunk = this._parseFirstChunk;//first parsing will be reading settings and configuring internal pixel reading
    }

    /**
     *
     * @param option {String}
     * @param options {Object}
     * @return {*}
     * @private
     */
    static _parseOptions(option, options) {
        if (options && options.hasOwnProperty(option)) {
            return options[option];
        }
        return null;
    }

    /**
     *
     * @param number {Number}
     * @param def {Number}
     * @param low {Number}
     * @param high {Number}
     * @return {Number}
     * @private
     */
    static _validateNumber(number, def, low, high) {
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

    /**
     *
     * @deprecated
     * @param string {String}
     */
    setGrayscale(string) {
        console.warn('grayscale option has been removed, "average" has proven to most accurate and is the default');
    }

    /**
     *
     * @param number {Number}
     */
    set difference(number) {
        this._difference = PamDiff._validateNumber(parseInt(number), 5, 1, 255);
    }

    /**
     *
     * @return {Number}
     */
    get difference() {
        return this._difference;
    }

    /**
     *
     * @param number {Number}
     * @return {PamDiff}
     */
    setDifference(number) {
        this.difference = number;
        return this;
    }

    /**
     *
     * @param number {Number}
     */
    set percent(number) {
        this._percent = PamDiff._validateNumber(parseInt(number), 5, 1, 100);
    }

    /**
     *
     * @return {Number}
     */
    get percent() {
        return this._percent;
    }

    /**
     *
     * @param number {Number}
     * @return {PamDiff}
     */
    setPercent(number) {
        this.percent = number;
        return this;
    }

    /**
     *
     * @param array {Array}
     */
    set regions(array) {
        if (!array) {
            delete this._regions;
            delete this._regionsArr;
            delete this._regionsLen;
            delete this._minDiff;
        } else if (!Array.isArray(array) || array.length < 1) {
            throw new Error(`Regions must be an array of at least 1 region object {name: 'region1', difference: 10, percent: 10, polygon: [[0, 0], [0, 50], [50, 50], [50, 0]]}`);
        } else {
            this._regions = array;
            this._processRegions();
        }
    }

    /**
     *
     * @return {Array}
     */
    get regions() {
        return this._regions;
    }

    /**
     *
     * @param array {Array}
     * @return {PamDiff}
     */
    setRegions(array) {
        this.regions = array;
        return this;
    }

    /**
     *
     * @param func {Function}
     */
    set callback(func) {
        if (!func) {
            delete this._callback;
        } else if (typeof func === 'function' && func.length === 1) {
            this._callback = func;
        } else {
            throw new Error('Callback must be a function that accepts 1 argument.');
        }
    }

    /**
     *
     * @return {Function}
     */
    get callback() {
        return this._callback;
    }

    /**
     *
     * @param func {Function}
     * @return {PamDiff}
     */
    setCallback(func) {
        this.callback = func;
        return this;
    }

    /**
     *
     * @return {PamDiff}
     */
    resetCache() {
        delete this._oldPix;
        delete this._newPix;
        delete this._width;
        delete this._height;
        delete this._wxh;
        delete this._bufLen;
        delete this._regionsArr;
        delete this._regionsLen;
        delete this._minDiff;
        this._parseChunk = this._parseFirstChunk;
        return this;
    }

    /**
     *
     * @param chunk
     * @private
     */
    _bwPixelDiff(chunk) {
        throw new Error("black and white pixel diff not available, yet");
    }

    /**
     *
     * @param chunk
     * @private
     */
    _grayPixelDiff(chunk) {
        this._newPix = chunk.pixels;
        let results = [];
        if (this._regions) {
            results = PC.compareGrayRegions(this._minDiff, this._regionsLen, this._regionsArr, this._bufLen, this._oldPix, this._newPix);
        } else {
            results = PC.compareGrayPixels(this._difference, this._percent, this._wxh, this._bufLen, this._oldPix, this._newPix);
        }
        if (results.length) {
            const data = {trigger: results, pam:chunk.pam};
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
        this._oldPix = this._newPix;
    }

    _rgbPixelDiff(chunk) {
        this._newPix = chunk.pixels;
        let results = [];
        if (this._regions) {
            results = PC.compareRgbRegions(this._minDiff, this._regionsLen, this._regionsArr, this._bufLen, this._oldPix, this._newPix);
        } else {
            results = PC.compareRgbPixels(this._difference, this._percent, this._wxh, this._bufLen, this._oldPix, this._newPix);
        }
        if (results.length) {
            const data = {trigger: results, pam:chunk.pam};
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
        this._oldPix = this._newPix;
    }

    /**
     *
     * @param chunk
     * @private
     */
    _rgbaPixelDiff(chunk) {
        this._newPix = chunk.pixels;
        let results;
        if (this._regions) {
            results = PC.compareRgbaRegions(this._minDiff, this._regionsLen, this._regionsArr, this._bufLen, this._oldPix, this._newPix);
        } else {
            results = PC.compareRgbaPixels(this._difference, this._percent, this._wxh, this._bufLen, this._oldPix, this._newPix);
        }
        if (results.length) {
            const data = {trigger: results, pam:chunk.pam};
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
        this._oldPix = this._newPix;
    }

    _processRegions() {
        if (this._regions && this._width && this._height) {
            this._regionsArr = [];
            this._minDiff = 255;
            for (const region of this._regions) {
                if (!region.hasOwnProperty('name') || !region.hasOwnProperty('polygon')) {
                    throw new Error('Region must include a name and a polygon property');
                }
                const polygonPoints = new PP(region.polygon);
                const bitset = polygonPoints.getBitset(this._width, this._height);
                const difference = PamDiff._validateNumber(parseInt(region.difference), this._difference, 1, 255);
                const percent = PamDiff._validateNumber(parseInt(region.percent), this._percent, 1, 100);
                this._minDiff = Math.min(this._minDiff, difference);
                this._regionsArr.push(
                    {
                        name: region.name,
                        diff: difference,
                        percent: percent,
                        count: bitset.count,
                        bitset: bitset.buffer
                    }
                );
            }
            this._regionsLen = this._regions.length;
        }
    }

    /**
     *
     * @param chunk
     * @private
     */
    _parseFirstChunk(chunk) {
        this._width = parseInt(chunk.width);
        this._height = parseInt(chunk.height);
        this._oldPix = chunk.pixels;
        this._wxh = this._width * this._height;
        this._processRegions();
        switch (chunk.tupltype) {
            case 'blackandwhite' :
                this._bufLen = this._wxh;
                if (this._bufLen !== this._oldPix.length) {
                    throw new Error("Pixel count does not match width * height");
                }
                this._parseChunk = this._bwPixelDiff;
                break;
            case 'grayscale' :
                this._bufLen = this._wxh;
                if (this._bufLen !== this._oldPix.length) {
                    throw new Error("Pixel count does not match width * height");
                }
                this._parseChunk = this._grayPixelDiff;
                break;
            case 'rgb' :
                this._bufLen = this._wxh * 3;
                if (this._bufLen !== this._oldPix.length) {
                    throw new Error("Pixel count does not match width * height * 3");
                }
                this._parseChunk = this._rgbPixelDiff;
                break;
            case 'rgb_alpha' :
                this._bufLen = this._wxh * 4;
                if (this._bufLen !== this._oldPix.length) {
                    throw new Error("Pixel count does not match width * height * 4");
                }
                this._parseChunk = this._rgbaPixelDiff;
                break;
            default :
                throw Error(`Unsupported tupltype: ${chunk.tupltype}. Supported tupltypes include grayscale(gray), blackandwhite(monob), rgb(rgb24), and rgb_alpha(rgba).`);
        }
    }

    /**
     *
     * @param chunk
     * @param encoding
     * @param callback
     * @private
     */
    _transform(chunk, encoding, callback) {
        this._parseChunk(chunk);
        callback();
    }

    /**
     *
     * @param callback
     * @private
     */
    _flush(callback) {
        this.resetCache();
        callback();
    }
}

/**
 *
 * @type {PamDiff}
 */
module.exports = PamDiff;