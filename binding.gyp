{
 "targets": [
   {
     "target_name": "addon",
     "sources": [
       "src/addon.cc",
       "src/gray_object.cc",
       "src/rgb_object.cc",
       "src/async.cc",
       "src/engine.cc",
       "src/results.cc",
       "src/ccl.cc"
     ],
     "cflags": [ "-O2", "-Wendif-labels", "-Werror", "-Wpedantic", "-Wunused-parameter" ],# removed for now "-finline-functions", "-funswitch-loops", "-fpredictive-commoning", "-fgcse-after-reload", "-ftree-vectorize", "-fvect-cost-model", "-ftree-partial-pre", "-fipa-cp-clone"
     "cflags!": [ "-Wno-unused-parameter", "-O3" ],
     "cflags_cc": [ "-std=gnu++11" ],
     "cflags_cc!": [ "-fno-exceptions", "-fno-rtti", "-std=gnu++1y", "-std=gnu++0x" ],
     "include_dirs": [ "<!@(node -p \"require('node-addon-api').include\")" ],
     "dependencies": [ "<!(node -p \"require('node-addon-api').gyp\")" ],
     "defines": [ "NAPI_CPP_EXCEPTIONS", "NODE_ADDON_API_DISABLE_DEPRECATED", "NAPI_DEBUG" ],
     "conditions": [
       ["OS==\"win\"", {
         "msvs_settings": {
           "VCCLCompilerTool": {
             "ExceptionHandling": 1
           }
         }
       }],
       ["OS==\"mac\"", {
         "cflags+": [ "-fvisibility=hidden" ],
         "xcode_settings": {
           "CLANG_CXX_LANGUAGE_STANDARD": "c++11",
           "CLANG_CXX_LIBRARY": "libc++",
           "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
           "MACOSX_DEPLOYMENT_TARGET": "10.7",
           "GCC_ENABLE_CPP_RTTI" : "YES",
           # "fastest,smallest","fastest","faster","fast", "3", "2", "1", "0", "", "s"
           "GCC_OPTIMIZATION_LEVEL": "2",
           # only passed to C files
           "OTHER_CFLAGS": [ "-fvisibility=hidden" ],
           # remove defaults passed to C files
           "OTHER_CFLAGS!": [ "-fno-strict-aliasing" ],
           # only passed to C++ files
           "OTHER_CPLUSPLUSFLAGS": [],
           # remove defaults passed to C++ files
           "OTHER_CPLUSPLUSFLAGS!": [],
           # passed to both C/C++ files
           "WARNING_CFLAGS": [ "-Werror", "-Wextra", "-Wpedantic", "-Wunused-parameter" ],
           # remove defaults passed to both C/C++ files
           "WARNING_CFLAGS!": [ "-Wno-unused-parameter", "-W" ]
         }
       }]
     ]
   }
 ]
}
# several options for flags, cflags_cc(appends), cflags_cc+(prepends), cflags_cc!(negates)
# if -O3 and -O2 are both passed, whichever is passed last wins the race

# see https://github.com/nodejs/node/blob/master/common.gypi