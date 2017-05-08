//
//  match_shirts.cpp
//  OpenCVInstalledGood
//
//  Created by Jake Vitale on 4/19/17.
//  Copyright © 2017 Jake Vitale. All rights reserved.
//

#include "match_shirts.hpp"

#include "find_lines.hpp"

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "brighten.hpp"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d.hpp"

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

void print_pixelss(Mat image);
std::vector<Point2f> find_pixels_vals(Mat image, int r, int g, int b);

void match_shirt(int argc, char** argv )
{
    Mat image1 = imread(argv[1], 1);
    Mat image2 = imread(argv[2], 1);
    Mat image3 = Mat::zeros( image1.size(), image1.type() );
    Mat dst = Mat::zeros( image1.size(), image1.type() );
    
    bool samesize = image1.rows == image2.rows and image1.cols==image2.cols;
    
    assert(samesize && "Pics are not same size");
    
    std::vector<Point2f> points_1;
    std::vector<Point2f> points_2;
    
//    points_1 = find_pixels_vals(image1, 45, 5, 30);
//    points_2 = find_pixels_vals(image2, 45, 5, 30);
    
    std::fstream myfile("points_white.txt", std::ios_base::in);
    std::fstream myfile2("points_black.txt", std::ios_base::in);
    
    float a;
    float b;
    int count=0;
    while (myfile >> a >> b)
    {
        count++;
        printf("%f, %f, %d \n", a, b, count);
        Point2f p (a,b);
        points_1.push_back(p);
    }
    
    count=0;
    while (myfile2 >> a >> b)
    {
        count++;
        printf("%f, %f, %d \n", a, b, count);
        Point2f p (a,b);
        points_2.push_back(p);
    }

    
    Mat h = findHomography(points_2, points_1);
    
    warpPerspective(image2, image3, h, Size(image1.cols, image1.rows));
    
//    resize(image3, image3, Size(image3.cols/4, image3.rows/4));
//    imshow("image3", image3);
//    waitKey();
    
    printf("Image 1 size: %d, %d", image1.size().height, image1.size().width);
    printf("Image 3 size: %d, %d", image3.size().height, image3.size().width);
    
    printf("Image 1 depth: %d", image1.depth());
    printf("Image 3 depth: %d", image3.depth());

    addWeighted(image1, .5, image3, .5, 0.0, dst);
    
    imwrite("blended.png", dst);
    
    //waitKey(0);
}

void print_pixelss(Mat image)
{
    for (int i = 0; i<image.rows; i++)
    {
        for (int j = 0; j<image.cols; j++)
        {
            Vec3b color1 = image.at<Vec3b>(Point(i,j));
            printf("Pixel in image at point (%d, %d) is (%d, %d, %d)\n", i, j, color1[2], color1[1], color1[0]);
        }
    }
}

std::vector<Point2f> find_pixels_vals(Mat image, int r, int g, int b)
{
    int count = 0;
    std::vector<Point2f> points;
    
    for (int i = 0; i<image.rows; i++)
    {
        for (int j = 0; j<image.cols; j++)
        {
            Vec3b color = image.at<Vec3b>(Point(i,j));
            if (color[2]==r and color[1]==g and color[0]==b)
            {
                count ++;
                //printf("Pixel in image at point (%d, %d) is (%d, %d, %d)\n", i, j, color[2], color[1], color[0]);
                points.push_back(Point2f(i,j));
            }
        }
    }
    printf("There are %d pixels with the given RGB vals\n", count);
    return points;
}







