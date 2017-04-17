#ifndef NODECV_IMGRPOC_IMGRPOC_H
#define NODECV_IMGRPOC_IMGRPOC_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <nan.h>
#include <iostream>
#include "../core/Mat.h"

class imgproc : public Nan::ObjectWrap {
  public:
  static void Init(v8::Local<v8::Object> target);
    static NAN_METHOD(matchTemplate);
    static NAN_METHOD(findPairs);
  private:

  static double compareSURFDescriptors( const float* d1, const float* d2, double best, int length );
  
  static int naiveNearestNeighbor( const float* vec, int laplacian,
                       const CvSeq* model_keypoints,
                       const CvSeq* model_descriptors );
  static void _findPairs( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,
            const CvSeq* imageKeypoints, const CvSeq* imageDescriptors, std::vector<int>& ptpairs );
  
  static void flannFindPairs( const CvSeq*, const CvSeq* objectDescriptors,
                 const CvSeq*, const CvSeq* imageDescriptors, std::vector<int>& ptpairs );
  
  /* a rough implementation for object location */
  static int locatePlanarObject( const CvSeq* objectKeypoints, const CvSeq* objectDescriptors,
                     const CvSeq* imageKeypoints, const CvSeq* imageDescriptors,
                     const CvPoint src_corners[4], CvPoint dst_corners[4] );
};
#endif
