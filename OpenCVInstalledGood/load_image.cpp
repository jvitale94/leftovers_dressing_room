//
//  load_image.cpp
//  OpenCVInstalledGood
//
//  Created by Jake Vitale on 4/11/17.
//  Copyright © 2017 Jake Vitale. All rights reserved.
//

#include "load_image.hpp"

#include <opencv2/opencv.hpp>

using namespace cv;

int load_image( int argc, char** argv )
{
    //get first argument
    char* imageName = argv[1];
    
    //declare a Mat to hold images data
    Mat image;
    //read the image given as first argument into the Mat
    image = imread( imageName, 1 );
    
    if( argc < 2 || !image.data )
    {
        printf( " No image data \n " );
        return -1;
    }
    
    //create a new Mat to hold new image
    Mat output;
    
    
    
    //Gray out first image and put it in gray_image
    //cvtColor( image, gray_image, COLOR_BGR2GRAY );
    
    Mat kern = (Mat_<char>(3,3) <<  0, -1,  0,
                -1,  5, -1,
                0, -1,  0);
    
    filter2D(image, output, image.depth(), kern);
    
    //write gray_image
    imwrite( "testimages/sharpened.jpg", output);
    
    //make windows to hold images
    namedWindow( "Original Image", WINDOW_AUTOSIZE );
    namedWindow( "Edited image", WINDOW_AUTOSIZE );
    
    //show images
    imshow( imageName, image );
    imshow( "Gray image", output );
    
    waitKey(0);
    
    return 0;
}
