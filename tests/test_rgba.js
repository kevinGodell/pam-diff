'use strict';

const dotenv = require('dotenv');

dotenv.config();

const argv = require('minimist')(process.argv.slice(2));

const async = argv.async || process.env.ASYNC || false;

const response = argv.response || process.env.RESPONSE || 'percent';

const {cpus} = require('os');

console.log(`cpu cores available: ${cpus().length}`);

console.time('=====> testing rgba pam diffs with no region set');

const assert = require('assert');

const P2P = require('pipe2pam');

const PamDiff = require('../index');

const ffmpegPath = require('ffmpeg-static').path;

const spawn = require('child_process').spawn;

const pamCount = 10;

let pamCounter = 0;

let pamDiffCounter = 0;

const pamDiffResults = [14, 14, 13, 13, 13, 13, 14, 14, 13];

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
    'rgba',
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

const pamDiff = new PamDiff({difference: 1, percent: 1, async: async, response: response});

pamDiff.on('diff', data => {
    assert(data.trigger[0].name === 'all', 'trigger name is not correct');
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
        console.timeEnd('=====> testing rgba pam diffs with no region set');
    }, 100);
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);