#include "imgproc.h"

void imgproc::Init(Local<Object> target) {
  Nan::HandleScope scope;
  Nan::SetMethod(target, "matchTemplate", matchTemplate);
};

NAN_METHOD(imgproc::matchTemplate) {
  Nan::EscapableHandleScope scope;
  Mat *img1 = Nan::ObjectWrap::Unwrap<Mat>(info[0]->ToObject());
  Mat *img2 = Nan::ObjectWrap::Unwrap<Mat>(info[1]->ToObject());
  /*
   TM_SQDIFF        =0
   TM_SQDIFF_NORMED =1
   TM_CCORR         =2
   TM_CCORR_NORMED  =3
   TM_CCOEFF        =4
   TM_CCOEFF_NORMED =5
   */
  
  int method = info[2]->Uint32Value();
  Local<Function> cb = Local<Function>::Cast(info[3]);
  Local<Value> argv[2];
  argv[0] = Nan::Null();
  
  try {
    Local<Object> out = Nan::New(Mat::constructor)->GetFunction() -> NewInstance(v8::Isolate::GetCurrent() -> GetCurrentContext()).FromMaybe(v8::Local<v8::Object>());
    
    Mat *m_out = Nan::ObjectWrap::Unwrap<Mat>(out);
    
    int cols = img1->mat.cols - img2->mat.cols + 1;
    int rows = img1->mat.rows - img2->mat.rows + 1;
    m_out->mat.create(cols, rows, CV_64FC1);
    cv::matchTemplate(img1->mat, img2->mat, m_out->mat, method);
    cv::normalize(m_out->mat, m_out->mat, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
    double minVal;
    double maxVal;
    cv::Point minLoc;
    cv::Point maxLoc;
    cv::Point matchLoc;
    
    minMaxLoc(m_out->mat, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
    
    if(method  == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED) {
      matchLoc = minLoc;
    } else {
      matchLoc = maxLoc;
    }
    
    //detected ROI
    unsigned int roi_x = matchLoc.x;
    unsigned int roi_y = matchLoc.y;
    unsigned int roi_width = img2->mat.cols;
    unsigned int roi_height = img2->mat.rows;
    
    m_out->mat.convertTo(m_out->mat, CV_8UC1, 255, 0);
    
    v8::Local <v8::Array> arr = Nan::New<v8::Array>(method);
    arr->Set(0, out);
    arr->Set(1, Nan::New<Number>(roi_x));
    arr->Set(2, Nan::New<Number>(roi_y));
    arr->Set(3, Nan::New<Number>(roi_width));
    arr->Set(4, Nan::New<Number>(roi_height));
    
    argv[1] = arr;

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
