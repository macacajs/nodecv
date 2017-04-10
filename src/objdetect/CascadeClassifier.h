#ifndef NODECV_OBJDETECT_CASCADECLASSIFIER_H
#define NODECV_OBJDETECT_CASCADECLASSIFIER_H

#include <opencv/cv.h>
#include <nan.h>
#include "../core/Mat.h"

using namespace v8;

class CascadeClassifier: public Nan::ObjectWrap {
  public:
    cv::CascadeClassifier cc;

    static Nan::Persistent<FunctionTemplate> constructor;
    static void Init(Local<Object> target);
    static NAN_METHOD(New);

    CascadeClassifier(v8::Value* fileName);

    static NAN_METHOD(DetectMultiScale);

    static void EIO_DetectMultiScale(uv_work_t *req);
    static int EIO_AfterDetectMultiScale(uv_work_t *req);
};

#endif
