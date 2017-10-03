// jshint esversion: 6, globalstrict: true, strict: true
'use strict';
const P2P = require('pipe2pam');
const PamDiff = require('../index');
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