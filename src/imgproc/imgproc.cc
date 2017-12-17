#include "imgproc.h"

using namespace v8;
using namespace std;

void imgproc::Init(Local<Object> target) {
  Nan::HandleScope scope;
  Nan::SetMethod(target, "matchTemplate", matchTemplate);
  Nan::SetMethod(target, "findPairs", findPairs);
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
  
  Mat dst;
  int result_cols = img1->mat.cols - img2->mat.cols + 1;
  int result_rows = img1->mat.rows - img2->mat.rows + 1;
  
  if (result_cols > 0 && result_rows > 0) {
    try {
      Local<Object> out = Nan::New(Mat::constructor)->GetFunction() -> NewInstance(v8::Isolate::GetCurrent() -> GetCurrentContext()).FromMaybe(v8::Local<v8::Object>());
      Mat *dst = Nan::ObjectWrap::Unwrap<Mat>(out);
      dst->mat.create(result_rows, result_cols, CV_32FC1);
      cv::matchTemplate(img1->mat, img2->mat, dst->mat, method);
      cv::normalize(dst->mat, dst->mat, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
      double minVal;
      double maxVal;
      cv::Point minLoc;
      cv::Point maxLoc;
      cv::Point matchLoc;
      
      minMaxLoc(dst->mat, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
      
      if (method  == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED) {
        matchLoc = minLoc;
      } else {
        matchLoc = maxLoc;
      }
      
      dst->mat.convertTo(dst->mat, CV_8UC1, 255, 0);
      
      //detected ROI
      unsigned int roi_x = matchLoc.x;
      unsigned int roi_y = matchLoc.y;
      unsigned int roi_width = img2->mat.cols;
      unsigned int roi_height = img2->mat.rows;
      
      v8::Local <v8::Array> arr = Nan::New<v8::Array>(5);
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
  } else {
    argv[0] = Nan::Error("one of the size values (row or column) is lower than 0");
    argv[1] = Nan::Null();
  }
  
  Nan::TryCatch try_catch;
  cb->Call(Nan::GetCurrentContext()->Global(), 2, argv);
  
  if (try_catch.HasCaught()) {
    Nan::FatalException(try_catch);
  }
  
  return;
}

double imgproc::compareSURFDescriptors(
                                       const float* d1,
                                       const float* d2,
                                       double best,
                                       int length) {
  double total_cost = 0;
  assert(length % 4 == 0);
  for (int i = 0; i < length; i += 4) {
    double t0 = d1[i  ] - d2[i  ];
    double t1 = d1[i + 1] - d2[i + 1];
    double t2 = d1[i + 2] - d2[i + 2];
    double t3 = d1[i + 3] - d2[i + 3];
    total_cost += t0 * t0 + t1 * t1 + t2 * t2 + t3 * t3;
    if(total_cost > best)
      break;
  }
  return total_cost;
}


int imgproc::naiveNearestNeighbor(const float* vec,
                                  int laplacian,
                                  const CvSeq* model_keypoints,
                                  const CvSeq* model_descriptors) {
  int length = (int)(model_descriptors->elem_size / sizeof(float));
  int i, neighbor = -1;
  double d, dist1 = 1e6, dist2 = 1e6;
  CvSeqReader reader, kreader;
  cvStartReadSeq(model_keypoints, &kreader, 0);
  cvStartReadSeq(model_descriptors, &reader, 0);
  
  for(i = 0; i < model_descriptors->total; i++) {
    const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
    const float* mvec = (const float*)reader.ptr;
    CV_NEXT_SEQ_ELEM(kreader.seq->elem_size, kreader);
    CV_NEXT_SEQ_ELEM(reader.seq->elem_size, reader);
    if (laplacian != kp->laplacian)
      continue;
    d = compareSURFDescriptors(vec, mvec, dist2, length);
    if (d < dist1) {
      dist2 = dist1;
      dist1 = d;
      neighbor = i;
    } else if (d < dist2) {
      dist2 = d;
    }
  }
  if (dist1 < 0.6*dist2)
    return neighbor;
  return -1;
}

void imgproc::_findPairs(
                         const CvSeq* objectKeypoints,
                         const CvSeq* objectDescriptors,
                         const CvSeq* imageKeypoints,
                         const CvSeq* imageDescriptors,
                         vector<int>& ptpairs) {
  int i;
  CvSeqReader reader, kreader;
  cvStartReadSeq( objectKeypoints, &kreader);
  cvStartReadSeq( objectDescriptors, &reader);
  ptpairs.clear();
  
  for( i = 0; i < objectDescriptors->total; i++) {
    const CvSURFPoint* kp = (const CvSURFPoint*)kreader.ptr;
    const float* descriptor = (const float*)reader.ptr;
    CV_NEXT_SEQ_ELEM(kreader.seq->elem_size, kreader);
    CV_NEXT_SEQ_ELEM(reader.seq->elem_size, reader);
    int nearest_neighbor = naiveNearestNeighbor(descriptor, kp->laplacian, imageKeypoints, imageDescriptors );
    if (nearest_neighbor >= 0) {
      ptpairs.push_back(i);
      ptpairs.push_back(nearest_neighbor);
    }
  }
}


void imgproc::flannFindPairs( const CvSeq*, const CvSeq* objectDescriptors,
                             const CvSeq*, const CvSeq* imageDescriptors, vector<int>& ptpairs ) {
  int length = (int)(objectDescriptors->elem_size / sizeof(float));
  
  cv::Mat m_object(objectDescriptors->total, length, CV_32F);
  cv::Mat m_image(imageDescriptors->total, length, CV_32F);
  
  
  // copy descriptors
  CvSeqReader obj_reader;
  float* obj_ptr = m_object.ptr<float>(0);
  cvStartReadSeq(objectDescriptors, &obj_reader);
  for (int i = 0; i < objectDescriptors->total; i++) {
    const float* descriptor = (const float*)obj_reader.ptr;
    CV_NEXT_SEQ_ELEM( obj_reader.seq->elem_size, obj_reader );
    memcpy(obj_ptr, descriptor, length * sizeof(float));
    obj_ptr += length;
  }
  CvSeqReader img_reader;
  float* img_ptr = m_image.ptr<float>(0);
  cvStartReadSeq(imageDescriptors, &img_reader);
  for(int i = 0; i < imageDescriptors->total; i++) {
    const float* descriptor = (const float*)img_reader.ptr;
    CV_NEXT_SEQ_ELEM( img_reader.seq->elem_size, img_reader );
    memcpy(img_ptr, descriptor, length*sizeof(float));
    img_ptr += length;
  }
  
  // find nearest neighbors using FLANN
  cv::Mat m_indices(objectDescriptors->total, 2, CV_32S);
  cv::Mat m_dists(objectDescriptors->total, 2, CV_32F);
  cv::flann::Index flann_index(m_image, cv::flann::KDTreeIndexParams(4));  // using 4 randomized kdtrees
  flann_index.knnSearch(m_object, m_indices, m_dists, 2, cv::flann::SearchParams(64)); // maximum number of leafs checked
  
  int* indices_ptr = m_indices.ptr<int>(0);
  float* dists_ptr = m_dists.ptr<float>(0);
  for (int i = 0; i < m_indices.rows; ++i) {
    if (dists_ptr[2 * i] < 0.6 * dists_ptr[2 * i + 1]) {
    		ptpairs.push_back(i);
    		ptpairs.push_back(indices_ptr[2*i]);
    }
  }
}


/* a rough implementation for object location */
int imgproc::locatePlanarObject(
                                const CvSeq* objectKeypoints,
                                const CvSeq* objectDescriptors,
                                const CvSeq* imageKeypoints,
                                const CvSeq* imageDescriptors,
                                const CvPoint src_corners[4],
                                CvPoint dst_corners[4],
                                int& simlarity) {
  double h[9];
  CvMat _h = cvMat(3, 3, CV_64F, h);
  simlarity = 0;
  vector<int> ptpairs;
  vector<CvPoint2D32f> pt1, pt2;
  CvMat _pt1, _pt2;
  int i, n;
  int kp1, kp2, kpthreshold;
  flannFindPairs(objectKeypoints, objectDescriptors, imageKeypoints, imageDescriptors, ptpairs);
  //_findPairs(objectKeypoints, objectDescriptors, imageKeypoints, imageDescriptors, ptpairs );

  kp1 = objectKeypoints->total;
  kp2 = imageKeypoints->total;
  kpthreshold = kp1 < kp2 ? kp1 : kp2;

  n = (int)(ptpairs.size()/2);
  simlarity = n * 100 / kpthreshold;

  if( n < 4 )
  return 0;

  pt1.resize(n);
  pt2.resize(n);

  for (i = 0; i < n; i++) {
  pt1[i] = ((CvSURFPoint*)cvGetSeqElem(objectKeypoints,ptpairs[i*2]))->pt;
  pt2[i] = ((CvSURFPoint*)cvGetSeqElem(imageKeypoints,ptpairs[i*2+1]))->pt;
  }
  
  _pt1 = cvMat(1, n, CV_32FC2, &pt1[0]);
  _pt2 = cvMat(1, n, CV_32FC2, &pt2[0]);
  
  if (!cvFindHomography( &_pt1, &_pt2, &_h, CV_RANSAC, 5))
    return 0;
  
  for (i = 0; i < 4; i++) {
    double x = src_corners[i].x, y = src_corners[i].y;
    double Z = 1./(h[6] * x + h[7] * y + h[8]);
    double X = (h[0] * x + h[1] * y + h[2]) * Z;
    double Y = (h[3] * x + h[4] * y + h[5]) * Z;
    dst_corners[i] = cvPoint(cvRound(X), cvRound(Y));
  }
  
  return 1;
}


NAN_METHOD(imgproc::findPairs) {
  Nan::EscapableHandleScope scope;
  cv::initModule_nonfree();
  Mat *object_mat = Nan::ObjectWrap::Unwrap<Mat>(info[0]->ToObject());
  Mat *image_mat = Nan::ObjectWrap::Unwrap<Mat>(info[1]->ToObject());
  
  Local<Function> cb = Local<Function>::Cast(info[2]);
  Local<Value> argv[2];
  v8::Local<v8::Object> obj = Nan::New<v8::Object>();

try {
  IplImage object_mat_temp = object_mat->mat;
  IplImage* object_src = &object_mat_temp;
  IplImage image_mat_temp = image_mat->mat;
  IplImage* image_src = &image_mat_temp;

  argv[0] = Nan::Null();

  CvMemStorage* storage = cvCreateMemStorage(0);

  static CvScalar colors[] = {
    {{0,0,255}},
    {{0,128,255}},
    {{0,255,255}},
    {{0,255,0}},
    {{255,128,0}},
    {{255,255,0}},
    {{255,0,0}},
    {{255,0,255}},
    {{255,255,255}}
  };
  IplImage* object = cvCreateImage(cvGetSize(object_src), object_src->depth, 1);
  cvCvtColor(object_src, object, CV_RGB2GRAY);
  IplImage* image = cvCreateImage(cvGetSize(image_src), image_src->depth, 1);
  cvCvtColor(image_src, image, CV_RGB2GRAY);

  CvSeq* objectKeypoints = 0, *objectDescriptors = 0;
  CvSeq* imageKeypoints = 0, *imageDescriptors = 0;

  CvSURFParams params = cvSURFParams(500, 1);

  double tt = (double)cvGetTickCount();
  cvExtractSURF(object, 0, &objectKeypoints, &objectDescriptors, storage, params);

  cvExtractSURF(image, 0, &imageKeypoints, &imageDescriptors, storage, params);

  tt = (double)cvGetTickCount() - tt;

  CvPoint src_corners[4] = {{0,0}, {object->width,0}, {object->width, object->height}, {0, object->height}};
  CvPoint dst_corners[4];
  int simularity;
  
  if (locatePlanarObject(objectKeypoints, objectDescriptors, imageKeypoints,
                         imageDescriptors, src_corners, dst_corners, simularity)) {
    obj->Set(Nan::New("result").ToLocalChecked(), Nan::New<Boolean>(true));
    obj->Set(Nan::New("width").ToLocalChecked(), Nan::New<Number>(image->width));
    obj->Set(Nan::New("height").ToLocalChecked(), Nan::New<Number>(image->height));
    obj->Set(Nan::New("match_x1").ToLocalChecked(), Nan::New<Number>(dst_corners[0].x));
    obj->Set(Nan::New("match_y1").ToLocalChecked(), Nan::New<Number>(dst_corners[0].y));
    obj->Set(Nan::New("match_x2").ToLocalChecked(), Nan::New<Number>(dst_corners[2].x));
    obj->Set(Nan::New("match_y2").ToLocalChecked(), Nan::New<Number>(dst_corners[2].y));
    obj->Set(Nan::New("simularity").ToLocalChecked(), Nan::New<Number>(simularity));
  } else {
    obj->Set(Nan::New("result").ToLocalChecked(), Nan::New<Boolean>(false));
    obj->Set(Nan::New("simularity").ToLocalChecked(), Nan::New<Number>(simularity));
  }
  argv[1] = obj;

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

