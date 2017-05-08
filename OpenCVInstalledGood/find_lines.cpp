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
#include <fstream>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;

CascadeClassifier face_cascade2;
String face_cascade_name2 = "haarcascade_frontalface_alt.xml";

std::vector<Point> detectFace(Mat frame);
void findBody(std::vector<Point> face_points, Mat image, Mat image2, Mat image3);
void blackOut(std::vector<Point> face_points, Mat image);
void find_sleeve(Mat image, Mat image2, Mat image3, int point_x, int point_y, int slope_x, int slope_y, int x_trav);
void change_Pixel(Mat image, Point point, int r, int g, int b);
void print_pixels(Mat image);
bool checkSlope(Mat image, int x, int y, int dir);
void count_pixels(Mat image, int r, int g, int b);


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
    Mat original_feature_points_only = imread(filename, 1);
    
    if(src.empty())
    {
        help();
        cout << "can not open " << filename << endl;
        return -1;
    }

    //GaussianBlur(src, src, Size( 5, 5 ), 0, 0 );
    
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
    
    findBody(face_points, cdst, original, original_feature_points_only);
    
    count_pixels(original_feature_points_only, 45, 5, 30);
    
    std::string name = argv[2];
    
    std::string name1 = ""+name+"_lines.png";
    std::string name2 = ""+name+"_circles.png";
    std::string name3 = ""+name+"_just_feature_points.png";
    
    imwrite(name1.c_str(), cdst);
    imwrite(name2.c_str(), original);
    imwrite(name3.c_str(), original_feature_points_only);
    
    //print_pixels(original_feature_points_only);
    
//    resize(cdst, cdst, Size(cdst.cols/4, cdst.rows/4));
//    resize(original, original, Size(original.cols/4, original.rows/4));
//    resize(original_feature_points_only, original_feature_points_only, Size(original_feature_points_only.cols/4, original_feature_points_only.rows/4));
//    
//    imshow("detected lines", cdst);
//    imshow("face", original);
//    imshow("feature points only", original_feature_points_only);
//    
//    waitKey();
    
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

