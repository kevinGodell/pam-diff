'use strict';

const { config: dotenvConfig } = require('dotenv');

dotenvConfig();

const argv = require('minimist')(process.argv.slice(2));

function getVal(...vals) {
  // returns first defined val or undefined
  for (let val of vals) {
    if (val === undefined) {
      continue;
    }
    return val;
  }
  return undefined;
}

// get vals in order of priority, args || env || default

const target = getVal(argv.target, process.env.TARGET, 'all'); // all || region(s)

const mask = getVal(argv.mask, process.env.MASK, false); // true || false

const sync = getVal(argv.sync, process.env.SYNC, false); // true || false

const response = getVal(argv.response, process.env.RESPONSE, 'percent'); // percent || bounds || blobs

const draw = getVal(argv.draw, process.env.DRAW, false); // true || false

const pixFmt = getVal(argv.pixfmt, process.env.PIXFMT, 'gray'); // gray || rgb24 || rgba

const outPath = getVal(argv.outPath, process.env.OUT_PATH, './'); // ./

const { cpus } = require('os');

const basePathToJpeg = `${__dirname}/out/${(response === 'bounds' || response === 'blobs') && toBool(draw) ? 'draw' : 'pam'}/`;

function toBool(val) {
  return val === true || val === 'true' || val === 1 || val === '1';
}

console.log(`cpu cores available: ${cpus().length}`);

const P2P = require('pipe2pam');
const PamDiff = require('../index');
const ffmpegPath = require('../lib/ffmpeg');
const { spawn, execFile } = require('child_process');
const { createWriteStream } = require('fs');

const params = [
  '-loglevel',
  'quiet',

  /* use hardware acceleration */
  '-hwaccel',
  'auto', // vda, videotoolbox, none, auto

  // '-stream_loop', '-1',//-1 for infinite

  /* use a pre-recorded mp4 video as input */
  '-re', // comment out to have ffmpeg read video as fast as possible

  '-rtsp_transport',
  'tcp',
  '-i',
  // `${__dirname}/in/circle_star.mp4`,
  // 'https://cdn.shinobi.video/videos/bears.mp4',
  // 'https://cdn.shinobi.video/videos/PlateDemo_Front.mp4',
  'rtsp://192.168.1.23:554/user=admin_password=pass_channel=1_stream=1.sdp',

  /* '-re',
    '-f',
    'lavfi',
    '-i',
    'testsrc=size=1920x1080:rate=2',*/

  /* set output flags */
  '-an',
  '-c:v',
  'pam',
  '-pix_fmt',
  pixFmt,
  '-f',
  'image2pipe',
  '-vf',
  // 'fps=4,scale=1920:1080',
  'fps=2,scale=640:360',
  'pipe:1',
];

const ffmpeg = spawn(`${outPath}ffmpeg`, params, {
  stdio: ['ignore', 'pipe', 'ignore'],
});

console.log(ffmpeg.spawnargs.join(' '));

ffmpeg.on('error', error => {
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

p2p.on('pam', data => {
  ++pamCounter;
});

let regions;

if (target === 'all') {
  regions = null;
} else {
  const region1 = {
    name: 'TOP_LEFT',
    difference: 10,
    percent: 7,
    polygon: [
      { x: 0, y: 0 },
      { x: 959, y: 0 },
      { x: 959, y: 539 },
      { x: 0, y: 539 },
    ],
  };
  const region2 = {
    name: 'BOTTOM_LEFT',
    difference: 10,
    percent: 7,
    polygon: [
      { x: 0, y: 540 },
      { x: 959, y: 540 },
      { x: 959, y: 1079 },
      { x: 0, y: 1079 },
    ],
  };
  const region3 = {
    name: 'TOP_RIGHT',
    difference: 10,
    percent: 7,
    polygon: [
      { x: 960, y: 0 },
      { x: 1919, y: 0 },
      { x: 1919, y: 539 },
      { x: 960, y: 539 },
    ],
  };
  regions = [region1, region2, region3];
}

const pamDiff = new PamDiff({ percent: 2, regions: regions, mask: mask, response: response, sync: sync, draw: draw });

pamDiff.on('diff', data => {
  // console.log(data);

  ++diffCounter;
  // comment out the following line if you want to use ffmpeg to create a jpeg from the pam image that triggered an image difference event
  // if(true){return;}

  const date = new Date();
  let name = `${pixFmt}-${toBool(sync) ? 'sync' : 'async'}-${diffCounter}`;
  for (const region of data.trigger) {
    if (response === 'bounds') {
      name += `--${region.name}-percent${region.percent}-minX${region.minX}-maxX${region.maxX}-minY${region.minY}-maxY${region.maxY}`;
    } else {
      name += `--${region.name}-percent${region.percent}`;
    }
  }
  const jpeg = `${name}.jpeg`;
  const pathToJpeg = `${outPath}${jpeg}`;

  // const ff = execFile(ffmpegPath, ['-y', '-f', 'rawvideo', '-pix_fmt', 'gray', '-s', '640x360', '-i', 'pipe:0', '-frames', 1, '-c:v', 'mjpeg', '-pix_fmt', 'yuvj422p', '-q:v', '1', '-huffman', 'optimal', pathToJpeg]);

  // ff.stdin.end(data.bc);

  /* const ff = execFile(ffmpegPath, ['-y', '-f', 'pam_pipe', '-c:v', 'pam', '-i', 'pipe:0', '-c:v', 'mjpeg', '-pix_fmt', 'yuvj420p', '-q:v', 1, '-huffman', 1, pathToJpeg]);

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

  console.log(`${pathToJpeg}.pam`);

  if (global.gc) {
    global.gc();
  }
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);
