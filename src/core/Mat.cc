#include "Mat.h"

using namespace v8;

Nan::Persistent<FunctionTemplate> Mat::constructor;
cv::Scalar setColor(Local<Object> objColor);

void Mat::Init(Local<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(Mat::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("Mat").ToLocalChecked());

  Nan::SetPrototypeMethod(ctor, "width", Width);
  Nan::SetPrototypeMethod(ctor, "height", Height);
  Nan::SetPrototypeMethod(ctor, "size", Size);
  Nan::SetPrototypeMethod(ctor, "ellipse", Ellipse);
  Nan::SetPrototypeMethod(ctor, "rectangle", Rectangle);
  target->Set(Nan::New("Mat").ToLocalChecked(), ctor->GetFunction());
};

NAN_METHOD(Mat::Width) {
  Nan::HandleScope scope;
  Mat *self = Nan::ObjectWrap::Unwrap<Mat>(info.This());
  info.GetReturnValue().Set(Nan::New<Number>(self->mat.size().width));
}

NAN_METHOD(Mat::Height) {
  Nan::HandleScope scope;
  Mat *self = Nan::ObjectWrap::Unwrap<Mat>(info.This());
  info.GetReturnValue().Set(Nan::New<Number>(self->mat.size().height));
}

NAN_METHOD(Mat::Size) {
  Nan::HandleScope scope;
  Mat *self = Nan::ObjectWrap::Unwrap<Mat>(info.This());
  v8::Local < v8::Array > arr = Nan::New<Array>(2);
  arr->Set(0, Nan::New<Number>(self->mat.size().height));
  arr->Set(1, Nan::New<Number>(self->mat.size().width));
  info.GetReturnValue().Set(arr);
}

NAN_METHOD(Mat::New) {
  Nan::HandleScope scope;
  if (info.This()->InternalFieldCount() == 0) {
    Nan::ThrowTypeError("Cannot instantiate without new");
  }

  Mat *mat = new Mat;
  mat->Wrap(info.Holder());
  info.GetReturnValue().Set(info.Holder());
}

NAN_METHOD(Mat::Ellipse) {

  Nan::HandleScope scope;
  Mat *self = Nan::ObjectWrap::Unwrap<Mat>(info.This());
  cv::Scalar color(0, 0, 255);
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  int thickness = 1;
  double angle = 0;
  double startAngle = 0;
  double endAngle = 360;
  int lineType = 8;
  int shift = 0;

  if (info[0]->IsObject()) {
    v8::Handle < v8::Object > options = v8::Local<v8::Object>::Cast(info[0]);
    if (options->Has(Nan::New<String>("center").ToLocalChecked())) {
      Local < Object > center =
        options->Get(Nan::New<String>("center").ToLocalChecked())->ToObject();
      x = center->Get(Nan::New<String>("x").ToLocalChecked())->Uint32Value();
      y = center->Get(Nan::New<String>("y").ToLocalChecked())->Uint32Value();
    }
    if (options->Has(Nan::New<String>("axes").ToLocalChecked())) {
      Local < Object > axes = options->Get(Nan::New<String>("axes").ToLocalChecked())->ToObject();
      width = axes->Get(Nan::New<String>("width").ToLocalChecked())->Uint32Value();
      height = axes->Get(Nan::New<String>("height").ToLocalChecked())->Uint32Value();
    }
    if (options->Has(Nan::New<String>("thickness").ToLocalChecked())) {
      thickness = options->Get(Nan::New<String>("thickness").ToLocalChecked())->Uint32Value();
    }
    if (options->Has(Nan::New<String>("angle").ToLocalChecked())) {
      angle = options->Get(Nan::New<String>("angle").ToLocalChecked())->NumberValue();
    }
    if (options->Has(Nan::New<String>("startAngle").ToLocalChecked())) {
      startAngle = options->Get(Nan::New<String>("startAngle").ToLocalChecked())->NumberValue();
    }
    if (options->Has(Nan::New<String>("endAngle").ToLocalChecked())) {
      endAngle = options->Get(Nan::New<String>("endAngle").ToLocalChecked())->NumberValue();
    }
    if (options->Has(Nan::New<String>("lineType").ToLocalChecked())) {
      lineType = options->Get(Nan::New<String>("lineType").ToLocalChecked())->Uint32Value();
    }
    if (options->Has(Nan::New<String>("shift").ToLocalChecked())) {
      shift = options->Get(Nan::New<String>("shift").ToLocalChecked())->Uint32Value();
    }
    if (options->Has(Nan::New<String>("color").ToLocalChecked())) {
      Local < Object > objColor =
        options->Get(Nan::New<String>("color").ToLocalChecked())->ToObject();
      color = setColor(objColor);
    }
  } else {
    x = info[0]->Uint32Value();
    y = info[1]->Uint32Value();
    width = info[2]->Uint32Value();
    height = info[3]->Uint32Value();

    if (info[4]->IsArray()) {
      Local < Object > objColor = info[4]->ToObject();
      color = setColor(objColor);
    }

    if (info[5]->IntegerValue())
      thickness = info[5]->IntegerValue();
  }

  cv::ellipse(self->mat, cv::Point(x, y), cv::Size(width, height), angle,
      startAngle, endAngle, color, thickness, lineType, shift);
  info.GetReturnValue().Set(Nan::Null());
}

NAN_METHOD(Mat::Rectangle) {
  Nan::HandleScope scope;
  Mat *self = Nan::ObjectWrap::Unwrap<Mat>(info.This());

  if (info[0]->IsArray() && info[1]->IsArray()) {
    Local < Object > xy = info[0]->ToObject();
    Local < Object > width_height = info[1]->ToObject();

    cv::Scalar color(0, 0, 255);

    if (info[2]->IsArray()) {
      Local < Object > objColor = info[2]->ToObject();
      color = setColor(objColor);
    }

    int x = xy->Get(0)->IntegerValue();
    int y = xy->Get(1)->IntegerValue();

    int width = width_height->Get(0)->IntegerValue();
    int height = width_height->Get(1)->IntegerValue();

    int thickness = 1;

    if (info[3]->IntegerValue())
      thickness = info[3]->IntegerValue();

    cv::rectangle(self->mat, cv::Point(x, y), cv::Point(x + width, y + height),
        color, thickness);
  }

  info.GetReturnValue().Set(Nan::Null());
}

cv::Scalar setColor(Local<Object> objColor) {
  int64_t channels[4] = { 0, 0, 0, 0 };

  if (!objColor->HasRealIndexedProperty(1)) {
    channels[0] = objColor->Get(0)->IntegerValue();
  } else if (!objColor->HasRealIndexedProperty(2)) {
    channels[0] = objColor->Get(0)->IntegerValue();
    channels[1] = objColor->Get(1)->IntegerValue();
  } else if (!objColor->HasRealIndexedProperty(4)) {
    channels[0] = objColor->Get(0)->IntegerValue();
    channels[1] = objColor->Get(1)->IntegerValue();
    channels[2] = objColor->Get(2)->IntegerValue();
    channels[3] = objColor->Get(3)->IntegerValue();
  } else {
    channels[0] = objColor->Get(0)->IntegerValue();
    channels[1] = objColor->Get(1)->IntegerValue();
    channels[2] = objColor->Get(2)->IntegerValue();
  }

  return cv::Scalar(channels[0], channels[1], channels[2], channels[3]);
}

