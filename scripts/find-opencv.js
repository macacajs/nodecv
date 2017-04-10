'use strict';

const exec = require('child_process').exec;

const flag = process.argv[2] || '--exists';

const opencv_version = '2.4.13.2';

exec(`pkg-config "opencv >= opencv_version" ${flag}`, (error, stdout, stderr) => {
  if (error) {
    throw new Error('ERROR: failed to run: pkg-config');
  }
  console.log(stdout.trim());
});
