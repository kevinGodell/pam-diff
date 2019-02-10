'use strict';

const dotenvConfig = require('dotenv').config();

//dotenvConfig();

const argv = require('minimist')(process.argv.slice(2));

const target = argv.target || process.env.TARGET || 'all';// all or mask or regions

const async = argv.async || process.env.ASYNC || false;// true or false

const response = argv.response || process.env.RESPONSE || 'percent';// percent or bounds

const draw = argv.draw || process.env.DRAW || false;// true or false

const pixFmt = argv.pixfmt || process.env.PIXFMT || 'gray';// gray or rgb24 or rgba

const {cpus} = require('os');

const basePathToJpeg = `${__dirname}/out/${response === 'bounds' && toBool(draw) ? 'draw' : 'pam'}/`;

function toBool(val) {
    return val === true || val === 'true' || val === 1 || val === '1';
}

console.log(`cpu cores available: ${cpus().length}`);

const P2P = require('pipe2pam');
const PamDiff = require('../index');
const {path: ffmpegPath} = require('ffmpeg-static');
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
    pixFmt,
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
    console.log(`exit ${code} ${signal}`);
    // may be a race condition with these values
    console.log(`pam count: ${pamCounter}`);
    console.log(`diff count: ${diffCounter}`);
    console.log(`jpeg count: ${jpegCounter}`);
});

const p2p = new P2P();

let pamCounter = 0;
let diffCounter = 0;
let jpegCounter = 0;

p2p.on('pam', (data) => {
    ++pamCounter;
});

let regions, mask;

if (target === 'regions') {
    const region1 = {name: 'region1', difference: 10, percent: 7, polygon: [{x: 0, y: 0}, {x: 0, y: 360}, {x: 160, y: 360}, {x: 160, y: 0}]};
    const region2 = {name: 'region2', difference: 10, percent: 7, polygon: [{x: 160, y: 0}, {x: 160, y: 360}, {x: 320, y: 360}, {x: 320, y: 0}]};
    const region3 = {name: 'region3', difference: 10, percent: 7, polygon: [{x: 320, y: 0}, {x: 320, y: 360}, {x: 480, y: 360}, {x: 480, y: 0}]};
    const region4 = {name: 'region4', difference: 10, percent: 7, polygon: [{x: 480, y: 0}, {x: 480, y: 360}, {x: 640, y: 360}, {x: 640, y: 0}]};
    regions = [region1, region2, region3, region4];
} else if (target === 'mask') {
    const region1 = {name: 'myMask', polygon: [{x: 0, y: 0}, {x: 0, y: 360}, {x: 320, y: 360}, {x: 320, y: 0}]};
    regions = [region1];
    mask = true;
} else {
    regions = mask = null;
}

const pamDiff = new PamDiff({regions: regions, mask: mask, response: response, async: async, draw: draw});

pamDiff.on('diff', (data) => {
    //console.log(data);

    ++diffCounter;
    //comment out the following line if you want to use ffmpeg to create a jpeg from the pam image that triggered an image difference event
    //if(true){return;}

    const date = new Date();
    let name = `${pixFmt}-${toBool(async) ? 'async' : 'sync'}-${diffCounter}`;
    for (const region of data.trigger) {
        if (response === 'bounds') {
            name += `--${region.name}-percent${region.percent}-minX${region.minX}-maxX${region.maxX}-minY${region.minY}-maxY${region.maxY}`;
        } else {
            name += `--${region.name}-percent${region.percent}`;
        }
    }
    const jpeg = `${name}.jpeg`;
    const pathToJpeg = `${basePathToJpeg}${jpeg}`;

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
            //console.log(`FFMPEG clean exit after creating ${jpeg}`);
            ++jpegCounter;
        } else {
            throw new Error('FFMPEG is not working with current parameters');
        }
    });
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);