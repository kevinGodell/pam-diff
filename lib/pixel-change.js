'use strict';

class PixelChange {

    static compareGrayPixels(diff, wxh, bufLen, buf0, buf1) {
        let diffs = 0;
        for (let i = 0; i < bufLen; i++) {
            if(Math.abs(buf0[i] - buf1[i]) >= diff) {diffs++;}
        }
        return Math.floor(100 * diffs / wxh);
    }

    static compareRgbPixels(diff, wxh, bufLen, buf0, buf1) {
        let diffs = 0;
        for (let i = 0; i < bufLen; i+=3) {
            if(Math.abs(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) {diffs++;}
        }
        return Math.floor(100 * diffs / wxh);
    }

    static compareRgbaPixels(diff, wxh, bufLen, buf0, buf1) {
        let diffs = 0;
        for (let i = 0; i < bufLen; i+=4) {
            if(Math.abs(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) {diffs++;}
        }
        return Math.floor(100 * diffs / wxh);
    }

    static compareGrayRegions(minDiff, regLen, regions, bufLen, buf0, buf1) {
        const diffs = new Uint32Array(regLen);
        for (let p = 0; p < bufLen; p++) {
            const diff = Math.abs(buf0[p] - buf1[p]);
            if (minDiff > diff) {continue;}
            for (let i = 0; i < regLen; i++) {
                const region = regions[i];
                if (!region.bitset[p] || diff < region.diff) {continue;}
                diffs[i]++;
            }
        }
        const results = [];
        for (let i = 0; i < regLen; i++) {
            results.push({name: regions[i].name, percent: Math.floor(100 * diffs[i] / regions[i].count)});
        }
        return results;
    }

    static compareRgbRegions(minDiff, regLen, regions, bufLen, buf0, buf1) {
        const diffs = new Uint32Array(regLen);
        for (let i = 0, p = 0; i < bufLen; i+=3, p++) {
            const diff = Math.abs(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3;
            if (minDiff > diff) {continue;}
            for (let i = 0; i < regLen; i++) {
                const region = regions[i];
                if (!region.bitset[p] || diff < region.diff) {continue;}
                diffs[i]++;
            }
        }
        const results = [];
        for (let i = 0; i < regLen; i++) {
            results.push({name: regions[i].name, percent: Math.floor(100 * diffs[i] / regions[i].count)});
        }
        return results;
    }

    static compareRgbaRegions(minDiff, regLen, regions, bufLen, buf0, buf1) {
        const diffs = new Uint32Array(regLen);
        for (let i = 0, p = 0; i < bufLen; i+=4, p++) {
            const diff = Math.abs(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3;
            if (minDiff > diff) {continue;}
            for (let i = 0; i < regLen; i++) {
                const region = regions[i];
                if (!region.bitset[p] || diff < region.diff) {continue;}
                diffs[i]++;
            }
        }
        const results = [];
        for (let i = 0; i < regLen; i++) {
            results.push({name: regions[i].name, percent: Math.floor(100 * diffs[i] / regions[i].count)});
        }
        return results;
    }
}

module.exports = PixelChange;

//todo - should always return array
//todo - array will only be populated when percent >= percent
//todo - percent will have to be passed into method
//todo - will have to test this behavior on n-api module first to keep symmetry
//todo(ne) - check integrity of data before sending here to be processed