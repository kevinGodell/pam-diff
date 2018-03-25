# pam-diff
###### [![dependencies Status](https://david-dm.org/kevinGodell/pam-diff/master/status.svg)](https://david-dm.org/kevinGodell/pam-diff/master) [![Build Status](https://travis-ci.org/kevinGodell/pam-diff.svg?branch=master)](https://travis-ci.org/kevinGodell/pam-diff) [![Build status](https://ci.appveyor.com/api/projects/status/hu6qw285sm6vfwtd/branch/master?svg=true)](https://ci.appveyor.com/project/kevinGodell/pam-diff/branch/master) [![GitHub issues](https://img.shields.io/github/issues/kevinGodell/pam-diff.svg)](https://github.com/kevinGodell/pam-diff/issues) [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/kevinGodell/pam-diff/master/LICENSE) [![Greenkeeper badge](https://badges.greenkeeper.io/kevinGodell/pam-diff.svg)](https://greenkeeper.io/)
Measure differences between pixel arrays extracted from pam images. Works well with node module [pipe2pam](https://www.npmjs.com/package/pipe2pam) to extract pam images from an ffmpeg pipe. Supported ***tupltypes*** are ***rgb***, ***rgb_alpha***, and ***grayscale***. It is currently being used for a video motion detection project.
### installation:
``` 
npm install pam-diff@latest --save
```
### installation of experimental n-api version:
```
npm install pam-diff@n-api --save
```
**To run the example below, also install pipe2pam:**
```
npm install pipe2pam --save
```
### usage:
The following [example](https://github.com/kevinGodell/pam-diff/tree/master/examples/example.js) uses ffmpeg to connect to a rtsp ip camera video feed and generates 1000 downscaled rgb24 pam images at a rate of 1 per second. The pam images are piped from ffmpeg's stdout into pipe2pam to parse them into into pam objects. The pam objects are then piped into pam-diff to measure pixel differences. For each compared pixel that has a **difference** that exceeds the setting, it will be calculated to determine the percent of difference. If the **percent** of changed pixels exceeds the setting, a **diff** event will be emitted which contains a data object containing details. This example also shows how to take the pam image that triggered the diff event and convert it to a jpeg using ffmpeg.
```
const P2P = require('pipe2pam');
const PamDiff = require('pam-diff');
const ChildProcess = require('child_process');
const spawn = ChildProcess.spawn;
const execFile = ChildProcess.execFile;

const params = [
    '-loglevel',
    'quiet',

    /* use hardware acceleration */
    '-hwaccel',
    'auto', //vda, videotoolbox, none, auto

    /* use an artificial video input */
    /*'-re',
     '-f',
     'lavfi',
     '-i',
     'testsrc=size=1920x1080:rate=15',*/

    /* use an rtsp ip cam video input */
    '-rtsp_transport',
    'tcp',
    '-i',
    'rtsp://192.168.1.4:554/user=admin_password=pass_channel=1_stream=0.sdp',

    /* set output flags */
    '-an',
    '-c:v',
    'pam',
    '-pix_fmt',
    'rgb24',//rgba, rgb24, gray
    '-f',
    'image2pipe',
    '-vf',
    'fps=1,scale=320:180',//1920:1080 scaled down: 400:225, 384:216, 368:207, 352:198, 336:189, 320:180
    //'fps=1,scale=iw*1/6:ih*1/6',
    '-frames',
    '1000',
    'pipe:1'
];

const ffmpeg = spawn('ffmpeg', params);

console.log(ffmpeg.spawnargs.join(' '));

ffmpeg.on('error', (error) => {
    console.log(error);
});

ffmpeg.on('exit', (code, signal) => {
    console.log('exit', code, signal);
});

const p2p = new P2P();

let counter = 0;

p2p.on('pam', (data) => {
    //you do not have to listen to this event if you are just piping this data to pam-diff
    console.log(`received pam ${++counter}`);
});

const pamDiff = new PamDiff({difference: 5, percent: 5});

pamDiff.on('diff', (data) => {
    console.log(data);
    
    //comment out the following line if you want to use ffmpeg to create a jpeg from the pam image that triggered an image difference event
    if(true){return;}
    
    const date = new Date();
    let name = `${date.getUTCFullYear()}-${date.getUTCMonth() + 1}-${date.getUTCDate()}_${date.getHours()}-${date.getUTCMinutes()}-${date.getUTCSeconds()}-${date.getUTCMilliseconds()}`;
    for (const region of data.trigger) {
        name += `(${region.name}=${region.percent})`;
    }
    const jpeg = `${name}.jpeg`;
    const ff = execFile('ffmpeg', ['-f', 'pam_pipe', '-c:v', 'pam', '-i', 'pipe:0', '-c:v', 'mjpeg', '-pix_fmt', 'yuvj422p', '-q:v', '1', '-huffman', 'optimal', jpeg]);
    ff.stdin.end(data.pam);
    ff.on('exit', (data) => {
        if (data === 0) {
            console.log(`FFMPEG clean exit after creating ${jpeg}`);
        } else {
            throw new Error('FFMPEG is not working with current parameters');
        }
    });
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);
```

See [tests](https://github.com/kevinGodell/pam-diff/tree/master/tests) or [examples](https://github.com/kevinGodell/pam-diff/tree/master/examples) for more implementations.
