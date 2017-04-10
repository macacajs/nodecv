#ifndef NODECV_IMGRPOC_IMGRPOC_H
#define NODECV_IMGRPOC_IMGRPOC_H

#include <opencv/cv.h>
#include <nan.h>
#include <iostream>
#include "../core/Mat.h"

using namespace v8;
using namespace node;

class imgproc : public Nan::ObjectWrap {
  public:
    static void Init(Local<Object> target);
    static NAN_METHOD(matchTemplate);
};

#endif
