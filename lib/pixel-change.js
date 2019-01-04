'use strict';

class PixelChange {

    static compareGrayPixels(diff, percent, wxh, bufLen, buf0, buf1) {
        let diffs = 0;
        for (let i = 0; i < bufLen; i++) {if(Math.abs(buf0[i] - buf1[i]) >= diff) {diffs++;}}
        const perc = Math.floor(100 * diffs / wxh);
        if (perc >= percent) {return [{name: "all", percent: perc}];}
        return [];
    }

    static compareRgbPixels(diff, percent, wxh, bufLen, buf0, buf1) {
        let diffs = 0;
        for (let i = 0; i < bufLen; i+=3) {if(Math.abs(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) {diffs++;}}
        const perc = Math.floor(100 * diffs / wxh);
        if (perc >= percent) {return [{name: "all", percent: perc}];}
        return [];
    }

    static compareRgbaPixels(diff, percent, wxh, bufLen, buf0, buf1) {
        let diffs = 0;
        for (let i = 0; i < bufLen; i+=4) {if(Math.abs(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) {diffs++;}}
        const perc = Math.floor(100 * diffs / wxh);
        if (perc >= percent) {return [{name: "all", percent: perc}];}
        return [];
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
            const percent = Math.floor(100 * diffs[i] / regions[i].count);
            if (regions[i].percent > percent) {continue;}
            results.push({name: regions[i].name, percent: percent});
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
            const percent = Math.floor(100 * diffs[i] / regions[i].count);
            if (regions[i].percent > percent) {continue;}
            results.push({name: regions[i].name, percent: percent});
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
            const percent = Math.floor(100 * diffs[i] / regions[i].count);
            if (regions[i].percent > percent) {continue;}
            results.push({name: regions[i].name, percent: percent});
        }
        return results;
    }

    static compareGrayMask(diff, percent, count, bitset, bufLen, buf0, buf1) {
        let diffs = 0;
        for (let i = 0; i < bufLen; i++) {if(bitset[i] && Math.abs(buf0[i] - buf1[i]) >= diff) {diffs++;}}
        const perc = Math.floor(100 * diffs / count);
        if (perc >= percent) {return [{name: "mask", percent: perc}];}
        return [];
    }

    static compareRgbMask(diff, percent, count, bitset, bufLen, buf0, buf1) {
        let diffs = 0;
        for (let i = 0, p = 0; i < bufLen; i+=3, p++) {if(bitset[p] && Math.abs(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) {diffs++;}}
        const perc = Math.floor(100 * diffs / count);
        if (perc >= percent) {return [{name: "mask", percent: perc}];}
        return [];
    }

    static compareRgbaMask(diff, percent, count, bitset, bufLen, buf0, buf1) {
        let diffs = 0;
        for (let i = 0, p = 0; i < bufLen; i+=4, p++) {if(bitset[p] && Math.abs(buf0[i] + buf0[i+1] + buf0[i+2] - buf1[i] - buf1[i+1] - buf1[i+2])/3 >= diff) {diffs++;}}
        const perc = Math.floor(100 * diffs / count);
        if (perc >= percent) {return [{name: "mask", percent: perc}];}
        return [];
    }

}

module.exports = PixelChange;