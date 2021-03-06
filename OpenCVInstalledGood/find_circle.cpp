//
//  find_circle.cpp
//  OpenCVInstalledGood
//
//  Created by Jake Vitale on 4/13/17.
//  Copyright © 2017 Jake Vitale. All rights reserved.
//

#include "find_circle.hpp"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace cv;

/** @function main */
int find_circle(int argc, char** argv)
{
    Mat src, src_gray;
    
    /// Read the image
    src = imread( argv[1], 1 );
    
    if( !src.data )
    { return -1; }
    
    /// Convert it to gray
    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    
    /// Reduce the noise so we avoid false circle detection
    GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );
    
    std::vector<Vec3f> circles;
    
    /// Apply the Hough Transform to find the circles
    HoughCircles( src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, 200, 100, 0, 0 );
    
    /// Draw the circles detected
    for( size_t i = 0; i < circles.size(); i++ )
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // circle center
        circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
    }
    
    /// Show your results
    namedWindow( "Hough Circle Transform Demo", WINDOW_AUTOSIZE );
    resize(src, src, Size(src.cols/4, src.rows/4));
    imshow( "Hough Circle Transform Demo", src );
    
    waitKey(0);
    return 0;
}
