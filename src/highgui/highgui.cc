#include "highgui.h"

void highgui::Init(Local<Object> target) {
  Nan::HandleScope scope;
  Nan::SetMethod(target, "imread", imread);
  Nan::SetMethod(target, "imwrite", imwrite);
};

NAN_METHOD(highgui::imread) {
  Nan::EscapableHandleScope scope;

  Local<Value> argv[2];
  argv[0] = Nan::Null();

  Local<Object> im_h = Nan::New(Mat::constructor)->GetFunction() -> NewInstance(v8::Isolate::GetCurrent() -> GetCurrentContext()).FromMaybe(v8::Local<v8::Object>());
  Mat *img = Nan::ObjectWrap::Unwrap<Mat>(im_h);
  argv[1] = im_h;

  Local<Function> cb = Local<Function>::Cast(info[1]);

  try {
    cv::Mat mat;

    if (info[0]->IsString()) {
      std::string filename = std::string(*Nan::Utf8String(info[0]->ToString()));
      mat = cv::imread(filename, CV_LOAD_IMAGE_UNCHANGED);
    } else if (Buffer::HasInstance(info[0])) {
      uint8_t *buf = (uint8_t *) Buffer::Data(info[0]->ToObject());
      size_t len = Buffer::Length(info[0]->ToObject());
      cv::Mat *mbuf = new cv::Mat(len, 1, CV_64FC1, buf);
      mat = cv::imdecode(*mbuf, CV_LOAD_IMAGE_UNCHANGED);

      if (mat.empty()) {
        argv[0] = Nan::Error("Error loading file");
      }
    }

    img->mat = mat;
  } catch (cv::Exception& e) {
    argv[0] = Nan::Error(e.what());
    argv[1] = Nan::Null();
  }

  Nan::TryCatch try_catch;
  cb->Call(Nan::GetCurrentContext()->Global(), 2, argv);

  if (try_catch.HasCaught()) {
    Nan::FatalException(try_catch);
  }

  return;
}

NAN_METHOD(highgui::imwrite) {
  Nan::HandleScope scope;

  Mat *img = Nan::ObjectWrap::Unwrap<Mat>(info[1]->ToObject());

  if (!info[0]->IsString()) {
    Nan::ThrowTypeError("filename required");
  }

  Nan::Utf8String filename(info[0]);
  int res = cv::imwrite(*filename, (img->mat).clone());

  info.GetReturnValue().Set(Nan::New<Number>(res));
}
