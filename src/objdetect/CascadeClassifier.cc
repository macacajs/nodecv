#include "CascadeClassifier.h"

Nan::Persistent<FunctionTemplate> CascadeClassifier::constructor;

void CascadeClassifier::Init(Local<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate> (CascadeClassifier::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("CascadeClassifier").ToLocalChecked());

  Nan::SetPrototypeMethod(ctor, "detectMultiScale", DetectMultiScale);

  target->Set(Nan::New("CascadeClassifier").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(CascadeClassifier::New) {
  Nan::HandleScope scope;

  if (info.This()->InternalFieldCount() == 0) {
    Nan::ThrowTypeError("Cannot instantiate without new");
  }

  CascadeClassifier *pt = new CascadeClassifier(*info[0]);
  pt->Wrap(info.This());
  info.GetReturnValue().Set( info.This() );
}

CascadeClassifier::CascadeClassifier(v8::Value* fileName) {
  std::string filename;
  filename = std::string(*Nan::Utf8String(fileName->ToString()));

  if (!cc.load(filename.c_str())) {
    Nan::ThrowTypeError("Error loading file");
  }
}

class AsyncDetectMultiScale: public Nan::AsyncWorker {
  public:
    AsyncDetectMultiScale(Nan::Callback *callback, CascadeClassifier *cc, Mat* im, double scale, int neighbors, int minw, int minh) :
      Nan::AsyncWorker(callback),
      cc(cc),
      im(im),
      scale(scale),
      neighbors(neighbors),
      minw(minw),
      minh(minh) {
      }

    ~AsyncDetectMultiScale() {
    }

    void Execute() {
      try {
        std::vector < cv::Rect > objects;

        cv::Mat gray;

        if (this->im->mat.channels() != 1) {
          cvtColor(this->im->mat, gray, CV_BGR2GRAY);
          equalizeHist(gray, gray);
        } else {
          gray = this->im->mat;
        }
        this->cc->cc.detectMultiScale(gray, objects, this->scale, this->neighbors, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(this->minw, this->minh));
        res = objects;
      } catch (cv::Exception& e) {
        SetErrorMessage(e.what());
      }
    }

    void HandleOKCallback() {
      Nan::HandleScope scope;

      Local < Value > argv[2];
      v8::Local < v8::Array > arr = Nan::New < v8::Array > (this->res.size());

      for (unsigned int i = 0; i < this->res.size(); i++) {
        v8::Local < v8::Object > x = Nan::New<v8::Object>();
        x->Set(Nan::New("x").ToLocalChecked(), Nan::New < Number > (this->res[i].x));
        x->Set(Nan::New("y").ToLocalChecked(), Nan::New < Number > (this->res[i].y));
        x->Set(Nan::New("width").ToLocalChecked(), Nan::New < Number > (this->res[i].width));
        x->Set(Nan::New("height").ToLocalChecked(), Nan::New < Number > (this->res[i].height));
        arr->Set(i, x);
      }

      argv[0] = Nan::Null();
      argv[1] = arr;

      Nan::TryCatch try_catch;
      callback->Call(2, argv);
      if (try_catch.HasCaught()) {
        Nan::FatalException(try_catch);
      }
    }

  private:
    CascadeClassifier *cc;
    Mat* im;
    double scale;
    int neighbors;
    int minw;
    int minh;
    std::vector<cv::Rect> res;
};

NAN_METHOD(CascadeClassifier::DetectMultiScale) {
  Nan::HandleScope scope;

  CascadeClassifier *self = Nan::ObjectWrap::Unwrap<CascadeClassifier> (info.This());

  if (info.Length() < 2) {
    Nan::ThrowTypeError("detectMultiScale takes at least 2 info");
  }

  Mat *im = Nan::ObjectWrap::Unwrap < Mat > (info[0]->ToObject());

  if (info.Length() <= (1) || !info[1]->IsFunction())
    return Nan::ThrowTypeError("Argument 1 must be a function");
  Local<Function> cb = Local<Function>::Cast(info[1]);

  double scale = 1.1;
  if (info.Length() > 2 && info[2]->IsNumber()) {
    scale = info[2]->NumberValue();
  }

  int neighbors = 2;
  if (info.Length() > 3 && info[3]->IsInt32()) {
    neighbors = info[3]->IntegerValue();
  }

  int minw = 30;
  int minh = 30;
  if (info.Length() > 5 && info[4]->IsInt32() && info[5]->IsInt32()) {
    minw = info[4]->IntegerValue();
    minh = info[5]->IntegerValue();
  }

  Nan::Callback *callback = new Nan::Callback(cb.As<Function>());

  Nan::AsyncQueueWorker( new AsyncDetectMultiScale(callback, self, im, scale, neighbors, minw, minh));
  return;
}
