'use strict';

const dotenv = require('dotenv');

dotenv.config();

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

const nodeEnv = getVal(argv.nodeEnv, process.env.NODE_ENV, 'production'); // production || development

const sync = getVal(argv.sync, process.env.SYNC, false); // true || false

const response = getVal(argv.response, process.env.RESPONSE, 'percent'); // percent || bounds || blobs

const draw = getVal(argv.draw, process.env.DRAW, false); // true || false

const pool = getVal(argv.pool, process.env.POOL, 0); // 0 || 2

const { cpus } = require('node:os');

const assert = require('node:assert');

const { spawn } = require('node:child_process');

const P2P = require('pipe2pam');

const PamDiff = require('../index');

const ffmpegPath = require('../lib/ffmpeg');

console.log(`cpu cores available: ${cpus().length}`);

console.time('=====> testing rgba pam diffs with 2 region masks set');

const pamCount = 10;

let pamCounter = 0;

let pamDiffCounter = 0;

const pamDiffResults = [14, 15, 14, 14, 14, 13, 15, 14, 13];

const params = [
  /* log info to console */
  '-loglevel',
  'quiet',
  // '-stats',

  /* use an artificial video input */
  // '-re',
  '-f',
  'lavfi',
  '-i',
  'testsrc=size=1920x1080:rate=15',

  /* set output flags */
  '-an',
  '-c:v',
  'pam',
  '-pix_fmt',
  'rgba',
  '-f',
  'image2pipe',
  '-vf',
  'fps=1,scale=400:225',
  '-frames',
  pamCount,
  'pipe:1',
];

const p2p = new P2P({ pool });

p2p.on('data', data => {
  pamCounter++;
});

const region1 = {
  polygon: [
    { x: 0, y: 0 },
    { x: 0, y: 224 },
    { x: 99, y: 224 },
    { x: 99, y: 0 },
  ],
};

const region2 = {
  polygon: [
    { x: 100, y: 0 },
    { x: 100, y: 224 },
    { x: 199, y: 224 },
    { x: 199, y: 0 },
  ],
};

const regions = [region1, region2];

const pamDiff = new PamDiff({
  difference: 1,
  percent: 1,
  mask: true,
  regions: regions,
  sync: sync,
  response: response,
  draw: draw,
  debug: nodeEnv === 'development',
});

pamDiff.on('diff', data => {
  if (data.debug) {
    const { name, count, duration } = data.debug;
    console.log(`${name}-${count}: ${duration}ms`);
  }
  // console.log(~~(data.trigger[0].percent));
  assert(data.trigger[0].name === 'mask', 'trigger name is not correct');
  assert(~~data.trigger[0].percent === pamDiffResults[pamDiffCounter++], 'trigger percent is not correct');
});

const ffmpeg = spawn(ffmpegPath, params, { stdio: ['ignore', 'pipe', 'inherit'] });

ffmpeg.on('error', error => {
  console.log(error);
});

ffmpeg.on('exit', (code, signal) => {
  assert(code === 0, `FFMPEG exited with code ${code} and signal ${signal}`);
  setTimeout(() => {
    assert(pamDiffCounter === pamCount - 1, `did not get ${pamCount - 1} pam diffs`);
    console.timeEnd('=====> testing rgba pam diffs with 2 region masks set');
  }, 1000);
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);
