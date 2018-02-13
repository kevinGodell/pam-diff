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

    /* use an artificial video input */
    //'-re',
    '-f',
    'lavfi',
    '-i',
    'testsrc=size=1920x1080:rate=15',

    /* use an rtsp ip cam video input */
    /*'-rtsp_transport',
    'tcp',
    '-i',
    'rtsp://192.168.1.22:554/user=admin_password=pass_channel=1_stream=0.sdp',*/

    /* set output flags */
    '-an',
    '-c:v',
    'pam',
    '-pix_fmt',
    //'gray',
    //'rgba',
    'rgb24',
    '-f',
    'image2pipe',
    '-vf',
    'fps=2,scale=400:225',//1920:1080 scaled down = 640:360, 400:225, 384:216, 368:207, 352:198, 336:189, 320:180
    //'fps=1,scale=iw*1/6:ih*1/6',
    '-frames',
    '100',
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
});

const p2p = new P2P();

let counter = 0;

p2p.on('pam', (data) => {
    //you do not have to listen to this event if you are just piping this data to pam-diff
    console.log(`received pam ${++counter}`);
});

const pamDiff = new PamDiff({difference: 1, percent: 1});

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