void findBody(std::vector<Point> face_points, Mat image, Mat image2, Mat image3)
{
    //find center
    Point center = face_points[0];
    
    
    //center of face is feature point
    change_Pixel(image3, center, 45, 5, 30);
    change_Pixel(image2, center, 45, 5, 30);
    
    ofstream myfile;
    myfile.open ("points_white.txt");
    string xcoord = to_string(center.x);
    string ycoord = to_string(center.y);
    myfile << xcoord + " " + ycoord + "\n";

    
    int r = image.rows;
    blackOut(face_points, image);
    Point tracker;
    
    //Finds Neck Line - i.e. first non black point below center of face
    for (int i = center.y; i < r; i++)
    {
        Vec3b vals = image.at<Vec3b>(i, center.x);
        uchar r = vals.val[2];
        if (r>0)
        {
            tracker = Point(center.x, i);
            circle(image, Point(center.x,i), 20, Scalar(255, 0, 0), 2, 8, 0);
            circle(image2, Point(center.x,i), 20, Scalar(255, 0, 0), 2, 8, 0);
            
            change_Pixel(image, Point(center.x, i), 45, 5, 30);
            change_Pixel(image2, Point(center.x, i), 45, 5, 30);
            change_Pixel(image3, Point(center.x, i), 45, 5, 30);
            //circle(image3, Point(center.x,i), 20, Scalar(255, 0, 0), 2, 8, 0);
            
            break;
        }
    }
    
    xcoord = to_string(tracker.x);
    ycoord = to_string(tracker.y);
    myfile << xcoord + " " + ycoord + "\n";
    myfile.close();
    
    //left sleeve
    printf("Left Sleeve\n");
    find_sleeve(image, image2, image3, tracker.x-500, tracker.y+500, 1, 1, 1);
    
    //right sleeve
    printf("\n\nRight Sleeve\n");
    find_sleeve(image, image2, image3, tracker.x+500, tracker.y+500, -1, 1, -1);

    //find waist line
    Point center_waist;
    Point right_waist;
    Point left_waist;
    
    for (int i = tracker.y+700; i < r; i++)
    {
        Vec3b color = image.at<Vec3b>(Point(tracker.x,i));
        if (color[2]>0)
        {
            center_waist = Point (tracker.x, i);
            break;
        }
        color[0] = 0;
        color[1] = 255;
        color[2] = 0;
        image.at<Vec3b>(Point(tracker.x,i)) = color;
    }
    
    int slope = 0;
    int threshold = 10;
    int point_x = center_waist.x;
    int point_y = center_waist.y;
    
    int count_feats = 0;
    std::vector<Point> rightpoints;
    std::vector<Point> leftpoints;
    std::vector<Point> orderedpoints;
    
    //right waist
    while (slope-threshold<0)
    {
        //keeps looking for points consistent with already found slope
        for (int i = -5; i<5; i++)
        {
            Vec3b color = image.at<Vec3b>(Point(point_x+1,point_y+i));
            if (color[2]>0)
            {
                point_x = point_x + 1;
                point_y = point_y+i;
                rightpoints.push_back(Point(point_x,point_y));
                slope = i;
                color[0] = 0;
                color[1] = 255;
                color[2] = 0;
                image.at<Vec3b>(Point(point_x, point_y)) = color;
                //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);
                break;
            }
            if (i==4)
            {
                slope = 11;
            }
        }
    }
    
    right_waist = Point(point_x, point_y);
    
    point_x = center_waist.x;
    point_y = center_waist.y;
    slope = 0;
    
    //left waist
    while (slope-threshold<0)
    {
        //keeps looking for points consistent with already found slope
        for (int i = -5; i<5; i++)
        {
            Vec3b color = image.at<Vec3b>(Point(point_x - 1,point_y+i));
            if (color[2]>0)
            {
                point_x = point_x - 1;
                point_y = point_y+i;
                leftpoints.push_back(Point(point_x, point_y));
                slope = i;
                color[0] = 0;
                color[1] = 255;
                color[2] = 0;
                image.at<Vec3b>(Point(point_x, point_y)) = color;
                //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);
                break;
            }
            if (i==4)
            {
                slope = 11;
            }
        }
    }
    
    for (vector<Point>::reverse_iterator i = leftpoints.rbegin();
         i != leftpoints.rend(); ++i ) {
        orderedpoints.push_back(Point(i->x, i->y));
    }
    
    orderedpoints.push_back(center_waist);
    
    for (Point p: rightpoints)
    {
        orderedpoints.push_back(p);
    }
    
    left_waist = Point(point_x, point_y);
    
    int feat_mod = 1;
    
    float distance = sqrt((right_waist.x-left_waist.x)*(right_waist.x-left_waist.x)
                          +(right_waist.y-left_waist.y)*(right_waist.y-left_waist.y));
    float interval = distance/67;
    int interval_rounded = floor(interval)+1;
    
    int index = 0;
    
    orderedpoints.pop_back();
    
    ofstream waistfile;
    waistfile.open ("points_white.txt", std::ios::app);
    
    for (Point p : orderedpoints)
    {
        if (index == 0)
        {
            index++;
        }
        else
        {
            feat_mod++;
            if (feat_mod % (interval_rounded-1) == 0 and count_feats < 2)
            {
                feat_mod = 0;
                count_feats++;
                change_Pixel(image3, p, 45, 5, 30);
                string xcoord = to_string(p.x);
                string ycoord = to_string(p.y);
                waistfile << xcoord + " " + ycoord + "\n";
                circle(image2, p, 20, Scalar(255, 0, 5*count_feats), 2, 8, 0);
            }
            else if (feat_mod % interval_rounded == 0 and count_feats > 2 and count_feats < 65)
            {
                feat_mod = 0;
                count_feats++;
                change_Pixel(image3, p, 45, 5, 30);
                string xcoord = to_string(p.x);
                string ycoord = to_string(p.y);
                waistfile << xcoord + " " + ycoord + "\n";
                circle(image2, p, 20, Scalar(255, 0, 5*count_feats), 2, 8, 0);
            }
            
            else if (feat_mod % (interval_rounded-1) == 0 and count_feats < 67)
            {
                feat_mod = 0;
                count_feats++;
                change_Pixel(image3, p, 45, 5, 30);
                string xcoord = to_string(p.x);
                string ycoord = to_string(p.y);
                waistfile << xcoord + " " + ycoord + "\n";
                circle(image2, p, 20, Scalar(255, 0, 5*count_feats), 2, 8, 0);
            }
            //change_Pixel(image2, p, 0, 255, 0);
            //printf("(%d, %d)\n", p.x, p.y);
        }
    }
    
    waistfile.close();
    
    printf("%d\n", count_feats);
    
//    circle(image, center_waist, 20, Scalar(255, 0, 0), 2, 8, 0);
//    circle(image2, center_waist, 20, Scalar(255, 0, 0), 2, 8, 0);
//    change_Pixel(image, center_waist, 45, 5, 30);
//    change_Pixel(image2, center_waist, 45, 5, 30);
//    change_Pixel(image3, center_waist, 45, 5, 30);
//    circle(image3, center_waist, 20, Scalar(255, 0, 0), 2, 8, 0);
    
    circle(image, right_waist, 20, Scalar(255, 0, 0), 2, 8, 0);
    circle(image2, right_waist, 20, Scalar(255, 0, 0), 2, 8, 0);
    change_Pixel(image, right_waist, 45, 5, 30);
    change_Pixel(image2, right_waist, 45, 5, 30);
    change_Pixel(image3, right_waist, 45, 5, 30);
    //circle(image3, right_waist, 20, Scalar(255, 0, 0), 2, 8, 0);
    
    circle(image, left_waist, 20, Scalar(255, 0, 0), 2, 8, 0);
    circle(image2, left_waist, 20, Scalar(255, 0, 0), 2, 8, 0);
    change_Pixel(image, left_waist, 45, 5, 30);
    change_Pixel(image2, left_waist, 45, 5, 30);
    change_Pixel(image3, left_waist, 45, 5, 30);
    //circle(image3, left_waist, 20, Scalar(255, 0, 0), 2, 8, 0);

}

