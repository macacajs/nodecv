#ifndef __MAT_H
#define __MAT_H

#include <opencv/cv.h>
#include <node_object_wrap.h>
#include <nan.h>
#include <string.h>

class NodeMat: public Nan::ObjectWrap {
  public:
    cv::Mat mat;
  protected:
    NodeMat(): Nan::ObjectWrap() {
    };
};

class Mat: public NodeMat {
  public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static void Init(v8::Local<v8::Object> target);
    static NAN_METHOD(New);
    static NAN_METHOD(Width);
    static NAN_METHOD(Height);
    static NAN_METHOD(Size);
    static NAN_METHOD(Ellipse);
    static NAN_METHOD(Rectangle);
};

#endif
