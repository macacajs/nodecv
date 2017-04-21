{
  "targets": [{
    "target_name": "nodecv",
      "sources": [
        "src/init.cc",
        "src/core/Mat.cc",
        "src/highgui/highgui.cc",
        "src/objdetect/CascadeClassifier.cc",
        "src/features2d/features2d.cc",
        "src/imgproc/imgproc.cc"
      ],
      "libraries": [
        "<!@(node scripts/find-opencv.js --libs)"
      ],
      "include_dirs": [
        "<!@(node scripts/find-opencv.js --cflags)",
        "<!(node -e \"require('nan')\")"
      ],
      "cflags!" : [
        "-fno-exceptions"
      ],
      "cflags_cc!": [
        "-fno-rtti",
        "-fno-exceptions"
      ],
      "conditions": [
        [
          "OS==\"linux\" or OS==\"freebsd\" or OS==\"openbsd\" or OS==\"solaris\" or OS==\"aix\"",
          {
            "cflags": [
              "<!@(node scripts/find-opencv.js --cflags)",
              "-Wall"
            ]
          }
        ],
        [
          "OS==\"win\"",
          {
            "cflags": [
              "-Wall"
            ],
            "defines": [
              "WIN"
            ],
            "msvs_settings": {
              "VCCLCompilerTool": {
                "ExceptionHandling": "2",
                "DisableSpecificWarnings": [
                  "4244"
                ],
              },
            }
          }
        ],
        [
          "OS==\"mac\"",
          {
            "xcode_settings": {
              "CLANG_CXX_LIBRARY": "libc++",
              "OTHER_CFLAGS": [
                "-mmacosx-version-min=10.7",
                "<!@(node scripts/find-opencv.js --cflags)",
              ],
              "GCC_ENABLE_CPP_RTTI": "YES",
              "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
            }
          }
        ]
      ],
      "configurations": {
        "Debug": {
          "conditions": [
            [
              'OS=="linux"',
              {
                "cflags": [
                  "-coverage"
                ],
                "ldflags": [
                  "-coverage"
                ]
              }
            ],
            [
              'OS=="mac"',
              {
                "xcode_settings": {
                  "OTHER_CFLAGS": [
                    "-fprofile-arcs -ftest-coverage",
                  ],
                  "OTHER_LDFLAGS": [
                    "--coverage"
                  ]
                }
              }
            ]
          ]
        },
      }
    }, {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [
        "nodecv"
      ],
      "copies": [
        {
          "files": [
            "<(PRODUCT_DIR)/nodecv.node"
          ],
          "destination": "<!(node scripts/find-build-dir.js)"
        }
      ]
    }
  ]
}
