//
//  find_features.cpp
//  OpenCVInstalledGood
//
//  Created by Jake Vitale on 4/11/17.
//  Copyright © 2017 Jake Vitale. All rights reserved.
//

#include "find_features.hpp"

#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;
using namespace cv::xfeatures2d;

void readme2();

/** @function main */
int find_features( int argc, char** argv )
{
    if( argc != 3 )
    { readme2(); return -1; }
    
    Mat img_1 = imread( argv[1], IMREAD_GRAYSCALE );
    Mat img_2 = imread( argv[2], IMREAD_GRAYSCALE );
    
    if( !img_1.data || !img_2.data )
    { std::cout<< " --(!) Error reading images " << std::endl; return -1; }
    
    //-- Step 1: Detect the keypoints using SURF Detector
    int minHessian = 400;
    
    Ptr<SURF> detector = SURF::create( minHessian );
    
    std::vector<KeyPoint> keypoints_1, keypoints_2;
    
    detector->detect( img_1, keypoints_1 );
    detector->detect( img_2, keypoints_2 );
    
    //-- Draw keypoints
    Mat img_keypoints_1; Mat img_keypoints_2;
    
    drawKeypoints( img_1, keypoints_1, img_keypoints_1, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
    drawKeypoints( img_2, keypoints_2, img_keypoints_2, Scalar::all(-1), DrawMatchesFlags::DEFAULT );
    
    //-- Show detected (drawn) keypoints
    resize(img_keypoints_1, img_keypoints_1, Size(img_keypoints_1.cols/4, img_keypoints_1.rows/4));
    resize(img_keypoints_2, img_keypoints_2, Size(img_keypoints_2.cols/4, img_keypoints_2.rows/4));
    
    imshow("Keypoints 1", img_keypoints_1 );
    imshow("Keypoints 2", img_keypoints_2 );
    
    waitKey(0);
    
    return 0;
}

/** @function readme */
void readme2()
{ std::cout << " Usage: ./SURF_detector <img1> <img2>" << std::endl; }
