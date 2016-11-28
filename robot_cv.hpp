/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   robot_cv.hpp
 * Author: Daniel Davidek
 *
 * Created on 23. listopadu 2016, 14:14
 */

#ifndef ROBOT_CV_HPP
#define ROBOT_CV_HPP

//#include <opencv2/opencv.hpp>
//#include <raspicam/raspicam_cv.h>
//#include <raspicam/raspicam.h>
#include <cstdlib>

#include <iostream>
#include <cstring>
#include <string>

#include <iostream>
#include <fstream>


#include <time.h>
#include <limits.h>

#include <vector>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <raspicam/raspicam_cv.h>

using namespace cv;
using namespace std;
  

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// raspicam functions
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

string get_image_type_string(Mat & image);
void set_raspicam_param(int param_index, double param_value, string param_name);
int open_raspicam();
int init_raspicam(unsigned x, unsigned y, unsigned fps, int format);
int set_raspicam_format(int format);
Mat grab_frame();

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// OpenCV wrapper functions
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Mat make_gray(Mat im);
Mat make_color(Mat im);
int write_image(string NAME, Mat & output, int jpg_quality ); // [&] = reference = pointer wihtout dereferencing [*]
void rename_it(string filename_jpg);
Mat get_x_edges(Mat img_gray);
Mat make_absolute(Mat image);
Mat get_show_edge_image(Mat signed_edges);
Mat get_blend_images(Mat im1, Mat im2, double a);
Mat add_label(Mat image, char const* text_string, double text_offset );
Mat join_images_updown(Mat im_bottom, Mat im_top, char const* text_new_top);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// robot computer vision logic functions
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int cv_loop();
int start_cv_loop_thread();

int start_mjpg_streamer();
int start_cv_loop_thread();

#ifdef __cplusplus
extern "C" {
#endif

    int init_cv();
    int start_cv();

#ifdef __cplusplus
}
#endif

#endif /* ROBOT_CV_HPP */


