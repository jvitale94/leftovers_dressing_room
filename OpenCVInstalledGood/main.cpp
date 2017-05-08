#include <stdio.h>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>
#include "load_image.hpp"
#include "find_features.hpp"
#include "blend.hpp"
#include "brighten.hpp"
#include "draw_stuff.hpp"
#include "fourier.hpp"
#include "blur_stuff.hpp"
#include "erode_dilute.hpp"
#include "morph.hpp"
#include "face.hpp"
#include "tonemap.hpp"
#include "find_lines.hpp"
#include "find_logo.hpp"
#include "find_circle.hpp"
#include "extract_shirt.hpp"
#include "match_shirts.hpp"
#include "detect_feats.hpp"
#include "find_lines_black.hpp"

using namespace cv;
using namespace cv::xfeatures2d;

void readme();

//* @function main 
int main( int argc, char** argv )
{
    //detect_feats(argc, argv);
    //extract_shirt(argc, argv);
    //find_lines(argc, argv);
    //find_lines_black(argc, argv);
    match_shirt(argc, argv);
    //find_logo(argc, argv);
    //find_circle(argc, argv);
    //tone(argc, argv);
    //face();
    //morph(argc, argv);
    //eandd(argc, argv);
    //blur_stuff(argc, argv);
    //fourier(argc, argv);
    //draw_stuff();
    //brighten(argc, argv);
    //blend(argc, argv);
    //load_image(argc, argv);
    //find_features(argc, argv);
}

