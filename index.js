'use strict';

const { Transform } = require('stream');

const PP = require('polygon-points');

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
            if (this._regions) {
                delete this._regions;
                delete this._regionsLength;
                delete this._minDiff;
            }
            this._diffs = 0;
        } else if (!Array.isArray(array) || array.length < 1) {
            throw new Error(`Regions must be an array of at least 1 region object {name: 'region1', difference: 10, percent: 10, polygon: [[0, 0], [0, 50], [50, 50], [50, 0]]}`);
        } else {
            this._regions = [];
            this._minDiff = 255;
            for (const region of array) {
                if (!region.hasOwnProperty('name') || !region.hasOwnProperty('polygon')) {
                    throw new Error('Region must include a name and a polygon property');
                }
                const polygonPoints = new PP(region.polygon);
                const difference = PamDiff._validateNumber(parseInt(region.difference), this._difference, 1, 255);
                const percent = PamDiff._validateNumber(parseInt(region.percent), this._percent, 1, 100);
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
        //delete this._oldPix;
        //delete this._newPix;
        //delete this._width;
        //delete this._length;
        this._parseChunk = this._parseFirstChunk;
        return this;
    }

    /**
     *
     * @param regions {Array}
     * @param width {Number}
     * @param height {Number}
     * @private
     */
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

    /**
     *
     * @param chunk
     * @private
     */
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

    /**
     *
     * @param chunk
     * @private
     */
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

    /**
     *
     * @param chunk
     * @private
     */
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

    /**
     *
     * @param chunk
     * @private
     */
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

    /**
     *
     * @param chunk
     * @private
     */
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
//todo get bounding box of all regions combined to exclude some pixels before checking if they exist inside specific regions