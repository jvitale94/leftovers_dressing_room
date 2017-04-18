//

//  find_lines.cpp

//  OpenCVInstalledGood

//

//  Created by Jake Vitale on 4/12/17.

//  Copyright © 2017 Jake Vitale. All rights reserved.

//



#include "find_lines.hpp"



#include "opencv2/highgui.hpp"

#include "opencv2/imgproc.hpp"

#include "opencv2/objdetect.hpp"



#include <iostream>

#include <stdio.h>



using namespace cv;

using namespace std;



CascadeClassifier face_cascade2;

String face_cascade_name2 = "haarcascade_frontalface_alt.xml";



std::vector<Point> detectFace(Mat frame);

void findBody(std::vector<Point> face_points, Mat image, Mat image2);

void blackOut(std::vector<Point> face_points, Mat image);



void help()

{

    cout << "\nThis program demonstrates line finding with the Hough transform.\n"

    "Usage:\n"

    "./houghlines <image_name>, Default is pic1.jpg\n" << endl;

}



int find_lines(int argc, char** argv)

{

    //help();

    const char* filename = argc >= 2 ? argv[1] : "pic1.jpg";

    

    Mat src = imread(filename, 0);

    Mat original = imread(filename, 1);

    

    if(src.empty())

    {

        help();

        cout << "can not open " << filename << endl;

        return -1;

    }



    GaussianBlur(src, src, Size( 5, 5 ), 0, 0 );

    

    Mat dst, cdst;

    

    Canny(src, dst, 50, 200, 3);

    cvtColor(dst, cdst, COLOR_GRAY2BGR);

    

#if 0

    vector<Vec2f> lines;

    HoughLines(dst, lines, 1, CV_PI/180, 100, 0, 0 );

    

    for( size_t i = 0; i < lines.size(); i++ )

    {

        float rho = lines[i][0], theta = lines[i][1];

        Point pt1, pt2;

        double a = cos(theta), b = sin(theta);

        double x0 = a*rho, y0 = b*rho;

        pt1.x = cvRound(x0 + 1000*(-b));

        pt1.y = cvRound(y0 + 1000*(a));

        pt2.x = cvRound(x0 - 1000*(-b));

        pt2.y = cvRound(y0 - 1000*(a));

        line( cdst, pt1, pt2, Scalar(0,0,255), 3, LINE_AA);

    }

#else

    vector<Vec4i> lines;

    HoughLinesP(dst, lines, 1, CV_PI/180, 50, 50, 10 );

    for( size_t i = 0; i < lines.size(); i++ )

    {

        Vec4i l = lines[i];

        line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);

    }

#endif

    

    std::vector<Point> face_points = detectFace(original);

    //iterate through pixels and find the right line below. scan to left and right to find shoulders

    

    findBody(face_points, cdst, original);

    

    

    imwrite( "testimages/detected_lines.jpg", cdst);

    imwrite( "testimages/detected_src.jpg", original);

    

    resize(cdst, cdst, Size(cdst.cols/4, cdst.rows/4));

    resize(original, original, Size(original.cols/4, original.rows/4));

    

    imshow("detected lines", cdst);

    imshow("face", original);

    

    waitKey();

    

    return 0;

}



std::vector<Point> detectFace(Mat frame)

{

    if( !face_cascade2.load( face_cascade_name2 ) ){ printf("--(!)Error loading face cascade\n"); };

    

    std::vector<Rect> faces;

    

    Mat frame_gray;

    

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );

    equalizeHist( frame_gray, frame_gray );

    

    face_cascade2.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );

    

    Point finalpoint;

    std::vector<Point> face_points;

    

    for( size_t i = 0; i < 1; i++ )

    {

        Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );

        

        face_points.push_back(center);

        face_points.push_back(Point(faces[i].x,faces[i].y));

        face_points.push_back(Point(faces[i].x,faces[i].y+faces[i].height));

        face_points.push_back(Point(faces[i].x+faces[i].width,faces[i].y));

        face_points.push_back(Point(faces[i].x+faces[i].width,faces[i].y+faces[i].height));



        ellipse( frame, center, Size( faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0 );

        

    }

    

    return face_points;

}



void findBody(std::vector<Point> face_points, Mat image, Mat image2)

