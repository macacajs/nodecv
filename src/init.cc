#include "core/Mat.h"
#include "highgui/highgui.h"
#include "objdetect/CascadeClassifier.h"
#include "features2d/features2d.h"
#include "imgproc/imgproc.h"

using v8::FunctionTemplate;

NAN_MODULE_INIT(InitModules) {
  highgui::Init(target);
  Mat::Init(target);
  CascadeClassifier::Init(target);
  features2d::Init(target);
  imgproc::Init(target);
}

NODE_MODULE(NativeExtension, InitModules);
