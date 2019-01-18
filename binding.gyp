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
     #"cflags": [ "-Werror", "-Wall", "-Wextra", "-Wpedantic", "-Wunused-parameter", "-funroll-loops", "-Ofast" ],#targets all files, c and c++
     #"cflags_c": [ "-hello" ],# does this do anything?
     #"cflags_cc": [ "-Werror", "-Wall", "-Wextra", "-Wpedantic", "-Wunused-parameter", "-funroll-loops", "-Ofast" ],#target c++ only
     #"cflags": [ "-O2" ],
     "cflags_cc": [ "-O2", "-std=gnu++14" ],# using cflags_cc+ caused this flag to come before the default, we need it after to cancel previous
     "cflags!": [ "-fno-exceptions", "-Wno-unused-parameter"],
     "cflags_cc!": [ "-fno-exceptions", "-Wno-unused-parameter" ],
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
           "CLANG_CXX_LANGUAGE_STANDARD": 'c++14',
           "CLANG_CXX_LIBRARY": "libc++",
           "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
           "MACOSX_DEPLOYMENT_TARGET": "10.7",
           "GCC_ENABLE_CPP_RTTI" : "YES",

           # options to test: "fast", "3", "2", "1", "0", "", "s"
           #"GCC_OPTIMIZATION_LEVEL": "1",

           # only passed to C files
           "OTHER_CFLAGS" : [],

           # only passed to C++ files
           "OTHER_CPLUSPLUSFLAGS": [
                "-Werror",
                "-Wextra",
                "-Wpedantic",
                "-Wunused-parameter",
                #"-Weverything"
                #"-fdiagnostics-show-hotness",
                #"-fsave-optimization-record"
           ],
         }
       }]
     ]
   }
 ]
}