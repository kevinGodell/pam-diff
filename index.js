'use strict';

const { Transform } = require('stream');

const PP = require('polygon-points');

const PC = require('pixel-change');

class PamDiff extends Transform {
  /**
   *
   * @param [options] {Object}
   * @param [options.debug=false] {Boolean} - If true, debug info will be logged to console
   * @param [options.difference=5] {Number} - Pixel difference value 1 to 255
   * @param [options.percent=5] {Number} - Percent of pixels or blobs that exceed difference value, float 0.0 to 100.0
   * @param [options.response=percent] {String} - Accepted values: percent or bounds or blobs
   * @param [options.draw=false] {Boolean} - If true and response is 'bounds' or 'blobs', return a pixel buffer with drawn bounding box
   * @param [options.regions] {Array} - Array of region objects
   * @param options.regions[i].name {String} - Name of region
   * @param [options.regions[i].difference=options.difference] {Number} - Difference value for region
   * @param [options.regions[i].percent=options.percent] {Number} - Percent value for region, float 0.0 to 100.0
   * @param options.regions[i].polygon {Array} - Array of x y coordinates [{x:0,y:0},{x:0,y:360},{x:160,y:360},{x:160,y:0}]
   * @param [options.mask=false] {Boolean} - Indicate if regions should be used as masks of pixels to ignore
   * @param [callback] {Function} - Function to be called when diff event occurs
   */
  constructor(options, callback) {
    super({ objectMode: true });
    this.debug = PamDiff._parseOptions('debug', options); // output debug info to console. defaults to false
    this.response = PamDiff._parseOptions('response', options); // percent, bounds, blobs
    this.draw = PamDiff._parseOptions('draw', options); // return pixels with bounding box if response is bounds or blobs
    this.difference = PamDiff._parseOptions('difference', options); // global value, can be overridden per region
    this.percent = PamDiff._parseOptions('percent', options); // global value, can be overridden per region
    this.mask = PamDiff._parseOptions('mask', options); // should be processed before regions
    this.regions = PamDiff._parseOptions('regions', options); // can be zero regions, a single region, or multiple regions. if no regions, all pixels will be compared.
    this.callback = callback; // callback function to be called when pixel difference is detected
    this._parseChunk = this._parseFirstChunk; // first parsing will be reading settings and configuring internal pixel reading
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
    return Number.isNaN(number) ? def : number < low ? low : number > high ? high : number;
  }

