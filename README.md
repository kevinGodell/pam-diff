# pam-diff
###### [![dependencies Status](https://david-dm.org/kevinGodell/pam-diff/master/status.svg)](https://david-dm.org/kevinGodell/pam-diff/master) [![Build Status](https://travis-ci.org/kevinGodell/pam-diff.svg?branch=master)](https://travis-ci.org/kevinGodell/pam-diff) [![Build status](https://ci.appveyor.com/api/projects/status/hu6qw285sm6vfwtd/branch/master?svg=true)](https://ci.appveyor.com/project/kevinGodell/pam-diff/branch/master) [![GitHub issues](https://img.shields.io/github/issues/kevinGodell/pam-diff.svg)](https://github.com/kevinGodell/pam-diff/issues) [![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/kevinGodell/pam-diff/master/LICENSE) [![npm](https://img.shields.io/npm/dt/pam-diff.svg?style=flat-square)](https://www.npmjs.com/package/pam-diff)
Measure differences between pixel arrays extracted from pam images. Works well with node module [pipe2pam](https://www.npmjs.com/package/pipe2pam) to extract pam images from an ffmpeg pipe. Supported ***tupltypes*** are ***rgb***, ***rgb_alpha***, and ***grayscale***. It is currently being used for a video motion detection project.
### Installation:
``` 
npm install pam-diff@latest --save
```
#### *Important Note:* The js-only version will no longer receive any updates. All future work will be dedicated to the n-api version because it is much more efficient.
#### *New Feature:* Async made default in 0.13.6. ~~Starting with version 0.13.0, the option to use worker threads can be enabled by passing `{async: true}` to the pam-diff constructor.~~
#### *New Feature:* Starting with version 0.13.2, the option to get x y bounding box coordinates can be set by passing `{response: "bounds"}` to the pam-diff constructor.
#### *New Feature:* Starting with version 0.13.5, the option to get the pixel buffer containing the drawn x y bounding box can be set by passing `{draw: true}` to the pam-diff constructor.
#### *New Feature:* Starting with version 0.13.6, the option to filter results by connected component labelling can be set by passing `{response: "blobs"}` to the pam-diff constructor.
#### *New Feature:* Starting with version 0.13.6, async behavior will now be default. If you need the pixel difference measurements to block the event loop, use `{sync: true}`.
#### *New Feature:* Starting with version 1.0.0, pre-built binaries will be used. If binaries are not available, installation will fall back to node-gyp.
### Usage Options:
##### When comparing 2 equally sized buffers of grayscale, rgb, or rgba pixels, there are several options:
1. all (default)
    - All pixels will be targeted when checking for differences.
    - To use this option, set the configuration object without creating any regions.
```javascript
const pamDiff = new PamDiff({difference: 5, percent: 5});
```
2. regions
    - Specific regions of pixels will be targeted when checking for differences and the rest will be ignored.
    - To use this option, create a regions array and pass it to the constructor.
```javascript
const region1 = {name: 'region1', difference: 12, percent: 22, polygon: [{x: 0, y: 0}, {x: 0, y: 225}, {x: 100, y: 225}, {x: 100, y: 0}]};
const region2 = {name: 'region2', difference: 14, percent: 10, polygon: [{x: 100, y: 0}, {x: 100, y: 225}, {x: 200, y: 225}, {x: 200, y: 0}]};
const regions = [region1, region2];
const pamDiff = new PamDiff({regions : regions});
```
3. mask
    - Specific regions of pixels will be ignored when checking for differences.
    - To use this option, create a regions array and set the mask option to true.
```javascript
const region1 = {name: 'region1', polygon: [{x: 0, y: 0}, {x: 0, y: 225}, {x: 100, y: 225}, {x: 100, y: 0}]};
const region2 = {name: 'region2', polygon: [{x: 100, y: 0}, {x: 100, y: 225}, {x: 200, y: 225}, {x: 200, y: 0}]};
const regions = [region1, region2];
const pamDiff = new PamDiff({difference: 12, percent: 10, mask: true, regions : regions});
```
##### Getting results back from the pixel difference detection:
1. event
    - A *diff* event will be emitted with a data object passed as the only argument.
```javascript
pamDiff.on('diff', data => {
    console.log(data);
}); 
```
2. callback
    - A *callback* function will be called with a data object passed as the only argument.
    - The callback can be passed as the 2nd argument to the constructor or it can be added later.
```javascript
/* callback function */      
function myCallback(data) {
    console.log(data);
}

/* via the constructor */
const pamDiff = new pamDiff({difference: 10, percent: 20}, myCallback);

/* via the setter */
pamDiff.callback = myCallback;

/* via the chain-able setter */
pamDiff.setCallback(myCallback).setDifference(10).setPercent(20);

/* remove the callback */
pamDiff.callback = null;
```
##### Expected results:
1. When targeting all pixels:
```
{
    trigger: [
        {name: 'all', percent: 13}
    ],
    pam: <Buffer>,
    headers: <Buffer>,
    pixels: <Buffer>
}
```
2. When targeting regions of pixels:
```
{
    trigger: [
        {name: 'region1', percent: 13},
        {name: 'region2', percent: 22}
    ],
    pam: <Buffer>,
    headers: <Buffer>,
    pixels: <Buffer>
}
```
3. When targeting all pixels ignored by mask:
```
{
    trigger: [
        {name: 'mask', percent: 13}
    ],
    pam: <Buffer>,
    headers: <Buffer>,
    pixels: <Buffer>
}
```
4. When targeting all pixels and setting {response: "bounds"}:
```
{
    trigger: [
        {name: 'all', percent: 13, minX: 42, maxX: 399, minY: 113, maxY: 198}
    ],
    pam: <Buffer>,
    headers: <Buffer>,
    pixels: <Buffer>
}
```
5. When targeting all pixels and setting {response: "blobs"}:
```
{
    trigger: [
        {
            name: "all",
            percent: 9,
            minX: 137,
            maxX: 1782,
            minY: 392,
            maxY: 695,
            blobs: [
                {
                    label: 0,
                    percent: 3,
                    minX: 1192,
                    maxX: 1486,
                    minY: 392,
                    maxY: 695
                },
                {
                    label: 1,
                    percent: 3,
                    minX: 1488,
                    maxX: 1782,
                    minY: 392,
                    maxY: 695
                }
            ]
        }
    ],
    pam: <Buffer>,
    headers: <Buffer>,
    pixels: <Buffer>
}
```
### Other Resources:
View the [docs](https://kevingodell.github.io/pam-diff/PamDiff.html), [tests](https://github.com/kevinGodell/pam-diff/tree/master/tests), or [examples](https://github.com/kevinGodell/pam-diff/tree/master/examples) for more implementations.
### Future Plans:
- [x] Make pre-built binaries available when using node-gyp is not an option.
- [x] Include option to return coordinates for bounding box of changed pixels.
- [x] Include option to return pixel buffer containing bounding boxes.
- [x] Introduce blob filtering to group changed pixels with their neighbors.
- [x] Include option to return pixel buffer containing bounding boxes around blobs.
- [x] Make async worker threads the default. Can be overridden with {sync: true}.