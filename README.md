# pam-diff

###### [![Buy me a coffee](https://img.shields.io/badge/-buy%20me%20a%20coffee-red?logo=buy%20me%20a%20coffee)](https://buymeacoffee.com/kevinGodell) [![Donate via PayPal](https://img.shields.io/badge/Donate-Paypal-blue)](https://www.paypal.com/donate/?business=HTMERJAFHJJEU&no_recurring=0&item_name=support+open+source+software+development&currency_code=USD) [![Build Status](https://github.com/kevinGodell/pam-diff/workflows/build/badge.svg)](https://github.com/kevinGodell/pam-diff/actions?query=workflow%3Abuild) [![Build Status](https://ci.appveyor.com/api/projects/status/hu6qw285sm6vfwtd/branch/master?svg=true)](https://ci.appveyor.com/project/kevinGodell/pam-diff/branch/master) [![GitHub Issues](https://img.shields.io/github/issues/kevinGodell/pam-diff.svg)](https://github.com/kevinGodell/pam-diff/issues) [![GitHub License](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/kevinGodell/pam-diff/master/LICENSE) [![npm](https://img.shields.io/npm/dt/pam-diff.svg?style=flat-square)](https://www.npmjs.com/package/pam-diff)

Measure differences between pixel arrays extracted from pam images. Works well with node module [pipe2pam](https://www.npmjs.com/package/pipe2pam) to extract pam images from an ffmpeg pipe. Supported **_tupltypes_** are **_rgb_**, **_rgb_alpha_**, and **_grayscale_**. It is currently being used for a video motion detection project.

## Installation:

```
npm install pam-diff --save
```

## Changelog:

###### _version 1.2.1:_
* Set defaults if configuration object is not passed in constructor.

###### _version 1.2.0:_
* Dropping support for node.js < 14.
* Data event emitted for all results returned from [pixel-change](https://www.npmjs.com/package/pixel-change).
* Initialized event emitted after first chunk parsed.
* Debug object attached to output if debug property is set to true.
* Updated [docs](https://kevingodell.github.io/pam-diff/PamDiff.html) to show deprecations.

###### _version 1.1.0:_
* Percent is now a float to allow for more precise results.
* Sync option is removed.

## Usage Options:

###### When comparing 2 equally sized buffers of grayscale, rgb, or rgba pixels, there are several options:

- ### all (default)

    - All pixels will be targeted when checking for differences.
    - To use this option, set the configuration object without creating any regions.

    ```javascript
    const pamDiff = new PamDiff({ difference: 5, percent: 5 });
    ```

- ### regions

    - Specific regions of pixels will be targeted when checking for differences and the rest will be ignored.
    - To use this option, create a regions array and pass it to the constructor.

    ```javascript
    const region1 = {
      name: 'region1',
      difference: 12,
      percent: 22,
      polygon: [
        { x: 0, y: 0 },
        { x: 0, y: 224 },
        { x: 99, y: 224 },
        { x: 99, y: 0 },
      ],
    };
    const region2 = {
      name: 'region2',
      difference: 14,
      percent: 10,
      polygon: [
        { x: 100, y: 0 },
        { x: 100, y: 224 },
        { x: 199, y: 224 },
        { x: 199, y: 0 },
      ],
    };
    const regions = [region1, region2];
    const pamDiff = new PamDiff({ regions: regions });
    ```

- ### mask

    - Specific regions of pixels will be ignored when checking for differences.
    - To use this option, create a regions array and set the mask option to true.
    - `difference` and `percent` of the individual region is ignored. Set global values.

    ```javascript
    const region1 = {
      name: 'region1',
      polygon: [
        { x: 0, y: 0 },
        { x: 0, y: 224 },
        { x: 99, y: 224 },
        { x: 99, y: 0 },
      ],
    };
    const region2 = {
      name: 'region2',
      polygon: [
        { x: 100, y: 0 },
        { x: 100, y: 224 },
        { x: 199, y: 224 },
        { x: 199, y: 0 },
      ],
    };
    const regions = [region1, region2];
    const pamDiff = new PamDiff({ difference: 12, percent: 10, mask: true, regions: regions });
    ```

###### Getting results back from the pixel difference detection:

1. event
    - A _diff_ event will be emitted when there is a pixel difference detection.

   ```javascript
    pamDiff.on('diff', data => {
      console.log(data);
    });
    ```

   - A _data_ event will be emitted regardless of pixel difference detection.

   ```javascript
    pamDiff.on('data', data => {
      console.log(data);
    });
    ```

2. callback (deprecated)
   - A _callback_ function will be called with a data object passed as the only argument.
   - The callback can be passed as the 2nd argument to the constructor, or it can be added later.
   - Deprecated. Scheduled to be removed.

    ```javascript
    /* callback function */
    function myCallback(data) {
      console.log(data);
    }

    /* via the constructor */
    const pamDiff = new pamDiff({ difference: 10, percent: 20 }, myCallback);

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
            { name: 'all', percent: 13 }
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
            { name: 'region1', percent: 13 },
            { name: 'region2', percent: 22 }
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
            { name: 'mask', percent: 13 }
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
            { name: 'all', percent: 13, minX: 42, maxX: 399, minY: 113, maxY: 198 }
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
