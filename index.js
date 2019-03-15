'use strict';

const {Transform} = require('stream');

const PP = require('polygon-points');

const addon = require('bindings')('addon');

class PamDiff extends Transform {
    /**
     *
     * @param [options] {Object}
     * @param [options.async=false] {Boolean} - Pixel change detection will be sent to a libuv worker thread.
     * @param [options.difference=5] {Number} - Pixel difference value 1 to 255
     * @param [options.percent=5] {Number} - Percent of pixels that exceed difference value.
     * @param [options.response=percent] {String} - Accepted values: percent or bounds.
     * @param [options.draw=false] {Boolean} - Return a pixel buffer with drawn bounding box. Must also set {response: 'bounds'}.
     * @param [options.regions] {Array} - Array of regions.
     * @param options.regions[i].name {String} - Name of region.
     * @param [options.regions[i].difference=options.difference] {Number} - Difference value for region.
     * @param [options.regions[i].percent=options.percent] {Number} - Percent value for region.
     * @param options.regions[i].polygon {Array} - Array of x y coordinates [{x:0,y:0},{x:0,y:360},{x:160,y:360},{x:160,y:0}]
     * @param [options.mask=false] {Boolean} - Indicate if regions should be used as masks of pixels to ignore.
     * @param [callback] {Function} - Function to be called when diff event occurs.
     */
    //[options.blob] {Number} - Number of pixels connected at top, right, bottom, left to be considered a blob.
    constructor(options, callback) {
        super(options);
        Transform.call(this, {objectMode: true});
        this.response = PamDiff._parseOptions('response', options);//percent, bounds, blobs
        this.draw = PamDiff._parseOptions('draw', options);// return pixels with bounding box if response is bounds
        this.async = PamDiff._parseOptions('async', options);// should be processed before regions
        this.difference = PamDiff._parseOptions('difference', options);// global value, can be overridden per region
        this.percent = PamDiff._parseOptions('percent', options);// global value, can be overridden per region
        this.mask = PamDiff._parseOptions('mask', options);// should be processed before regions
        //this.blobSize = PamDiff._parseOptions('blobSize', options);// should be processed before regions
        this.regions = PamDiff._parseOptions('regions', options);// can be zero regions, a single region, or multiple regions. if no regions, all pixels will be compared.
        this.callback = callback;// callback function to be called when pixel difference is detected
        this._parseChunk = this._parseFirstChunk;// first parsing will be reading settings and configuring internal pixel reading
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
     * @param bool
     * @return {boolean}
     * @private
     */
    static _validateBoolean(bool) {
        return (bool === true || bool === 'true' || bool === 1 || bool === '1');
    }

    /**
     *
     * @param string {String}
     * @param strings {Array}
     * @return {String}
     * @private
     */
    static _validateString(string, strings) {
        if (strings.includes(string)) {
            return string;
        }
        return strings[0];
    }

    /**
     *
     * @param string {String}
     */
    set response(string) {
        this._response = PamDiff._validateString(string, ['percent', 'bounds', 'blobs']);
        this._processRegions();
        this._configurePixelDiffEngine();
    }

    /**
     *
     * @return {String}
     */
    get response() {
        return this._response || null;
    }

    /**
     *
     * @param string {String}
     * @return {PamDiff}
     */
    setResponse(string) {
        this.response = string;
        return this;
    }

    /**
     *
     * @param bool {Boolean}
     */
    set async(bool) {
        this._async = PamDiff._validateBoolean(bool);
        this._processRegions();
        this._configurePixelDiffEngine();
    }

    /**
     *
     * @return {Boolean}
     */
    get async() {
        return this._async || false;
    }

    /**
     *
     * @param bool {Boolean}
     * @return {PamDiff}
     */
    setAsync(bool) {
        this.async = bool;
        return this;
    }

    /**
     *
     * @param bool {Boolean}
     */
    set draw(bool) {
        this._draw = PamDiff._validateBoolean(bool);
        this._processRegions();
        this._configurePixelDiffEngine();
    }

    /**
     *
     * @return {Boolean}
     */
    get draw() {
        return this._draw || false;
    }

    /**
     *
     * @param bool {Boolean}
     * @return {PamDiff}
     */
    setDraw(bool) {
        this.draw = bool;
        return this;
    }

    /**
     *
     * @param number {Number}
     */
    set difference(number) {
        this._difference = PamDiff._validateNumber(parseInt(number), 5, 1, 255);
        this._configurePixelDiffEngine();
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
        this._configurePixelDiffEngine();
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
            delete this._regionObj;
            delete this._maskObj;
        } else if (!Array.isArray(array) || array.length < 1) {
            throw new Error(`Regions must be an array of at least 1 region object {name: 'region1', difference: 10, percent: 10, polygon: [[0, 0], [0, 50], [50, 50], [50, 0]]}`);
        } else {
            this._regions = array;
            this._processRegions();
        }
        this._configurePixelDiffEngine();
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

    set mask(bool) {
        this._mask = PamDiff._validateBoolean(bool);
        this._processRegions();
        this._configurePixelDiffEngine();
    }

    get mask() {
        return this._mask;
    }

    setMask(bool) {
        this.mask = bool;
        return this;
    }

    /**
     *
     * @param number {Number}
     */
    set blobSize(number) {
        this._blobSize = PamDiff._validateNumber(parseInt(number), 0, 0, 100000);
        this._configurePixelDiffEngine();
    }

    /**
     *
     * @return {Number}
     */
    get blobSize() {
        return this._blobSize || 0;
    }

    /**
     *
     * @param number
     * @return {PamDiff}
     */
    setBlobSize(number) {
        this.blobSize = number;
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
        delete this._engine;
        delete this._oldPix;
        delete this._newPix;
        delete this._width;
        delete this._height;
        delete this._depth;
        delete this._tupltype;
        delete this._regionObj;
        delete this._maskObj;
        this._parseChunk = this._parseFirstChunk;
        return this;
    }

    /**
     *
     * @private
     */
    _processRegions() {
        if (!this._regions || !this._width || !this._height) {
            return;
        }
        if (this._mask) {
            let minX = this._width;
            let maxX = 0;
            let minY = this._height;
            let maxY = 0;
            const wxh = this._width * this._height;
            const buffer = Buffer.alloc(wxh, 1);
            for (const region of this._regions) {
                if (!region.hasOwnProperty('polygon')) {
                    throw new Error('Region must include a polygon property');
                }
                const pp = new PP(region.polygon);
                const bitset = pp.getBitset(this._width, this._height);
                const bitsetBuffer = bitset.buffer;
                for (let i = 0; i < wxh; i++) {
                    if (bitsetBuffer[i]) {
                        buffer[i] = 0;
                    }
                }
            }
            let count = 0;
            for (let i = 0; i < wxh; i++) {
                if (buffer[i]) {
                    const y = Math.floor(i / this._width);
                    const x = i % this._width;
                    minX = Math.min(minX, x);
                    maxX = Math.max(maxX, x);
                    minY = Math.min(minY, y);
                    maxY = Math.max(maxY, y);
                    count++;
                }
            }
            if (count === 0) {
                throw new Error('Bitset count must be greater than 0');
            }
            this._maskObj = {count: count, bitset: buffer, minX: minX, maxX: maxX, minY: minY, maxY: maxY};
        } else {
            const regions = [];
            let minDiff = 255;
            let minX = this._width;
            let maxX = 0;
            let minY = this._height;
            let maxY = 0;
            const wxh = this._width * this._height;
            const mask = Buffer.alloc(wxh, 0);
            for (const region of this._regions) {
                if (!region.hasOwnProperty('name') || !region.hasOwnProperty('polygon')) {
                    throw new Error('Region must include a name and a polygon property');
                }
                const pp = new PP(region.polygon);
                const bitset = pp.getBitset(this._width, this._height);
                for(let i = 0; i < wxh; ++i) {
                    if (bitset[i]) {
                        mask[i] = 1;// todo should be called combined bitset
                    }
                }
                const difference = PamDiff._validateNumber(parseInt(region.difference), this._difference, 1, 255);
                const percent = PamDiff._validateNumber(parseInt(region.percent), this._percent, 1, 100);
                minDiff = Math.min(minDiff, difference);
                minX = Math.min(minX, bitset.minX);
                maxX = Math.max(maxX, bitset.maxX);
                minY = Math.min(minY, bitset.minY);
                maxY = Math.max(maxY, bitset.maxY);
                regions.push(
                    {
                        name: region.name,
                        diff: difference,
                        percent: percent,
                        count: bitset.count,
                        bitset: bitset.buffer,
                        minX: bitset.minX,
                        maxX: bitset.maxX,
                        minY: bitset.minY,
                        maxY: bitset.maxY
                    }
                );
            }
            this._regionObj = {mask: mask, minDiff: minDiff, minX: minX, maxX: maxX, minY: minY, maxY: maxY, length: regions.length, regions: regions};
        }
    }

    /**
     *
     * @private
     */
    _configurePixelDiffEngine() {
        if (!this._tupltype || !this._width || !this._height) {
            return;
        }
        let engine = this._tupltype;
        engine += `_${this._width}_x_${this._height}`;
        const config = {width: this._width, height: this._height, depth: this._depth, response: this._response, async: this._async};
        if (this._regionObj) {
            engine += '_regions';
            config.target = 'regions';
            config.mask = this._regionObj.mask;
            config.minDiff = this._regionObj.minDiff;
            config.minX = this._regionObj.minX;
            config.maxX = this._regionObj.maxX;
            config.minY = this._regionObj.minY;
            config.maxY = this._regionObj.maxY;
            config.regions = this._regionObj.regions;
        } else if (this._maskObj) {
            engine += '_mask';
            config.target = 'mask';
            config.difference = this._difference;
            config.percent = this._percent;
            config.bitsetCount = this._maskObj.count;
            config.bitset = this._maskObj.bitset;
            config.minX = this._maskObj.minX;
            config.maxX = this._maskObj.maxX;
            config.minY = this._maskObj.minY;
            config.maxY = this._maskObj.maxY;
        } else {
            engine += '_all';
            config.target = 'all';
            config.difference = this._difference;
            config.percent = this._percent;
        }
        engine += `_${this._response}`;
        if ((this._response === 'bounds' || this._response === 'blobs') && this._draw) {
            config.draw = this._draw;
            engine += '_draw';
        }
        engine += this._async ? '_async' : '_sync';
        this._engine = addon(config);
        if (process.env.NODE_ENV === 'development') {
            this._parseChunk = this._parsePixelsDebug;
            this._debugEngine = engine;
            this._debugCount = 0;
        } else {
            this._parseChunk = this._parsePixels;
        }
    }

    /**
     *
     * @param chunk
     * @private
     */
    _parsePixels(chunk) {
        this._newPix = chunk.pixels;
        this._engine.compare(this._oldPix, this._newPix, (err, results, pixels) => {
            if (results.length) {
                const data = {trigger: results, pam: chunk.pam, headers: chunk.headers};
                if (pixels) {
                    data.pixels = pixels;
                } else {
                    data.pixels = chunk.pixels;
                }
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
        });
        this._oldPix = this._newPix;
    }

    /**
     *
     * @param chunk
     * @private
     */
    _parsePixelsDebug(chunk) {
        const debugCount = this._debugCount++;
        console.time(`${this._debugEngine}-${debugCount}`);
        this._newPix = chunk.pixels;
        this._engine.compare(this._oldPix, this._newPix, (err, results, pixels) => {
            if (results.length) {
                const data = {trigger: results, pam: chunk.pam, headers: chunk.headers};
                if (pixels) {
                    data.pixels = pixels;
                } else {
                    data.pixels = chunk.pixels;
                }
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
            console.timeEnd(`${this._debugEngine}-${debugCount}`);
        });
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
        this._depth = parseInt(chunk.depth);
        this._oldPix = chunk.pixels;
        this._tupltype = chunk.tupltype;
        this._processRegions();
        this._configurePixelDiffEngine();
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