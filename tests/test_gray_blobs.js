'use strict';

const dotenv = require('dotenv');

dotenv.config();

const argv = require('minimist')(process.argv.slice(2));

const async = argv.async || process.env.ASYNC || false;

const response = argv.response || process.env.RESPONSE || 'percent';

const draw = argv.draw || process.env.DRAW || false;

const {cpus} = require('os');

console.log(`cpu cores available: ${cpus().length}`);

//console.time('=====> testing gray pam diffs with no region set');

const assert = require('assert');

const P2P = require('pipe2pam');

const PamDiff = require('../index');

const ffmpegPath = require('ffmpeg-static').path;

//const spawn = require('child_process').spawn;

const {spawn, execFile} = require('child_process');

const {createWriteStream} = require('fs');

const pamCount = 100;

let pamCounter = 0;

let pamDiffCounter = 0;

let diffCounter = 0;

const pamDiffResults = [14, 15, 14, 14, 14, 14, 14, 14, 14];

const params = [
    /* log info to console */
    '-loglevel',
    'quiet',
    //'-stats',
    
    /* use an artificial video input */
    //'-re',
    /*'-f',
    'lavfi',
    '-i',
    'testsrc=size=1920x1080:rate=15',*/

    '-i',
    `${__dirname}/../examples/in/circle_star.mp4`,

    /* set output flags */
    '-an',
    '-c:v',
    'pam',
    '-pix_fmt',
    'gray',
    '-f',
    'image2pipe',
    '-vf',
    'fps=6,scale=640:360',
    //'fps=6,scale=400:225',
    '-frames',
    pamCount,
    'pipe:1'
];

const p2p = new P2P();

p2p.on('pam', data => {
    pamCounter++;
});

const region1 = {name: 'region1', polygon: [{x: 320, y: 0}, {x: 320, y: 380}, {x: 650, y: 390}, {x: 650, y: 0}]};

const region2 = {name: 'region2', polygon: [{x: 0, y: 0}, {x: 99, y: 0}, {x: 99, y: 1000}, {x: 0, y: 2000}]};

const regions = [region1, region2];

const pamDiff = new PamDiff({difference: 1, percent: 1, async: async, regions: regions, mask: false, response: response, draw: draw});

pamDiff.on('diff', data => {
    //console.log(data.trigger[0]);
    //console.log(data.trigger[0].blobs);
    //assert(data.trigger[0].name === 'all', 'trigger name is not correct');
    //assert(data.trigger[0].percent === pamDiffResults[pamDiffCounter++], 'trigger percent is not correct');

    console.log(data);

    ++diffCounter;

    let name = `${diffCounter}-blobs`;
    //comment out the following line if you want to use ffmpeg to create a jpeg from the pam image that triggered an image difference event
    //if(true){return;}

    //const date = new Date();
    //let name = `${pixFmt}-${toBool(async) ? 'async' : 'sync'}-${diffCounter}`;
    //for (const region of data.trigger) {
    //    if (response === 'bounds') {
    //        name += `--${region.name}-percent${region.percent}-minX${region.minX}-maxX${region.maxX}-minY${region.minY}-maxY${region.maxY}`;
    //    } else {
    //        name += `--${region.name}-percent${region.percent}`;
    //    }
    //}
    const jpeg = `${name}.jpeg`;
    //const pathToJpeg = `${basePathToJpeg}${jpeg}`;
    const pathToJpeg = jpeg;

    //console.log(pathToJpeg);

    //const ff = execFile(ffmpegPath, ['-y', '-f', 'rawvideo', '-pix_fmt', 'gray', '-s', '640x360', '-i', 'pipe:0', '-frames', 1, '-c:v', 'mjpeg', '-pix_fmt', 'yuvj422p', '-q:v', '1', '-huffman', 'optimal', pathToJpeg]);

    //ff.stdin.end(data.bc);

    /*const ff = execFile(ffmpegPath, ['-y', '-f', 'pam_pipe', '-c:v', 'pam', '-i', 'pipe:0', '-c:v', 'mjpeg', '-pix_fmt', 'yuvj420p', '-q:v', 1, '-huffman', 1, pathToJpeg]);

    ff.on('exit', (data, other) => {
        if (data === 0) {
            //console.log(`FFMPEG clean exit after creating ${jpeg}`);
            ++jpegCounter;
        } else {
            throw new Error('FFMPEG is not working with current parameters');
        }
    });

    ff.stderr.on('data', data => {
        //console.log('ffmpeg stderr data', data);
    });

    ff.stdin.write(data.headers);
    ff.stdin.end(data.pixels);*/

    const writeStream = createWriteStream(`${pathToJpeg}.pam`);
    writeStream.write(data.headers);
    writeStream.end(data.pixels);

    if (global.gc) {
        global.gc();
    }



});

const ffmpeg = spawn(ffmpegPath, params, {stdio: ['ignore', 'pipe', 'inherit']});

ffmpeg.on('error', error => {
    console.log(error);
});

ffmpeg.on('exit', (code, signal) => {
    assert(code === 0, `FFMPEG exited with code ${code} and signal ${signal}`);
    //setTimeout(()=> {
        //assert(pamDiffCounter === pamCount - 1, `did not get ${pamCount - 1} pam diffs`);
        //console.timeEnd('=====> testing gray pam diffs with no region set');
    //}, 1000);
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);