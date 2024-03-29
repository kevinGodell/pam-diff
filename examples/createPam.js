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

  '-stream_loop',
  '1', // -1 for infinite

  /* use a pre-recorded mp4 video as input */
  // '-re',//comment out to have ffmpeg read video as fast as possible
  '-i',
  `${__dirname}/in/circle_star.mp4`,

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
  'fps=4,scale=640:360',
  'pipe:1',
];

const ffmpeg = spawn(ffmpegPath, params, {
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
  // 640 360
  const region1 = {
    name: 'TOP_LEFT',
    difference: 10,
    percent: 7,
    polygon: [
      { x: 0, y: 0 },
      { x: 319, y: 0 },
      { x: 319, y: 179 },
      { x: 0, y: 179 },
    ],
  };
  const region2 = {
    name: 'BOTTOM_LEFT',
    difference: 10,
    percent: 7,
    polygon: [
      { x: 0, y: 180 },
      { x: 319, y: 180 },
      { x: 319, y: 359 },
      { x: 0, y: 359 },
    ],
  };
  const region3 = {
    name: 'TOP_RIGHT',
    difference: 10,
    percent: 7,
    polygon: [
      { x: 320, y: 0 },
      { x: 639, y: 0 },
      { x: 639, y: 179 },
      { x: 320, y: 179 },
    ],
  };
  regions = [region1, region2, region3];
}

const pamDiff = new PamDiff({ regions: regions, mask: mask, response: response, sync: sync, draw: draw });

console.log({ mask, response, sync, draw, regions });

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
  const pathToJpeg = `${basePathToJpeg}${jpeg}`;

  // const ff = execFile(ffmpegPath, ['-y', '-f', 'rawvideo', '-pix_fmt', 'gray', '-s', '640x360', '-i', 'pipe:0', '-frames', 1, '-c:v', 'mjpeg', '-pix_fmt', 'yuvj422p', '-q:v', '1', '-huffman', 'optimal', pathToJpeg]);

  const ff = execFile(ffmpegPath, ['-y', '-f', 'pam_pipe', '-c:v', 'pam', '-i', 'pipe:0', '-c:v', 'mjpeg', '-pix_fmt', 'yuvj420p', '-q:v', 1, '-huffman', 1, pathToJpeg]);

  ff.on('exit', (data, other) => {
    if (data === 0) {
      // console.log(`FFMPEG clean exit after creating ${jpeg}`);
      ++jpegCounter;
    } else {
      throw new Error('FFMPEG is not working with current parameters');
    }
  });

  ff.stderr.on('data', data => {
    // console.log('ffmpeg stderr data', data);
  });

  ff.stdin.write(data.headers);
  ff.stdin.end(data.pixels);

  const writeStream = createWriteStream(`${pathToJpeg}.pam`);
  writeStream.write(data.headers);
  writeStream.end(data.pixels);

  if (global.gc) {
    global.gc();
  }
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);
