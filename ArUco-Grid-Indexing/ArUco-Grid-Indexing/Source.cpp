#include <opencv2/aruco.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

//using namespace cv;
using namespace std;

/// Global variables
cv::Mat src, erosion_dst, dilation_dst;

int erosion_elem = 0;
int erosion_size = 0;
int dilation_elem = 0;
int dilation_size = 0;
int const max_elem = 2;
int const max_kernel_size = 21;

cv::Point2f test;

void genMarkers() {
  cv::Mat markerImage1, markerImage2, markerImage3, markerImage4;
  cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
  //cv::aruco::drawMarker(dictionary, 23, 200, markerImage, 1);

  dictionary.drawMarker(0, 200, markerImage1, 1);
  dictionary.drawMarker(1, 200, markerImage2, 1);
  dictionary.drawMarker(2, 200, markerImage3, 1);
  dictionary.drawMarker(3, 200, markerImage4, 1);

  imshow("marker1", markerImage1);
  imshow("marker2", markerImage2);
  imshow("marker3", markerImage3);
  imshow("marker4", markerImage4);

  cv::waitKey(0);
}

static bool readDetectorParameters(string filename, cv::Ptr<cv::aruco::DetectorParameters> &params) {
  cv::FileStorage fs(filename, cv::FileStorage::READ);
  if (!fs.isOpened())
    return false;
  fs["adaptiveThreshWinSizeMin"] >> params->adaptiveThreshWinSizeMin;
  fs["adaptiveThreshWinSizeMax"] >> params->adaptiveThreshWinSizeMax;
  fs["adaptiveThreshWinSizeStep"] >> params->adaptiveThreshWinSizeStep;
  fs["adaptiveThreshConstant"] >> params->adaptiveThreshConstant;
  fs["minMarkerPerimeterRate"] >> params->minMarkerPerimeterRate;
  fs["maxMarkerPerimeterRate"] >> params->maxMarkerPerimeterRate;
  fs["polygonalApproxAccuracyRate"] >> params->polygonalApproxAccuracyRate;
  fs["minCornerDistanceRate"] >> params->minCornerDistanceRate;
  fs["minDistanceToBorder"] >> params->minDistanceToBorder;
  fs["minMarkerDistanceRate"] >> params->minMarkerDistanceRate;
  fs["doCornerRefinement"] >> params->doCornerRefinement;
  fs["cornerRefinementWinSize"] >> params->cornerRefinementWinSize;
  fs["cornerRefinementMaxIterations"] >> params->cornerRefinementMaxIterations;
  fs["cornerRefinementMinAccuracy"] >> params->cornerRefinementMinAccuracy;
  fs["markerBorderBits"] >> params->markerBorderBits;
  fs["perspectiveRemovePixelPerCell"] >> params->perspectiveRemovePixelPerCell;
  fs["perspectiveRemoveIgnoredMarginPerCell"] >> params->perspectiveRemoveIgnoredMarginPerCell;
  fs["maxErroneousBitsInBorderRate"] >> params->maxErroneousBitsInBorderRate;
  fs["minOtsuStdDev"] >> params->minOtsuStdDev;
  fs["errorCorrectionRate"] >> params->errorCorrectionRate;
  return true;
}

vector<cv::Point2f> findCorners(vector<vector<cv::Point2f>> corners) {
  
  cv::Point2f midPoint;
  midPoint.x = 0;
  midPoint.y = 0;

  for (int i = 0; i < corners.size(); i++) {
    for (int j = 0; j < 4; j++) {
      midPoint.x += corners.at(i).at(j).x;
      midPoint.y += corners.at(i).at(j).y;
    }
  }

  midPoint.x = midPoint.x / (corners.size() * 4);
  midPoint.y = midPoint.y / (corners.size() * 4);

  //test = midPoint;

 

  vector<cv::Point2f> gridCorners(4);

  for (int i = 0; i < corners.size(); i++) {
    float pDist = INFINITY;
    for (int j = 0; j < 4; j++) {
      float dist = sqrt( pow( abs(midPoint.x - corners.at(i).at(j).x), 2.0) + pow(abs(midPoint.y - corners.at(i).at(j).y) , 2.0) );

      if (dist < pDist) {
        pDist = dist;
        gridCorners.at(i) = corners.at(i).at(j);
      }
      
    }
  }

  return gridCorners;
}

void drawGrid(cv::Mat imageCopy, vector<cv::Point2f> gridCorners, int horiz, int vert) {

}

int main(int, char** argv)
{
  
  cv::Mat inputImage;

  cv::Mat image, imageCopy;

  string imageName("../Images/ArUcoGridLarge.png"); // by default
  image = cv::imread(imageName.c_str(), cv::IMREAD_COLOR); // Read the file

  int dictionaryId = 10; // alias for the DICT_6X6_250 dictionary

  cv::Ptr<cv::aruco::Dictionary> dictionary =
    cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionaryId));

  cv::Ptr<cv::aruco::DetectorParameters> detectorParams = cv::aruco::DetectorParameters::create();

  vector< int > ids;
  vector< vector< cv::Point2f > > corners, rejected;

  cv::aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);
  readDetectorParameters("../detector_params.yml", detectorParams);

  image.copyTo(imageCopy);
  if (ids.size() > 0) {
    cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
  }

  vector<cv::Point2f> gridCorners = findCorners(corners);

  cv::circle(imageCopy, gridCorners.at(0), 10, cv::Scalar(255, 255, 255), CV_FILLED, 8, 0);
  cv::circle(imageCopy, gridCorners.at(1), 10, cv::Scalar(255, 255, 255), CV_FILLED, 8, 0);
  cv::circle(imageCopy, gridCorners.at(2), 10, cv::Scalar(255, 255, 255), CV_FILLED, 8, 0);
  cv::circle(imageCopy, gridCorners.at(3), 10, cv::Scalar(255, 255, 255), CV_FILLED, 8, 0);
  

  cv::imshow("test", imageCopy);



  cv::waitKey(0);

  return 0;
}