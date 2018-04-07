'use strict';

process.env.NODE_ENV = 'development';

console.time('=====> testing gray pam blob diffs with no region set');

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
    '-re',
    '-f',
    'lavfi',
    '-i',
    'testsrc=size=1920x1080:rate=15',

    //'-rtsp_transport', 'tcp',
    //'-i', 'rtsp://192.168.1.22:554/user=admin_password=pass_channel=1_stream=0.sdp',

    /* set output flags */
    '-an',
    '-c:v',
    'pam',
    '-pix_fmt',
    'gray',
    '-f',
    'image2pipe',
    '-vf',
    'fps=1,scale=640:360',
    '-frames',
    pamCount,
    'pipe:1'
];

const p2p = new P2P();

p2p.on('pam', (data) => {
    pamCounter++;
});

const pamDiff = new PamDiff({difference: 1, percent: 1, blobSize: 1000});

pamDiff.on('diff', (data) => {
    if (pamDiffCounter === 1) {
        assert(data.trigger[0].blobs[1].label === 6);
        assert(data.trigger[0].blobs[1].size === 6169);
        assert(data.trigger[0].blobs[1].minX === 60);
        assert(data.trigger[0].blobs[1].maxX === 186);
        assert(data.trigger[0].blobs[1].minY === 268);
        assert(data.trigger[0].blobs[1].maxY === 316);
    }
    assert(data.trigger[0].name === 'all', 'trigger name is not correct');
    assert(data.trigger[0].percent === pamDiffResults[pamDiffCounter++], 'trigger percent is not correct');
});

const ffmpeg = spawn(ffmpegPath, params, {stdio: ['ignore', 'pipe', 'inherit']});

ffmpeg.on('error', (error) => {
    console.log(error);
});

ffmpeg.on('exit', (code, signal) => {
    assert(code === 0, `FFMPEG exited with code ${code} and signal ${signal}`);
    assert(pamDiffCounter === pamCount - 1, `did not get ${pamCount - 1} pam diffs`);
    console.timeEnd('=====> testing gray pam blob diffs with no region set');
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);
