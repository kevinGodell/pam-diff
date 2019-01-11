'use strict';

const dotenv = require('dotenv');

dotenv.config();

const {cpus} = require('os');

console.log(`cpu cores available: ${cpus().length}`);

console.time('=====> testing gray pam diffs with 2 region masks set');

const assert = require('assert');

const P2P = require('pipe2pam');

const PamDiff = require('../index');

const ffmpegPath = require('ffmpeg-static').path;

const spawn = require('child_process').spawn;

const async = process.env.ASYNC|| false;

const pamCount = 10;

let pamCounter = 0;

let pamDiffCounter = 0;

const pamDiffResults = [15, 16, 14, 15, 15, 14, 16, 15, 14];

const params = [
    /* log info to console */
    '-loglevel',
    'quiet',
    //'-stats',
    
    /* use an artificial video input */
    //'-re',
    '-f',
    'lavfi',
    '-i',
    'testsrc=size=1920x1080:rate=15',

    /* set output flags */
    '-an',
    '-c:v',
    'pam',
    '-pix_fmt',
    'gray',
    '-f',
    'image2pipe',
    '-vf',
    'fps=1,scale=400:225',
    '-frames',
    pamCount,
    'pipe:1'
];

const p2p = new P2P();

p2p.on('pam', data => {
    pamCounter++;
});

const region1 = {polygon: [{x: 0, y: 0}, {x: 0, y: 225}, {x: 100, y: 225}, {x: 100, y: 0}]};

const region2 = {polygon: [{x: 100, y: 0}, {x: 100, y: 225}, {x: 200, y: 225}, {x: 200, y: 0}]};

const regions = [region1, region2];

const pamDiff = new PamDiff({difference: 1, percent: 1, mask: true, regions : regions, async: async, response: 'bounds'});

pamDiff.on('diff', data => {
    assert(data.trigger[0].name === 'mask', 'trigger name is not correct');
    assert(data.trigger[0].percent === pamDiffResults[pamDiffCounter++], 'trigger percent is not correct');
});

const ffmpeg = spawn(ffmpegPath, params, {stdio: ['ignore', 'pipe', 'inherit']});

ffmpeg.on('error', error => {
    console.log(error);
});

ffmpeg.on('exit', (code, signal) => {
    assert(code === 0, `FFMPEG exited with code ${code} and signal ${signal}`);
    setTimeout(()=> {
        assert(pamDiffCounter === pamCount - 1, `did not get ${pamCount - 1} pam diffs`);
        console.timeEnd('=====> testing gray pam diffs with 2 region masks set');
    }, 100);
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);