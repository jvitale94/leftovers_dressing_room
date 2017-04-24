//
//  detect_feats.cpp
//  OpenCVInstalledGood
//
//  Created by Jake Vitale on 4/19/17.
//  Copyright © 2017 Jake Vitale. All rights reserved.
//

#include "detect_feats.hpp"

#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"


using namespace cv;
using namespace cv::xfeatures2d;

void readme();

/** @function main */
int detect_feats( int argc, char** argv )
{
    if( argc != 3 )
    { return -1; }
    
    Mat img_1 = imread( argv[1], IMREAD_GRAYSCALE );
    Mat img_2 = imread( argv[2], IMREAD_GRAYSCALE );
    
    if( !img_1.data || !img_2.data )
    { return -1; }
    
    //-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
    int minHessian = 400;
    
    Ptr<SURF> detector = SURF::create( minHessian );
    
    std::vector<KeyPoint> keypoints_1, keypoints_2;
    Mat descriptors_1, descriptors_2;
    
    Mat mask;
    
    detector->detectAndCompute( img_1, mask, keypoints_1, descriptors_1 );
    detector->detectAndCompute( img_2, mask, keypoints_2, descriptors_2 );
    
    //-- Step 2: Matching descriptor vectors with a brute force matcher
    BFMatcher matcher(NORM_L2);
    std::vector< DMatch > matches;
    matcher.match( descriptors_1, descriptors_2, matches );
    
    //-- Draw matches
    Mat img_matches;
    drawMatches( img_1, keypoints_1, img_2, keypoints_2, matches, img_matches );
    
    resize(img_matches, img_matches, Size(img_matches.cols/6, img_matches.rows/6));
    
    //-- Show detected matches
    imshow("Matches", img_matches );
    
    waitKey(0);
    
    return 0;
}

/** @function readme */
void readme()
{ std::cout << " Usage: ./SURF_descriptor <img1> <img2>" << std::endl; }
