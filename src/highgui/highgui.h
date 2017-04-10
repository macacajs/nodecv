#ifndef NODECV_HIGHGUI_HIGHGUI_H
#define NODECV_HIGHGUI_HIGHGUI_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <nan.h>
#include <iostream>
#include "../core/Mat.h"

using namespace v8;
using namespace node;

class highgui : public Nan::ObjectWrap {
  public:
    static void Init(Local<Object> target);
    static NAN_METHOD(imread);
    static NAN_METHOD(imwrite);
};

#endif
