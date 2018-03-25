# nodecv

[![NPM version][npm-image]][npm-url]
[![build status][travis-image]][travis-url]
[![Test coverage][coveralls-image]][coveralls-url]
[![node version][node-image]][node-url]
[![npm download][download-image]][download-url]

[npm-image]: https://img.shields.io/npm/v/nodecv.svg?style=flat-square
[npm-url]: https://npmjs.org/package/nodecv
[travis-image]: https://img.shields.io/travis/macacajs/nodecv.svg?style=flat-square
[travis-url]: https://travis-ci.org/macacajs/nodecv
[coveralls-image]: https://img.shields.io/coveralls/macacajs/nodecv.svg?style=flat-square
[coveralls-url]: https://coveralls.io/r/macacajs/nodecv?branch=master
[node-image]: https://img.shields.io/badge/node.js-%3E=_6-green.svg?style=flat-square
[node-url]: http://nodejs.org/download/
[download-image]: https://img.shields.io/npm/dm/nodecv.svg?style=flat-square
[download-url]: https://npmjs.org/package/nodecv

> Node.js binding to [OpenCV](//github.com/opencv/opencv)

## Installment

```bash
$ npm i nodecv --save-dev
```

## Usage

Image IO:

```javascript
nodecv.imread(imagePath, callback);
nodecv.imread(buffer, callback);
nodecv.imwrite(outputPath, mat);
```

Image Dissimilarity:

```javascript
nodecv.imageDissimilarity(mat, mat, callback);
```

Match:

```javascript
nodecv.matchTemplate(mat, mat, method, callback);
nodecv.findPairs(mat, mat, callback);
```

Mat properties:

```javascript
im.width();
im.height();
im.size();
im.ellipse(x, y, width, height);
im.rectangle(x, y, width, height, color, thickness);
```

Cascade detect:

``` javascript
const haarcascade = 'path/to/haarcascade.xml';
const cascade = new nodecv.CascadeClassifier(haarcascade);
cascade.detectMultiScale(mat, callback);
```

NodeCV follow [Google's C++ style conventions](//google.github.io/styleguide/cppguide.html), and [opencv@2.4.13.2](http://docs.opencv.org/2.4.13.2/).

## Use As Service

[Deploy with Docker](//github.com/macacajs/nodecv-server)

![](http://wx3.sinaimg.cn/large/6d308bd9gy1fek6oa9i3vj21kw0zrn4y.jpg)

## Test

```bash
$ make test
```

<!-- GITCONTRIBUTOR_START -->

## Contributors

|[<img src="https://avatars1.githubusercontent.com/u/1011681?v=4" width="100px;"/><br/><sub><b>xudafeng</b></sub>](https://github.com/xudafeng)<br/>|[<img src="https://avatars3.githubusercontent.com/u/870082?v=4" width="100px;"/><br/><sub><b>Chenxin</b></sub>](https://github.com/Chenxin)<br/>
| :---: | :---: |


This project follows the git-contributor [spec](https://github.com/xudafeng/git-contributor.git), auto upated at `Sun Mar 25 2018 16:07:06 GMT+0800`.

<!-- GITCONTRIBUTOR_END -->

## License

The MIT License (MIT)