void blackOut(std::vector<Point> face_points, Mat image)
{
    Point p1 = face_points[1];
    Point p2 = face_points[2];
    Point p3 = face_points[3];
    Point p4 = face_points[4];
    
    for (int x = p1.x; x<p3.x; x++)
    {
        for (int y = p1.y; y<p2.y+100; y++)
        {
            Vec3b color = image.at<Vec3b>(Point(x,y));
            color[0] = 0;
            color[1] = 0;
            color[2] = 0;
            image.at<Vec3b>(Point(x,y)) = color;
        }
    }
}

void find_sleeve(Mat image, Mat image2, Mat image3, int point_x, int point_y, int slope_x, int slope_y, int x_trav)
{
    //Finds sleeve
    while (point_x>0 && point_x<image.cols)
    {
        Vec3b color = image.at<Vec3b>(Point(point_x,point_y));
        if (color[2]>0)
        {
            //circle(image2, Point(point_x,point_y), 20, Scalar(0, 0, 255), 2, 8, 0);
            
            //****if bad point, black out surrounding pixels*** <---- idea for later???
            bool goodpoint = checkSlope(image, point_x, point_y, x_trav);
            if (goodpoint)
            {
                break;
            }
            //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);
        }
        //draws green line from neckline to sleeve
        color[0] = 0;
        color[1] = 255;
        color[2] = 0;
        image.at<Vec3b>(Point(point_x,point_y)) = color;
        point_x = point_x - slope_x;
        point_y = point_y + slope_y;
    }
    
    //Slope stores slope of sleeve
    int slope = 0;
    int finalslope = 0;
    //Draw the points traced on sleeve in green
    
    std::vector<Point> colorpoints;
    colorpoints.push_back(Point(point_x, point_y));
    
    //Find the first point on sleeve next to found point, assuming it is below
    for (int i = 0; i<50; i++)
    {
        Vec3b color = image.at<Vec3b>(Point(point_x+x_trav,point_y+i));
        if (color[2]>0)
        {
            point_x = point_x + x_trav;
            point_y = point_y+i;
            colorpoints.push_back(Point(point_x, point_y));
            slope = i;
            //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);
            break;
        }
    }
    
    printf("Slope is %d\n", slope);
    
    int threshold = 3;
    Point inside_corner;
    Point outside_corner;
    
    //find inside corner of sleeve
    while (slope-threshold<0)
    {
        //keeps looking for points consistent with already found slope
        for (int i = -3; i<4; i++)
        {
            Vec3b color = image.at<Vec3b>(Point(point_x+x_trav,point_y+i));
            if (color[2]>0)
            {
                point_x = point_x + x_trav;
                point_y = point_y+i;
                colorpoints.push_back(Point(point_x, point_y));
                slope = i;
                finalslope = i;
                //printf("Slope is: %d\n", slope);
                if (slope<-1)
                {
                    slope = 7;
                }
                //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);
                break;
            }
            if (i==3)
            {
                slope = 7;
            }
        }
    }
    
    inside_corner = Point(point_x, point_y);
    
    //draw green points of sleeve
//    for (Point p : colorpoints)
//    {
//        if (feat_mod % 15 == 0)
//        {
//            feat_mod = 0;
//            count_feats++;
//            change_Pixel(image3, p, 45, 5, 30);
//            circle(image2, p, 20, Scalar(255, 150, 1*10*count_feats), 2, 8, 0);
//        }
//        feat_mod++;
//        change_Pixel(image2, p, 0, 255, 0);
//    }
    
    colorpoints.clear();
    colorpoints.push_back(inside_corner);
    slope = finalslope;
//    point_x = original.x; //original was the first point found on the sleeve, unnecessary now that we
                            //can trace the color points from one corner
//    point_y = original.y;

    //find outside corner of sleeve
    while (slope-threshold<0)
    {
        //keeps looking for points consistent with already found slope
        for (int i = -5; i<5; i++)
        {
            Vec3b color = image.at<Vec3b>(Point(point_x-x_trav,point_y+i));
            if (color[2]>0)
            {
                point_x = point_x - x_trav;
                point_y = point_y+i;
                colorpoints.push_back(Point(point_x, point_y));
                slope = i;
                //printf("Slope is: %d\n", slope);
                if (slope<-4)
                {
                    //printf("Here 1\n");
                    slope = 7;
                }
                //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);
                break;
            }
            if (i==4)
            {
                //printf("Here 2\n");
                slope = 7;
            }
        }
    }
    
    outside_corner = Point(point_x, point_y);
    
    int count_feats = 0;
    int feat_mod = 1;
    
    printf("inside corner: %d, %d\n", inside_corner.x, inside_corner.y);
    printf("outside corner: %d, %d\n", outside_corner.x, outside_corner.y);
    
    float distance = sqrt((outside_corner.x-inside_corner.x)*(outside_corner.x-inside_corner.x)
                            +(outside_corner.y-inside_corner.y)*(outside_corner.y-inside_corner.y));
    float interval = distance/49;
    int interval_rounded = floor(interval)+1;
    
    int index = 0;
    
    colorpoints.pop_back();
    
    ofstream sleavefile;
    sleavefile.open ("points_white.txt", std::ios::app);
    
    std::vector<Point> pointstoadd;
    
    for (Point p : colorpoints)
    {
        if (index == 0)
        {
            index++;
        }
        else
        {
            feat_mod++;
            if (feat_mod % (interval_rounded-1) == 0 and count_feats < 2)
            {
                feat_mod = 0;
                count_feats++;
                change_Pixel(image3, p, 45, 5, 30);
                pointstoadd.push_back(p);
                circle(image2, p, 20, Scalar(255, 0, 5*count_feats), 2, 8, 0);
            }
            else if (feat_mod % interval_rounded == 0 and count_feats > 2 and count_feats < 47)
            {
                feat_mod = 0;
                count_feats++;
                change_Pixel(image3, p, 45, 5, 30);
                pointstoadd.push_back(p);
                circle(image2, p, 20, Scalar(255, 0, 5*count_feats), 2, 8, 0);
            }
            
            else if (feat_mod % (interval_rounded-1) == 0 and count_feats < 49)
            {
                feat_mod = 0;
                count_feats++;
                change_Pixel(image3, p, 45, 5, 30);
                pointstoadd.push_back(p);
                circle(image2, p, 20, Scalar(255, 0, 5*count_feats), 2, 8, 0);
            }
            //change_Pixel(image2, p, 0, 255, 0);
            //printf("(%d, %d)\n", p.x, p.y);
        }
    }
    
    for (vector<Point>::reverse_iterator i = pointstoadd.rbegin();
         i != pointstoadd.rend(); ++i ) {
        string xcoord = to_string(i->x);
        string ycoord = to_string(i->y);
        sleavefile << xcoord + " " + ycoord + "\n";
    }
    
    
    
    circle(image, inside_corner, 20, Scalar(255, 0, 0), 2, 8, 0);
    circle(image2, inside_corner, 20, Scalar(255, 0, 0), 2, 8, 0);
    //circle(image3, inside_corner, 20, Scalar(255, 0, 0), 2, 8, 0);
    change_Pixel(image, inside_corner, 45, 5, 30);
    change_Pixel(image2, inside_corner, 45, 5, 30);
    change_Pixel(image3, inside_corner, 45, 5, 30);
    
    string xcoord = to_string(inside_corner.x);
    string ycoord = to_string(inside_corner.y);
    sleavefile << xcoord + " " + ycoord + "\n";
    
    circle(image, outside_corner, 20, Scalar(255, 0, 0), 2, 8, 0);
    circle(image2, outside_corner, 20, Scalar(255, 0, 0), 2, 8, 0);
    //circle(image3, outside_corner, 20, Scalar(255, 0, 0), 2, 8, 0);
    change_Pixel(image, outside_corner, 45, 5, 30);
    change_Pixel(image2, outside_corner, 45, 5, 30);
    change_Pixel(image3, outside_corner, 45, 5, 30);
    
    xcoord = to_string(outside_corner.x);
    ycoord = to_string(outside_corner.y);
    sleavefile << xcoord + " " + ycoord + "\n";
    
    sleavefile.close();
    
    printf ("Count feats = %d\n", count_feats);
    
}

