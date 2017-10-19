// jshint esversion: 6, globalstrict: true, strict: true
'use strict';

console.time('=====> testing pam diffs with 4 regions set');

const assert = require('assert');

const P2P = require('pipe2pam');

const PamDiff = require('../index');

const spawn = require('child_process').spawn;

const pamCount = 10;

let pamCounter = 0;

let pamDiffCounter = 0;

const pamDiffResults = [13, 14, 12, 13, 13, 12, 14, 13, 12];

const params = [
    /* log info to console */
    '-loglevel',
    'quiet',
    '-stats',
    
    /* use an artificial video input */
    '-re',
    '-f',
    'lavfi',
    '-i',
    'testsrc=size=1920x1080:rate=15',

    /* set output flags */
    '-an',
    '-c:v',
    'pam',
    '-pix_fmt',
    'rgb24',
    '-f',
    'image2pipe',
    '-vf',
    'fps=1,scale=320:180',
    '-frames',
    pamCount,
    'pipe:1'
];

const p2p = new P2P();

p2p.on('pam', (data) => {
    pamCounter++;
});

const region1 = {name: 'region1', difference: 1, percent: 1, polygon: [[0, 0], [0, 225], [100, 225], [100, 0]]};

const region2 = {name: 'region2', difference: 1, percent: 1, polygon: [[100, 0], [100, 225], [200, 225], [200, 0]]};

const region3 = {name: 'region3', difference: 1, percent: 1, polygon: [[200, 0], [200, 225], [300, 225], [300, 0]]};

const region4 = {name: 'region4', difference: 1, percent: 1, polygon: [[300, 0], [300, 225], [400, 225], [400, 0]]};

const regions = [region1, region2, region3, region4];

const pamDiff = new PamDiff({grayscale: 'average', regions : regions});

pamDiff.on('diff', (data) => {
    assert(data.trigger[2].name === 'region3', 'trigger name is not correct');
    assert(data.trigger[2].percent === pamDiffResults[pamDiffCounter++], 'trigger percent is not correct');
});

const ffmpeg = spawn('ffmpeg', params, {stdio: ['ignore', 'pipe', 'inherit']});

ffmpeg.on('error', (error) => {
    console.log(error);
});

ffmpeg.on('exit', (code, signal) => {
    assert(code === 0, `FFMPEG exited with code ${code} and signal ${signal}`);
    assert(pamDiffCounter === pamCount - 1, `did not get ${pamCount - 1} pam diffs`);
    console.timeEnd('=====> testing pam diffs with 4 regions set');
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);