{

    //find center

    Point center = face_points[0];

    int r = image.rows;

    

    blackOut(face_points, image);

    

    Point tracker;

    

    for (int i = center.y; i < r; i++)

    {

        Vec3b vals = image.at<Vec3b>(i, center.x);

        uchar r = vals.val[2];

        if (r>0)

        {

            tracker = Point(center.x, i);

            circle(image, Point(center.x,i), 20, Scalar(255, 0, 0), 2, 8, 0);

            circle(image2, Point(center.x,i), 20, Scalar(255, 0, 0), 2, 8, 0);

            break;

        }

    }

    

    int slope_x = 1;

    int slope_y = 1;

    

    int point_x = tracker.x-30;

    int point_y = tracker.y+30;

    

    //Make this a method

    while (point_x>0)

    {

        Vec3b color = image.at<Vec3b>(Point(point_x,point_y));

        if (color[2]>0)

        {

            //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);

            break;

        }

        //draws green line from neckline to sleeve

        color[0] = 0;

        color[1] = 255;

        color[2] = 0;

        image.at<Vec3b>(Point(point_x,point_y)) = color;

        point_x = point_x - slope_x;

        point_y = point_y + slope_y;

    }

    

    int slope = 0;

    std::vector<Point> colorpoints;

    colorpoints.push_back(Point(point_x, point_y));

    

    for (int i = 0; i<50; i++)

    {

        Vec3b color = image.at<Vec3b>(Point(point_x+1,point_y+i));

        if (color[2]>0)

        {

            point_x = point_x+1;

            point_y = point_y+i;

            colorpoints.push_back(Point(point_x, point_y));

            slope = i;

            //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);

            break;

        }

    }

    

    int threshold = 6;

    

    while (slope-threshold<0)

    {

        for (int i = -5; i<5; i++)

        {

            Vec3b color = image.at<Vec3b>(Point(point_x+1,point_y+i));

            if (color[2]>0)

            {

                point_x = point_x+1;

                point_y = point_y+i;

                colorpoints.push_back(Point(point_x, point_y));

                slope = i;

                //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);

                break;

            }

            if (i==4)

            {

                slope = 7;

            }

        }

    }

    

    for (Point p : colorpoints)

    {

        Vec3b color = image.at<Vec3b>(p);

        color[0] = 0;

        color[1] = 255;

        color[2] = 0;

        image.at<Vec3b>(p) = color;

    }

    

    circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);

    circle(image2, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);

    

    

    ////////////Right Sleeve////////////////

    point_x = tracker.x+30;

    point_y = tracker.y+30;

    

    while (point_x>0)

    {

        Vec3b color = image.at<Vec3b>(Point(point_x,point_y));

        if (color[2]>0)

        {

            //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);

            break;

        }

        color[0] = 0;

        color[1] = 255;

        color[2] = 0;

        image.at<Vec3b>(Point(point_x,point_y)) = color;

        point_x = point_x + slope_x;

        point_y = point_y + slope_y;

    }

    

    slope = 0;

    colorpoints.clear();

    colorpoints.push_back(Point(point_x, point_y));

    

    for (int i = 0; i<50; i++)

    {

        Vec3b color = image.at<Vec3b>(Point(point_x-1,point_y+i));

        if (color[2]>0)

        {

            point_x = point_x-1;

            point_y = point_y+i;

            colorpoints.push_back(Point(point_x, point_y));

            slope = i;

            //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);

            break;

        }

    }

    

    while (slope-threshold<0)

    {

        for (int i = -5; i<5; i++)

        {

            Vec3b color = image.at<Vec3b>(Point(point_x-1,point_y+i));

            if (color[2]>0)

            {

                point_x = point_x-1;

                point_y = point_y+i;

                colorpoints.push_back(Point(point_x, point_y));

                slope = i;

                //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);

                break;

            }

            if (i==4)

            {

                slope = 7;

            }

        }

    }

    

    for (Point p : colorpoints)

    {

        Vec3b color = image.at<Vec3b>(p);

        color[0] = 0;

        color[1] = 255;

        color[2] = 0;

        image.at<Vec3b>(p) = color;

    }

    

    circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);

    circle(image2, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);

    

    //Found Sleeves

    

    for (int i = tracker.y+40; i < r; i++)

    {

        Vec3b color = image.at<Vec3b>(Point(tracker.x,i));

        if (color[2]>0)

        {

            circle(image, Point(tracker.x,i), 20, Scalar(255, 0, 0), 2, 8, 0);

            circle(image2, Point(tracker.x,i), 20, Scalar(255, 0, 0), 2, 8, 0);

            break;

        }

        color[0] = 0;

        color[1] = 255;

        color[2] = 0;

        image.at<Vec3b>(Point(tracker.x,i)) = color;

    }

}



void blackOut(std::vector<Point> face_points, Mat image)

{

    Point p1 = face_points[1];

    Point p2 = face_points[2];

    Point p3 = face_points[3];

    Point p4 = face_points[4];

    

    for (int x = p1.x; x<p3.x; x++)

    {

        for (int y = p1.y; y<p2.y; y++)

        {

            Vec3b color = image.at<Vec3b>(Point(x,y));

            color[0] = 0;

            color[1] = 0;

            color[2] = 0;

            image.at<Vec3b>(Point(x,y)) = color;

        }

    }

}

















