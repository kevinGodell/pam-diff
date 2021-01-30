'use strict';

const dotenv = require('dotenv');

dotenv.config();

const argv = require('minimist')(process.argv.slice(2));

const async = argv.async || process.env.ASYNC || false;

const response = argv.response || process.env.RESPONSE || 'percent';

const { cpus } = require('os');

console.log(`cpu cores available: ${cpus().length}`);

const P2P = require('pipe2pam');
const PamDiff = require('../index');
const ffmpegPath = require('../lib/ffmpeg');
const ChildProcess = require('child_process');
const spawn = ChildProcess.spawn;
const execFile = ChildProcess.execFile;

const params = [
  '-loglevel',
  'quiet',

  /* use hardware acceleration */
  '-hwaccel',
  'auto', // vda, videotoolbox, none, auto

  /* use a pre-recorded mp4 video as input */
  // '-re',//comment out to have ffmpeg read video as fast as possible
  '-i',
  `${__dirname}/in/circle_star.mp4`,

  /* set output flags */
  '-an',
  '-c:v',
  'pam',
  '-pix_fmt',
  'gray',
  // 'rgba',
  // 'rgb24',
  // 'monob',
  '-f',
  'image2pipe',
  '-vf',
  // 'fps=2,scale=1280:720',
  // 'fps=2,scale=1500:800',
  'fps=2,scale=1920:1080', // 1920:1080 scaled down = 640:360, 400:225, 384:216, 368:207, 352:198, 336:189, 320:180
  // 'fps=1,scale=iw*1/6:ih*1/6',
  // '-frames',
  // '10000',
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
  console.log('exit', code, signal);
});

const p2p = new P2P();

let counter = 0;

p2p.on('pam', data => {
  // you do not have to listen to this event if you are just piping this data to pam-diff
  console.log(`received pam ${++counter}`);
});

const region1 = {
  name: 'region1',
  difference: 1,
  percent: 1,
  polygon: [
    { x: 0, y: 0 },
    { x: 0, y: 1080 },
    { x: 480, y: 1080 },
    { x: 480, y: 0 },
  ],
};

const region2 = {
  name: 'region2',
  difference: 1,
  percent: 1,
  polygon: [
    { x: 480, y: 0 },
    { x: 480, y: 1080 },
    { x: 960, y: 1080 },
    { x: 960, y: 0 },
  ],
};

const region3 = {
  name: 'region3',
  difference: 1,
  percent: 1,
  polygon: [
    { x: 960, y: 0 },
    { x: 960, y: 1080 },
    { x: 1440, y: 1080 },
    { x: 1440, y: 0 },
  ],
};

const region4 = {
  name: 'region4',
  difference: 1,
  percent: 1,
  polygon: [
    { x: 1440, y: 0 },
    { x: 1440, y: 1080 },
    { x: 1920, y: 1080 },
    { x: 1920, y: 0 },
  ],
};

const regions = [region1, region2, region3, region4];

const pamDiff = new PamDiff({ regions: regions, response: response, async: async });

pamDiff.on('diff', data => {
  console.log(data);

  // comment out the following line if you want to use ffmpeg to create a jpeg from the pam image that triggered an image difference event
  // if(true){return;}

  const date = new Date();
  // let name = `${date.getUTCFullYear()}-${date.getUTCMonth() + 1}-${date.getUTCDate()}_${date.getHours()}-${date.getUTCMinutes()}-${date.getUTCSeconds()}-${date.getUTCMilliseconds()}`;
  let name = counter;
  for (const region of data.trigger) {
    if (response === 'bounds') {
      name += `--${region.name}-percent${region.percent}-minX${region.minX}-maxX${region.maxX}-minY${region.minY}-maxY${region.maxY}`;
    } else {
      name += `--${region.name}-percent${region.percent}`;
    }
  }
  const jpeg = `${__dirname}/out/${name}.jpeg`;
  const ff = execFile(ffmpegPath, ['-f', 'pam_pipe', '-c:v', 'pam', '-i', 'pipe:0', '-c:v', 'mjpeg', '-pix_fmt', 'yuvj422p', '-q:v', '1', '-huffman', 'optimal', jpeg]);
  ff.stdin.end(data.pam);
  ff.on('exit', data => {
    if (data === 0) {
      console.log(`FFMPEG clean exit after creating ${jpeg}`);
    } else {
      throw new Error('FFMPEG is not working with current parameters');
    }
  });
});

ffmpeg.stdout.pipe(p2p).pipe(pamDiff);
