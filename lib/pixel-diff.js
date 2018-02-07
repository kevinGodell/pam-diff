'use strict';

class PixeDiff {

    static compareGrayPixels(width, height, diff, buf0, buf1) {
        const wxh = width * height;
        let diffs = 0;
        /*if (wxh !== buf0.length || wxh !== buf1.length) {
            throw new Error('ljlkjklj');
        }
        if (diff < 1 || diff > 255) {
            throw new Error('ljlkjklj');
        }*/
        for (let i = 0; i < wxh; i++) {
            if(Math.abs(buf0[i] - buf1[i]) >= diff) {diffs++;}
        }
        return Math.floor(100 * diffs / wxh);
    }

    static compareRgbPixels(width, height, diff, buf0, buf1) {
        const wxh = width * height;
        const len = wxh * 3;
        let diffs = 0;
        /*if (len !== buf0.length || len !== buf1.length) {
            throw new Error('ljlkjklj');
        }
        if (diff < 1 || diff > 255) {
            throw new Error('ljlkjklj');
        }*/
        for (let i = 0; i < len; i+=3) {
            if(Math.abs(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) {diffs++;}
        }
        return Math.floor(100 * diffs / wxh);
    }

    static compareRgbaPixels(width, height, diff, buf0, buf1) {
        const wxh = width * height;
        const len = wxh * 4;
        let diffs = 0;
        /*if (len !== buf0.length || len !== buf1.length) {
            throw new Error('ljlkjklj');
        }
        if (diff < 1 || diff > 255) {
            throw new Error('ljlkjklj');
        }*/
        for (let i = 0; i < len; i+=4) {
            if(Math.abs(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) {diffs++;}
        }
        return Math.floor(100 * diffs / wxh);
    }

    static compareGrayRegions(width, height, regions, buf0, buf1) {
        const regionsLength = regions.length;
        const diffs = new Uint32Array(regionsLength);
        let minDiff = 255;
        for (let i = 0; i < regionsLength; i++) {
            minDiff = Math.min(minDiff, regions[i].diff);
        }
        for (let y = 0, p = 0; y < height; y++) {
            for (let x = 0; x < width; x++, p++) {
                const diff = Math.abs(buf0[p] - buf1[p]);
                if (minDiff > diff) {continue;}
                for (let i = 0; i < regionsLength; i++) {
                    const region = regions[i];
                    if (!region.bitset[p] || diff < region.diff) {continue;}
                    diffs[i]++;
                }
            }
        }
        const results = [];
        for (let i = 0; i < regionsLength; i++) {
            results.push({name: regions[i].name, percent: Math.floor(100 * diffs[i] / regions[i].count)});
        }
        return results;
    }

    static compareRgbRegions(width, height, regions, buf0, buf1) {
        const regionsLength = regions.length;
        const diffs = new Uint32Array(regionsLength);
        let minDiff = 255;
        for (let i = 0; i < regionsLength; i++) {
            minDiff = Math.min(minDiff, regions[i].diff);
        }
        for (let y = 0, p = 0; y < height; y++) {
            for (let x = 0; x < width; x++, p+=3) {
                const diff = Math.abs(buf0[p] + buf0[p+1] + buf0[p+2] - buf1[p] - buf1[p+1] - buf1[p+2])/3;
                if (minDiff > diff) {continue;}
                for (let i = 0; i < regionsLength; i++) {
                    const region = regions[i];
                    if (!region.bitset[p] || diff < region.diff) {continue;}
                    diffs[i]++;
                }
            }
        }
        const results = [];
        for (let i = 0; i < regionsLength; i++) {
            results.push({name: regions[i].name, percent: Math.floor(100 * diffs[i] / regions[i].count)});
        }
        return results;
    }

    static compareRgbaRegions(width, height, regions, buf0, buf1) {
        const regionsLength = regions.length;
        const diffs = new Uint32Array(regionsLength);
        let minDiff = 255;
        for (let i = 0; i < regionsLength; i++) {
            minDiff = Math.min(minDiff, regions[i].diff);
        }
        for (let y = 0, p = 0; y < height; y++) {
            for (let x = 0; x < width; x++, p+=3) {
                const diff = Math.abs(buf0[p] + buf0[p+1] + buf0[p+2] - buf1[p] - buf1[p+1] - buf1[p+2])/3;
                if (minDiff > diff) {continue;}
                for (let i = 0; i < regionsLength; i++) {
                    const region = regions[i];
                    if (!region.bitset[p] || diff < region.diff) {continue;}
                    diffs[i]++;
                }
            }
        }
        const results = [];
        for (let i = 0; i < regionsLength; i++) {
            results.push({name: regions[i].name, percent: Math.floor(100 * diffs[i] / regions[i].count)});
        }
        return results;
    }
}

module.exports = PixeDiff;