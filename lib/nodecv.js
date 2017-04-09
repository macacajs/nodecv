'use strict';

const pkg = require('../package.json');

const cv = require('bindings')(pkg.name);

module.exports = cv;
