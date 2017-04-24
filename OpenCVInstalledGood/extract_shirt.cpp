//
//  extract_shirt.cpp
//  OpenCVInstalledGood
//
//  Created by Jake Vitale on 4/18/17.
//  Copyright © 2017 Jake Vitale. All rights reserved.
//

#include "extract_shirt.hpp"
#include "find_lines.hpp"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "brighten.hpp"


#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

void extract_shirt(int argc, char** argv )
{
    Mat image = imread(argv[1], 1);
    
    Mat output;
    
    Mat kern = (Mat_<char>(3,3) <<  -1, -1,  -1,
                -1,  9, -1,
                -1, -1,  -1);
    
    filter2D(image, output, image.depth(), kern);
    
    for (int i = 0; i<image.rows; i++)
    {
        for (int j = 0; j<image.cols; j++)
        {
            Vec3b color = image.at<Vec3b>(Point(i,j));
            if (color[0]>50 and color [2]<100)
            {
                color[0] = 0;
                color[1] = 255;
                color[2] = 0;
                image.at<Vec3b>(Point(i,j)) = color;
            }
            
            color = output.at<Vec3b>(Point(i,j));
            if (color[0]>50 and color [2]<100)
            {
                color[0] = 0;
                color[1] = 255;
                color[2] = 0;
                output.at<Vec3b>(Point(i,j)) = color;
            }
        }
    }
    
    Vec3b color = image.at<Vec3b>(Point(1200,1200));
    printf("(%d, %d, %d)\n", color[2], color[1], color[0]);
    color = image.at<Vec3b>(Point(100,100));
    printf("(%d, %d, %d)\n", color[2], color[1], color[0]);

    
    resize(image, image, Size(image.cols/4, image.rows/4));
    resize(output, output, Size(output.cols/4, output.rows/4));
    
    imshow("Shirt", image);
    imshow("Sharpened", output);
    
    
    waitKey();
    
    return;
    
}
