#ifndef __FEATURES2D_H
#define __FEATURES2D_H

#include <opencv/cv.h>
#include <nan.h>
#include <iostream>
#include "../core/Mat.h"

using namespace v8;
using namespace node;

class features2d : public Nan::ObjectWrap {
  public:
    static void Init(Local<Object> target);
    static NAN_METHOD(imageDissimilarity);
};

#endif
