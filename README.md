# leftovers_dressing_room

This repository contains code that matches up the Leftovers t-shirt with an input image. The relevant files are main.cpp, find_lines.cpp, find_lines_black.cpp, and match_shirts.cpp. All other .cpp files were imported for the sake of familiarizing myself with the OpenCV library. 

## Installation

First, you must have OpenCV installed in order to run this program. Follow [this link](http://docs.opencv.org/2.4/doc/tutorials/introduction/linux_install/linux_install.html#linux-installation) on instructions of how to install OpenCV. You also need to download and inclue opencv_contrib when you make the files. Follow [this link](https://github.com/opencv/opencv_contrib) on instructions of how to include opencv_contrib in your library installation. Lastly, you will need to link your XCode environment with your OpenCV library once you have installed it. [This link](https://blogs.wcode.org/2014/11/howto-setup-xcode-6-1-to-work-with-opencv-libraries/) is a fansastic explanation on how to link the libraries properly.

## Files

The find_lines.cpp file is the meat and potatoes of this project. This is the file that takes the input image, and finds feature points on the face, neckline, sleeves, and waistline. In order to run this file, build the project in XCode, and the run ./OpenCVInstalledGood <inputimage> <fileprefix>. This will save 4 files to the direcotry you are working in:
* fileprefix_lines.png - the decomposition of the input image into its lines/edges. This is the image that I do most of the work of finding features on. It also includes green lines that show the path of the tracker that I used to find feature points. The neckline, corners of the sleeve, and corners of the waistline are circled.        
* fileprefix_circles.png - This is the input image with all the feature points circled.    
* fileprefix_just_feature_points.png - This is the same as the circles image, but instead of each feature being circled, the actual feature points are colored to a specific color.
* points_white.txt - This is the list of all feature points, which is used later to match with the list of features from a contorl image

The find_lines_black.cpp file runs the same algorithm on a control image (black.JPG) me wearing a Leftovers t-shirt. This does the same thing as above for the Leftovers shirt, finding features in the same way. There is some manual tinkering of feature point locations here to ensure perfect features are found. 

The match_shirts.cpp file takes the input image and the control image and warps the control image to line up with the input image, using the two feature point lists generated above, and a method called homography matching. It then takes the warped Lefotvers shirt image, and blends it with the input image, which shows how well the two shirts line up.

In main.cpp, there is just a list of calls to the the methods created in the three above files. Uncomment the file call that you wish to execute, rebuild the project and then run the executable to run the file. There should not be a need to run find_lines_black() since the feature points have already been found on the black shirt, and it is a control image.

## Misc Info

See above on how to run the program so it executes the find_lines() method. To run the match_shirts() method, uncomment it in main.cpp, build, then run ./OpenCVInstallGood <inputimage> black.JPG. This will save an image, blended.png, to your directory that shows the warped image blended with the input image.

I have run this process on a test image, white.JPG, and have included all outputs. 

In order to maximize the probability the algorithm will work, take a picture as similar as possible to the white.JPG image provided. That is to say, take a picture of the waist up, wearing a plain white shirt sleeve t-shirt, with your arms out at a slight angle. 


















