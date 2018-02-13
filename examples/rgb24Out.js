'use strict';

process.env.NODE_ENV = 'development';

const P2P = require('pipe2pam');
const PamDiff = require('../index');
const ffmpegPath = require('ffmpeg-static').path;
const ChildProcess = require('child_process');
const spawn = ChildProcess.spawn;
const execFile = ChildProcess.execFile;

const params = [
    '-loglevel',
    'quiet',

    /* use hardware acceleration */
    '-hwaccel',
    'auto',//vda, videotoolbox, none, auto

    /* use a pre-recorded mp4 video as input */
    //'-re',//comment out to have ffmpeg read video as fast as possible
    '-i',
    `${__dirname}/in/sample.mp4`,

    /* set output flags */
    '-an',
    '-c:v',
    'pam',
    '-pix_fmt',
    'rgb24',
    '-f',
    'image2pipe',
    '-vf',
    'fps=2,scale=640:360',
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
    console.timeEnd('rgb24Out.js');
});

const p2p = new P2P();

let counter = 0;

p2p.on('pam', (data) => {
    //you do not have to listen to this event if you are just piping this data to pam-diff
    console.log(`received pam ${++counter}`);
});

const region1 = {name: 'region1', difference: 9, percent: 9, polygon: [{x: 0, y: 0}, {x: 0, y:360}, {x: 160, y: 360}, {x: 160, y: 0}]};

const region2 = {name: 'region2', difference: 9, percent: 9, polygon: [{x: 160, y: 0}, {x: 160, y: 360}, {x: 320, y: 360}, {x: 320, y: 0}]};

const region3 = {name: 'region3', difference: 9, percent: 9, polygon: [{x: 320, y: 0}, {x: 320, y: 360}, {x: 480, y: 360}, {x: 480, y: 0}]};

const region4 = {name: 'region4', difference: 9, percent: 9, polygon: [{x: 480, y: 0}, {x: 480, y: 360}, {x: 640, y: 360}, {x: 640, y: 0}]};

const regions = [region1, region2, region3, region4];

const pamDiff = new PamDiff({regions : regions});

let diffCount = 0;

pamDiff.on('diff', (data) => {
    //console.log(data);
    diffCount++;
    //comment out the following line if you want to use ffmpeg to create a jpeg from the pam image that triggered an image difference event
    if(true){return;}

    const date = new Date();
    let name = `${date.getUTCFullYear()}-${date.getUTCMonth() + 1}-${date.getUTCDate()}_${date.getHours()}-${date.getUTCMinutes()}-${date.getUTCSeconds()}-${date.getUTCMilliseconds()}`;
    for (const region of data.trigger) {
        name += `(${region.name}=${region.percent})`;
    }
    const jpeg = `${name}.jpeg`;
    const pathToJpeg = `${__dirname}/out/rgb24/${jpeg}`;
    const ff = execFile('ffmpeg', ['-f', 'pam_pipe', '-c:v', 'pam', '-i', 'pipe:0', '-c:v', 'mjpeg', '-pix_fmt', 'yuvj422p', '-q:v', '1', '-huffman', 'optimal', pathToJpeg]);
    ff.stdin.end(data.pam);
    ff.on('exit', (data) => {
        if (data === 0) {
            console.log(`FFMPEG clean exit after creating ${jpeg}`);
        } else {
            throw new Error('FFMPEG is not working with current parameters');
        }
    });
});

console.time('rgb24Out.js');
ffmpeg.stdout.pipe(p2p).pipe(pamDiff);