{
 "targets": [
   {
     "target_name": "addon",
     "sources": [
       "src/addon.cc",
       "src/object.cc",
       "src/async.cc",
       "src/engine.cc",
       "src/results.cc"
     ],
     "cflags": [ "-Werror", "-Wall", "-Wextra", "-Wpedantic", "-Wunused-parameter", "-funroll-loops" ],
     "cflags_cc": [ "-Werror", "-Wall", "-Wextra", "-Wpedantic", "-Wunused-parameter", "-funroll-loops" ],
     "cflags!": [ "-fno-exceptions" ],
     "cflags_cc!": [ "-fno-exceptions" ],
     "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
     "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
     "defines": [ "NAPI_CPP_EXCEPTIONS", "NODE_ADDON_API_DISABLE_DEPRECATED" ],
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
           "__GCC_ENABLE_CPP_RTTI" : "YES"
         }
       }]
     ]
   }
 ]
}