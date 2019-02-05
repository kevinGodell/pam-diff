'use strict';

const {config:dotenvConfig} = require('dotenv');

dotenvConfig();

const argv = require('minimist')(process.argv.slice(2));

const async = argv.async || process.env.ASYNC || false;

const response = argv.response || process.env.RESPONSE || 'percent';

const draw = argv.draw || process.env.DRAW || false;

const {cpus} = require('os');

console.log(`cpu cores available: ${cpus().length}`);

const P2P = require('pipe2pam');
const PamDiff = require('../index');
const {path:ffmpegPath} = require('ffmpeg-static');
const {spawn, execFile} = require('child_process');

const params = [
    '-loglevel',
    'quiet',

    /* use hardware acceleration */
    '-hwaccel',
    'auto',//vda, videotoolbox, none, auto

    /* use a pre-recorded mp4 video as input */
    //'-re',//comment out to have ffmpeg read video as fast as possible
    '-i',
    `${__dirname}/in/circle_star.mp4`,

    /* set output flags */
    '-an',
    '-c:v',
    'pam',
    '-pix_fmt',
    'rgb24',
    '-f',
    'image2pipe',
    '-vf',
    'fps=4,scale=640:360',
    'pipe:1'
];

const ffmpeg = spawn(ffmpegPath, params, {
    stdio: ['ignore', 'pipe', 'ignore']
});

console.log(ffmpeg.spawnargs.join(' '));

ffmpeg.on('error', (error) => {
    console.log(error);
});

ffmpeg.on('exit', (code, signal) => {
    console.log('exit', code, signal);
    console.log(diffCount);
    console.timeEnd('rgbaOutDraw.js');
});

const p2p = new P2P();

let counter = 0;

p2p.on('pam', (data) => {
    //you do not have to listen to this event if you are just piping this data to pam-diff
    console.log(`received pam ${++counter}`);
});

const region1 = {name: 'region1', difference: 10, percent: 7, polygon: [{x: 0, y: 0}, {x: 0, y:360}, {x: 160, y: 360}, {x: 160, y: 0}]};

const region2 = {name: 'region2', difference: 10, percent: 7, polygon: [{x: 160, y: 0}, {x: 160, y: 360}, {x: 320, y: 360}, {x: 320, y: 0}]};

const region3 = {name: 'region3', difference: 10, percent: 7, polygon: [{x: 320, y: 0}, {x: 320, y: 360}, {x: 480, y: 360}, {x: 480, y: 0}]};

const region4 = {name: 'region4', difference: 10, percent: 7, polygon: [{x: 480, y: 0}, {x: 480, y: 360}, {x: 640, y: 360}, {x: 640, y: 0}]};

//const regionMask = {name: 'myMask', polygon: [{x:0, y:0}, {x:0, y:360}, {x:320, y:360}, {x:320, y:0}]};

const regions = [region1, region2, region3, region4];

//const regions = [regionMask];

const pamDiff = new PamDiff({/*regions : regions, */mask: false, response: response, async: async, draw: draw});

let diffCount = 0;

pamDiff.on('diff', (data) => {
    //console.log(data);

    diffCount++;
    //comment out the following line if you want to use ffmpeg to create a jpeg from the pam image that triggered an image difference event
    //if(true){return;}

    const date = new Date();
    //let name = `${date.getUTCFullYear()}-${date.getUTCMonth() + 1}-${date.getUTCDate()}_${date.getHours()}-${date.getUTCMinutes()}-${date.getUTCSeconds()}-${date.getUTCMilliseconds()}`;
    let name = counter;
    for (const region of data.trigger) {
        if (response === 'bounds') {
            name += `--${region.name}-percent${region.percent}-minX${region.minX}-maxX${region.maxX}-minY${region.minY}-maxY${region.maxY}`;
        } else {
            name += `--${region.name}-percent${region.percent}`;
        }
    }
    const jpeg = `${name}.jpeg`;
    const pathToJpeg = `${__dirname}/out/rgb24/${jpeg}`;

    //console.log(data.bc.length === 640 * 360);

    //const ff = execFile(ffmpegPath, ['-y', '-f', 'rawvideo', '-pix_fmt', 'gray', '-s', '640x360', '-i', 'pipe:0', '-frames', 1, '-c:v', 'mjpeg', '-pix_fmt', 'yuvj422p', '-q:v', '1', '-huffman', 'optimal', pathToJpeg]);

    //ff.stdin.end(data.bc);

    const ff = execFile(ffmpegPath, ['-y', '-f', 'pam_pipe', '-c:v', 'pam', '-i', 'pipe:0', '-c:v', 'mjpeg', '-pix_fmt', 'yuvj420p', '-q:v', 1, '-huffman', 1, pathToJpeg]);

    ff.stderr.on('data', data => {
        //console.log('ffmpeg stderr data', data);
    });

    if (data.headers && data.pixels) {
        ff.stdin.write(data.headers);
        ff.stdin.end(data.pixels);
    } else {
        ff.stdin.end(data.pam);
    }

    ff.on('exit', (data, other) => {
        if (data === 0) {
            console.log(`FFMPEG clean exit after creating ${jpeg}`);
        } else {
            throw new Error('FFMPEG is not working with current parameters');
        }
    });
});

console.time('rgbaOutDraw.js');
ffmpeg.stdout.pipe(p2p).pipe(pamDiff);