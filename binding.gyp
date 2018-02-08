{
  "targets": [
    {
      "target_name": "pixel_change",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": [ "src/PixelChange.cc" ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
      '__unused__conditions': [
        ['OS=="mac"', {
          'xcode_settings': { 'GCC_ENABLE_CPP_RTTI': 'YES' }
        }]
      ]
    }
  ]
}