//
//  blur_stuff.cpp
//  OpenCVInstalledGood
//
//  Created by Jake Vitale on 4/11/17.
//  Copyright © 2017 Jake Vitale. All rights reserved.
//

#include "blur_stuff.hpp"

#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

/// Global Variables
int DELAY_CAPTION = 1500;
int DELAY_BLUR = 100;
int MAX_KERNEL_LENGTH = 31;

Mat source; Mat dst;
char window_name[] = "Filter Demo 1";

/// Function headers
int display_caption( char* caption );
int display_dst( int delay );

/**
 * function main
 */
int blur_stuff( int argc, char** argv )
{
    namedWindow( window_name, WINDOW_AUTOSIZE );
    
    /// Load the source image
    source = imread( argv[1], 1  );
    
    if( display_caption( "Original Image" ) != 0 ) { return 0; }
    
    dst = source.clone();
    if( display_dst( DELAY_CAPTION ) != 0 ) { return 0; }
    
    /// Applying Homogeneous blur
    if( display_caption( "Homogeneous Blur" ) != 0 ) { return 0; }
    
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
    { blur( source, dst, Size( i, i ), Point(-1,-1) );
        if( display_dst( DELAY_BLUR ) != 0 ) { return 0; } }
    
    /// Applying Gaussian blur
    if( display_caption( "Gaussian Blur" ) != 0 ) { return 0; }
    
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
    { GaussianBlur( source, dst, Size( i, i ), 0, 0 );
        if( display_dst( DELAY_BLUR ) != 0 ) { return 0; } }
    
    /// Applying Median blur
    if( display_caption( "Median Blur" ) != 0 ) { return 0; }
    
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
    { medianBlur ( source, dst, i );
        if( display_dst( DELAY_BLUR ) != 0 ) { return 0; } }
    
    /// Applying Bilateral Filter
    if( display_caption( "Bilateral Blur" ) != 0 ) { return 0; }
    
    for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
    { bilateralFilter ( source, dst, i, i*2, i/2 );
        if( display_dst( DELAY_BLUR ) != 0 ) { return 0; } }
    
    /// Wait until user press a key
    display_caption( "End: Press a key!" );
    
    waitKey(0);
    return 0;
}

int display_caption( char* caption )
{
    dst = Mat::zeros( source.size(), source.type() );
    putText( dst, caption,
            Point( source.cols/4, source.rows/2),
            FONT_HERSHEY_COMPLEX, 1, Scalar(255, 255, 255) );
    
    imshow( window_name, dst );
    int c = waitKey( DELAY_CAPTION );
    if( c >= 0 ) { return -1; }
    return 0;
}

int display_dst( int delay )
{
    imshow( window_name, dst );
    int c = waitKey ( delay );
    if( c >= 0 ) { return -1; }
    return 0;
}