  /**
   *
   * @param bool
   * @return {boolean}
   * @private
   */
  static _validateBoolean(bool) {
    return bool === true || bool === 'true' || bool === 1 || bool === '1';
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
   * @param bool {Boolean}
   */
  set debug(bool) {
    this._debug = PamDiff._validateBoolean(bool);
    this._processRegions();
    this._configurePixelDiffEngine();
  }

  /**
   *
   * @return {Boolean}
   */
  get debug() {
    return this._debug;
  }

  /**
   *
   * @param bool {Boolean}
   * @return {PamDiff}
   */
  setDebug(bool) {
    this.debug = bool;
    return this;
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
    return this._response;
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
    return this._draw;
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
   * @param number {Number|String}
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
   * @param number {Number|String}
   */
  set percent(number) {
    this._percent = PamDiff._validateNumber(parseFloat(number), 5, 0, 100);
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
      this._regions = undefined;
      this._regionObj = undefined;
      // this._maskObj = undefined;
    } else if (!Array.isArray(array) || array.length < 1) {
      throw new Error(
        `Regions must be an array of at least 1 region object { name: 'region1', difference: 10, percent: 1, polygon: [{x: 0, y: 0}, {x: 0, y:50}, {x: 50, y:50}, {x: 50, y: 0}] }`
      );
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

  /**
   *
   * @param bool {Boolean}
   */
  set mask(bool) {
    this._mask = PamDiff._validateBoolean(bool);
    this._processRegions();
    this._configurePixelDiffEngine();
  }

  /**
   *
   * @return {Boolean}
   */
  get mask() {
    return this._mask;
  }

  /**
   *
   * @param bool {Boolean}
   * @return {PamDiff}
   */
  setMask(bool) {
    this.mask = bool;
    return this;
  }

  /**
   *
   * @param func {Function}
   */
  set callback(func) {
    if (!func) {
      this._callback = undefined;
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
    this._engineType = undefined;
    this._engine = undefined;
    this._oldPix = undefined;
    this._newPix = undefined;
    this._width = undefined;
    this._height = undefined;
    this._depth = undefined;
    this._tupltype = undefined;
    this._regionObj = undefined;
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
    const regions = [];
    if (this._mask === true) {
      // combine all regions to form a single region of flipped 0's and 1's
      let minX = this._width;
      let maxX = 0;
      let minY = this._height;
      let maxY = 0;
      const wxh = this._width * this._height;
      const maskBitset = Buffer.alloc(wxh, 1);
      for (const region of this._regions) {
        if (!region.hasOwnProperty('polygon')) {
          throw new Error('Region must include a polygon property');
        }
        const pp = new PP(region.polygon);
        const bitset = pp.getBitset(this._width, this._height);
        if (bitset.count === 0) {
          throw new Error('Bitset count must be greater than 0.');
        }
        const bitsetBuffer = bitset.buffer;
        for (let i = 0; i < wxh; ++i) {
          if (bitsetBuffer[i] === 1) {
            maskBitset[i] = 0;
          }
        }
      }
      let maskBitsetCount = 0;
      for (let i = 0; i < wxh; ++i) {
        if (maskBitset[i] === 1) {
          const y = Math.floor(i / this._width);
          const x = i % this._width;
          minX = Math.min(minX, x);
          maxX = Math.max(maxX, x);
          minY = Math.min(minY, y);
          maxY = Math.max(maxY, y);
          maskBitsetCount++;
        }
      }
      if (maskBitsetCount === 0) {
        throw new Error('Bitset count must be greater than 0');
      }
      regions.push({
        name: 'mask',
        bitset: maskBitset,
        bitsetCount: maskBitsetCount,
        difference: this._difference,
        percent: this._percent,
        minX: minX,
        maxX: maxX,
        minY: minY,
        maxY: maxY,
      });
    } else {
      for (const region of this._regions) {
        if (!region.hasOwnProperty('name') || !region.hasOwnProperty('polygon')) {
          throw new Error('Region must include a name and a polygon property');
        }
        const pp = new PP(region.polygon);
        const bitset = pp.getBitset(this._width, this._height);
        if (bitset.count === 0) {
          throw new Error('Bitset count must be greater than 0');
        }
        const difference = PamDiff._validateNumber(parseInt(region.difference), this._difference, 1, 255);
        const percent = PamDiff._validateNumber(parseFloat(region.percent), this._percent, 0, 100);
        regions.push({
          name: region.name,
          bitset: bitset.buffer,
          bitsetCount: bitset.count,
          difference: difference,
          percent: percent,
          minX: bitset.minX,
          maxX: bitset.maxX,
          minY: bitset.minY,
          maxY: bitset.maxY,
        });
      }
    }
    this._regionObj = { length: regions.length, regions: regions };
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
    const config = { width: this._width, height: this._height, depth: this._depth, response: this._response, sync: false };
    if (this._regionObj) {
      engine += '_region';
      // config.target = 'region';
      config.regions = this._regionObj.regions;
      if (this._regionObj.length > 1) {
        engine += 's';
      }
    } else {
      engine += '_all';
      // config.target = 'all';
      config.difference = this._difference;
      config.percent = this._percent;
    }
    engine += `_${this._response}`;
    if ((this._response === 'bounds' || this._response === 'blobs') && this._draw) {
      config.draw = this._draw;
      engine += '_draw';
    }
    engine += '_async';
    const pixelChange = PC(config);
    this._engine = pixelChange.compare.bind(pixelChange);
    this._engineType = engine;
    if (this._debug) {
      console.dir(this, { showHidden: false, depth: 0, colors: true });
      this._parseChunk = this._parsePixelsDebug;
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
    this._engine(this._oldPix, this._newPix, (err, data) => {
      const { results } = data;
      if (results.length) {
        const diff = { trigger: results, pam: chunk.pam, headers: chunk.headers, pixels: data.pixels || chunk.pixels };
        if (this._callback) {
          this._callback(diff);
        }
        if (this._readableState.pipesCount > 0) {
          this.push(diff);
        }
        if (this.listenerCount('diff') > 0) {
          this.emit('diff', diff);
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
    console.time(`${this._engineType}-${debugCount}`);
    this._newPix = chunk.pixels;
    this._engine(this._oldPix, this._newPix, (err, data) => {
      console.timeEnd(`${this._engineType}-${debugCount}`);
      const { results } = data;
      if (results.length) {
        const diff = { trigger: results, pam: chunk.pam, headers: chunk.headers, pixels: data.pixels || chunk.pixels };
        if (this._callback) {
          this._callback(diff);
        }
        if (this._readableState.pipesCount > 0) {
          this.push(diff);
        }
        if (this.listenerCount('diff') > 0) {
          this.emit('diff', diff);
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
  _parseFirstChunk(chunk) {
    this._width = parseInt(chunk.width);
    this._height = parseInt(chunk.height);
    this._depth = parseInt(chunk.depth);
    this._oldPix = chunk.pixels;
    this._tupltype = chunk.tupltype;
    this._processRegions();
    this._configurePixelDiffEngine();
    this.emit('initialized', { width: this._width, height: this._height, depth: this._depth, tupltype: this._tupltype });
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
