# pam-diff
Measure differences between pixel arrays extracted from pam images. Works well with node module [pipe2pam](https://www.npmjs.com/package/pipe2pam) to extract pam images from an ffmpeg pipe. Supported tupltypes are rgb24, rgb_alpha, grayscale, and monob.


### installation:
``` 
npm install pam-diff --save
```
**To run the example below, also install pipe2pam:**
```
npm install pipe2pam --save
```
### usage:

The following example uses ffmpeg's testsrc to simulate a video input and generate 1000 downscaled grayscale pam images at a rate of 1 per second. The pam images are piped from ffmpeg's stdout into pipe2pam to parse them into into pam objects. The pam objects are then piped into pam-diff to measure pixel differences. For each compared pixel that has a **difference** that exceeds the setting, it will be added to an array of x y coordinates. If the **percent** of changed pixels exceeds the setting, a **diff** event will be emitted which contains an array of pixel coordinates that have changed.

```
const P2P = require('pipe2pam');
const PamDiff = require('pam-diff');
const spawn = require('child_process').spawn;

const params = [
    '-loglevel',
    'quiet',
    
    /* use hardware acceleration */
    //'-hwaccel',
    //'auto', //vda, videotoolbox, none, auto

    /* use an artificial video input */
    '-re',
    '-f',
    'lavfi',
    '-i',
    'testsrc=size=1920x1080:rate=15',

    /* use an rtsp ip cam video input */
    /*'-rtsp_transport',
    'tcp', //udp, http, tcp
    '-i',
    'rtsp://192.168.1.22:554/user=admin_password=pass_channel=1_stream=0.sdp',*/
    
    '-an',
    '-c:v',
    'pam',
    '-f',
    'image2pipe',
    '-pix_fmt',
    'gray',//rgb24, rgba, monob, gray
    '-vf',
    'fps=1,scale=iw*1/6:ih*1/6',
    '-frames',
    '1000',
    'pipe:1'
];

const ffmpeg = spawn('ffmpeg', params);

console.log(ffmpeg.spawnargs.join(' '));

ffmpeg.on('error', function(error) {
    console.log(error);
});

ffmpeg.on('exit', function(code, signal) {
    console.log('exit', code, signal);
});

const p2p = new P2P();

let counter = 0;

p2p.on('pam', function(data) {
    //you do not have to do anything here if you are just piping this data to pam-diff
    console.log('received pam', ++counter);
});

const pamDiff = new PamDiff({grayscale: 'average', difference: 4, percent: 5});

pamDiff.on('diff', function(data) {
    //further analyze the pixels for regions or trigger motion detection from this event
    console.log(`${data.diffPix.length} pixels different, ${data.percent}%`);
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);
```