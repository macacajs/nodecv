#include "features2d.h"

void features2d::Init(Local<Object> target) {
  Nan::HandleScope scope;
  Nan::SetMethod(target, "imageDissimilarity", imageDissimilarity);
};

class AsyncDetectSimilarity: public Nan::AsyncWorker {
  public:
    AsyncDetectSimilarity(Nan::Callback *callback, cv::Mat image1, cv::Mat image2) :
      Nan::AsyncWorker(callback),
      image1(image1),
      image2(image2),
      dissimilarity(0) {
      }

    ~AsyncDetectSimilarity() {
    }

    void Execute() {

      cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("ORB");
      cv::Ptr<cv::DescriptorExtractor> extractor =
        cv::DescriptorExtractor::create("ORB");
      cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming");

      std::vector<cv::DMatch> matches;

      cv::Mat descriptors1 = cv::Mat();
      cv::Mat descriptors2 = cv::Mat();

      std::vector<cv::KeyPoint> keypoints1;
      std::vector<cv::KeyPoint> keypoints2;

      detector->detect(image1, keypoints1);
      detector->detect(image2, keypoints2);

      extractor->compute(image1, keypoints1, descriptors1);
      extractor->compute(image2, keypoints2, descriptors2);

      matcher->match(descriptors1, descriptors2, matches);

      double max_dist = 0;
      double min_dist = 100;

      for (int i = 0; i < descriptors1.rows; i++) {
        double dist = matches[i].distance;
        if (dist < min_dist) {
          min_dist = dist;
        }
        if (dist > max_dist) {
          max_dist = dist;
        }
      }

      std::vector<cv::DMatch> good_matches;
      double good_matches_sum = 0.0;

      for (int i = 0; i < descriptors1.rows; i++) {
        double distance = matches[i].distance;
        if (distance <= std::max(2 * min_dist, 0.02)) {
          good_matches.push_back(matches[i]);
          good_matches_sum += distance;
        }
      }

      dissimilarity = (double) good_matches_sum / (double) good_matches.size();
    }

    void HandleOKCallback() {
      Nan::HandleScope scope;

      Local<Value> argv[2];

      argv[0] = Nan::Null();
      argv[1] = Nan::New<Number>(dissimilarity);

      callback->Call(2, argv);
    }

  private:
    cv::Mat image1;
    cv::Mat image2;
    double dissimilarity;
};

NAN_METHOD(features2d::imageDissimilarity) {
  Nan::HandleScope scope;

  if (info.Length() <= (2) || !info[2]->IsFunction())
    return Nan::ThrowTypeError("Argument 2 must be a function");  
  Local<Function> cb = Local<Function>::Cast(info[2]);

  cv::Mat image1 = Nan::ObjectWrap::Unwrap<Mat>(info[0]->ToObject())->mat;
  cv::Mat image2 = Nan::ObjectWrap::Unwrap<Mat>(info[1]->ToObject())->mat;

  Nan::Callback *callback = new Nan::Callback(cb.As<Function>());

  Nan::AsyncQueueWorker(new AsyncDetectSimilarity(callback, image1, image2) );
  return;
}
