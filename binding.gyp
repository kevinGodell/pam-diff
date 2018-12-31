{
  "targets": [
    {
      "target_name": "addon",
      "sources": [
        "src/addon.cc",
        "src/diff.cc",
        "src/sync.cc",
        "src/async.cc"
      ],
      "cflags!": [ "-fno-exceptions -Weverything -Wall -Werror -Wno-unused -Wconversion" ],
      "cflags_cc!": [ "-fno-exceptions -Weverything -Werror -Wno-unused -Wall -Wextra -Q --help=warning -Wconversion" ],
      "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
      "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
      "defines": [ "NAPI_CPP_EXCEPTIONS" ],
      "conditions": [
        ["OS==\"win\"", {
          "msvs_settings": {
            "VCCLCompilerTool": {
              "ExceptionHandling": 1
            }
          }
        }],
        ["OS==\"mac\"", {
          "xcode_settings": {
            "CLANG_CXX_LIBRARY": "libc++",
            "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
            "MACOSX_DEPLOYMENT_TARGET": "10.7",
            "GCC_ENABLE_CPP_RTTI": "NO"
          }
        }]
      ]
    }
  ]
}