void change_Pixel(Mat image, Point point, int r, int g, int b)
{
    Vec3b color = image.at<Vec3b>(point);
    color[0] = b;
    color[1] = g;
    color[2] = r;
    image.at<Vec3b>(point) = color;
}

void print_pixels(Mat image)
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

bool checkSlope(Mat image, int x, int y, int dir)
{
    int slope = 0;
    
    for (int i = 0; i<20; i++)
    {
        Vec3b color = image.at<Vec3b>(Point(x+10*dir-dir*i, y+10));
        if (color[2]>0)
        {
            slope = i;
            //circle(image, Point(point_x,point_y), 20, Scalar(255, 0, 0), 2, 8, 0);
            break;
        }
        color[0] = 0;
        color[1] = 255;
        color[2] = 0;
        image.at<Vec3b>(Point(x,y)) = color;
    }
    
    //printf("Check slope is: %d\n", slope);
    if (slope==0)
        return true;
    return false;
}

void count_pixels(Mat image, int r, int g, int b)
{
    int count = 0;
    
    for (int i = 0; i<image.rows; i++)
    {
        for (int j = 0; j<image.cols; j++)
        {
            Vec3b color = image.at<Vec3b>(Point(i,j));
            if (color[2]==r and color[1]==g and color[0]==b)
            {
                count ++;
                //printf("Pixel in image at point (%d, %d) is (%d, %d, %d)\n", i, j, color[2], color[1], color[0]);
            }
        }
    }
    printf("There are %d pixels with the given RGB vals\n", count);
}








