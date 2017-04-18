//
//  blend.cpp
//  OpenCVInstalledGood
//
//  Created by Jake Vitale on 4/11/17.
//  Copyright © 2017 Jake Vitale. All rights reserved.
//

#include "blend.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;

int blend( int argc, char** argv )
{
    double alpha = 0.5; double beta; double input;
    
    Mat src1, src2, dst;
    
    /// Ask the user enter alpha
    std::cout<<" Simple Linear Blender "<<std::endl;
    std::cout<<"-----------------------"<<std::endl;
    std::cout<<"* Enter alpha [0-1]: ";
    std::cin>>input;
    
    /// We use the alpha provided by the user if it is between 0 and 1
    if( input >= 0.0 && input <= 1.0 )
    { alpha = input; }
    
    /// Read image ( same size, same type )
    src1 = imread( argv[1], 1 );
    src2 = imread( argv[2], 1 );
    
    if( !src1.data ) { printf("Error loading src1 \n"); return -1; }
    if( !src2.data ) { printf("Error loading src2 \n"); return -1; }
    
    /// Create Windows
    
    namedWindow("Linear Blend", CV_WINDOW_AUTOSIZE);
    
    beta = ( 1.0 - alpha );
    addWeighted( src1, alpha, src2, beta, 0.0, dst);
    
    resize(dst, dst, Size(dst.cols/2, dst.rows/2.5));
    imwrite( "testimages/blurred.jpg", dst);
    imshow( "Linear Blend", dst );
    
    waitKey(0);
    return 0;
}
