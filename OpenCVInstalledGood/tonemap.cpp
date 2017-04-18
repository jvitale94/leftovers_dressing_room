//
//  tonemap.cpp
//  OpenCVInstalledGood
//
//  Created by Jake Vitale on 4/11/17.
//  Copyright © 2017 Jake Vitale. All rights reserved.
//

#include "tonemap.hpp"

#include <opencv2/photo.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

void loadExposureSeq(String, vector<Mat>&, vector<float>&);

int tone(int, char**argv)
{
    vector<Mat> images;
    vector<float> times;
    loadExposureSeq(argv[1], images, times);
    
    Mat response;
    Ptr<CalibrateDebevec> calibrate = createCalibrateDebevec();
    calibrate->process(images, response, times);
    
    Mat hdr;
    Ptr<MergeDebevec> merge_debevec = createMergeDebevec();
    merge_debevec->process(images, hdr, times, response);
    
    Mat ldr;
    Ptr<TonemapDurand> tonemap = createTonemapDurand(2.2f);
    tonemap->process(hdr, ldr);
    
    Mat fusion;
    Ptr<MergeMertens> merge_mertens = createMergeMertens();
    merge_mertens->process(images, fusion);
    
    imwrite("fusion.png", fusion * 255);
    imwrite("ldr.png", ldr * 255);
    imwrite("hdr.hdr", hdr);
    
    return 0;
}

void loadExposureSeq(String path, vector<Mat>& images, vector<float>& times)
{
    path = path + std::string("/");
    
    //need to list different photos with exposure times
    ifstream list_file((path + "list.txt").c_str());
    string name;
    float val;
    while(list_file >> name >> val) {
        Mat img = imread(path + name);
        images.push_back(img);
        times.push_back(1 / val);
    }
    list_file.close();
}
