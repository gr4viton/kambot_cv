/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   robot_cv.cpp
 * Author: Daniel Davidek
 *
 * Created on 23. listopadu 2016, 14:14
 */
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// includes
#include "robot_cv.hpp"


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// global variables
raspicam::RaspiCam_Cv Camera; // raspi camera class object instance
Mat prev_output; // output of the previous function
Mat streamed_image; // image which is written to the disk, to stream it
string stream_image_path = "/home/pi/stream/out.jpg"; // where to save out.jpg

int frame_num = 0; // frame counter
int jpg_quality = 90; // quality for file saving - streaming = 95 is default
double line_position = 0; // output of the computer vision alogirthm <-1; 1>


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function declarations
// in robot_cv.hpp

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// function definitions



// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// raspicam functions
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

string get_image_type_string(Mat & image)
{
    int type_i = image.type();
    if (type_i == 0)
        return "CV_8U";
    return (
        (type_i && CV_8UC1) ? "CV_8UC1" : (
        (type_i && CV_8UC3) ? "CV_8UC3" : (
        (type_i && CV_8UC4) ? "CV_8UC4" : (
                            "other" ))));
}

void set_raspicam_param(int param_index, double param_value, string param_name)
{
    cout<<"Setting "<<param_value<<"\tas \t["<<param_name<<"] ... ";
    Camera.set(param_index, param_value);
    cout<<"Done!"<<endl;
}

int open_raspicam()
{
    cout<<"Opening Camera ...";
    if (!Camera.open()) {
        cerr<<"Error opening the camera"<<endl;
        return -1;
    }
    else
        cout<<"Done!"<<endl;
}

int init_raspicam(unsigned x=320, unsigned y=240, unsigned fps=30, int format=CV_8U)
// CV_8U = grayscale unsigned 8bit = byte 
{
    Camera.set( CV_CAP_PROP_FORMAT, format );    
    int ret = open_raspicam();
    set_raspicam_param(CV_CAP_PROP_FRAME_WIDTH, (double)x, "camera image width");
    set_raspicam_param(CV_CAP_PROP_FRAME_HEIGHT, (double)y, "camera image height");
    set_raspicam_param(CV_CAP_PROP_FPS, (double)fps, "camera fps");
    return ret;
}

int set_raspicam_format(int format){
    cout<<"Releasing Camera to change parameters..."<<endl;
    Camera.release();
    cout<<"Seting Camera format..."<<endl;
    Camera.set( CV_CAP_PROP_FORMAT, format);    
    open_raspicam();
    return 0;
}

Mat grab_frame()
{
    Mat frame;

    Camera.grab();
    Camera.retrieve(frame);

    if(frame.empty())
        cout<<"Retrieved empty frame! ["<<frame_num<<"]"<<endl;
    else
        cout<<"\r Retrieved image ["<<frame_num<<"]! ("<<
                frame.cols<<"x"<<frame.rows<<"), "<<
                get_image_type_string(frame)<<" >> "<<flush;
    
    frame_num++;
    return frame;
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// OpenCV wrapper functions
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

Mat make_gray(Mat im)
{
    // if not grayscale - convert it
    if(!(im.type() && CV_8U))
    {
        cout<<">> It's not grayscale - going to convert it to grayscale!"<<endl;
        cvtColor(im, im, COLOR_RGB2GRAY);
    }
    return im;
}

Mat make_color(Mat im)
{
    if(!(im.type() && CV_8UC3))
        cvtColor(im, im, COLOR_GRAY2RGB);
    return im;
}

void rename_it(string filename_jpg)
{
    string filename_mjpg = filename_jpg;
    filename_mjpg.insert(filename_mjpg.length() - 3, "m");
    int rc = rename(filename_jpg.c_str(), filename_mjpg.c_str());
    if (rc)
    {
        cout<<"\nCould not rename ["<<filename_jpg<<"] to ["<<filename_mjpg<<"] !"<<endl;
        cerr<<"Return code ["<<rc<<"]renaming jpg to mjpg"<<endl;
    }
}

int write_image(string filename_jpg, Mat & output, int jpg_quality=95 ) // reference = pointer wihtout dereferencing
{    
    vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
    compression_params.push_back(jpg_quality);

    //write the image to file
    bool bSuccess = 1;
    if(jpg_quality==95)
        bSuccess = imwrite(filename_jpg, output);
    else
    {
/*        if (jpg_quality_param[1]!=jpg_quality)
            jpg_quality_param[1] = jpg_quality;*/
        bSuccess = imwrite(filename_jpg, output, compression_params);
    }
    if ( !bSuccess )
    {
         cout << "ERROR : Failed to save the image" << endl;
         return 1;
    }
    rename_it(filename_jpg); // rename to .mjpg
    return 0;
}


Mat get_x_edges(Mat img_gray)
// the input and return arguments are not optimal as it is passed by value
// this copies the image everytime
// you should use pointers (or C++ references) to pass the value by refference
// this would only copy the address (pointer) to the image = faster for processing
{
    //cout<<">> Going to find edges by Sobel filter!"<<endl;
    int x_order = 1;
    int y_order = 0;

    // default values        
    int scale = 1; 
    int delta = 0; 
    int ddepth = CV_16S;
    int conv_core_size = 3;

    Mat grad_x;

    Sobel( img_gray, grad_x, ddepth, x_order, y_order, conv_core_size, scale, delta, BORDER_DEFAULT ); 

    // Scharr is almost similar to Sobel filter with convolution core size 3,
    // which has to some point better performance (for the core size 3)
    //Scharr( img_gray, grad_x, ddepth, x_order, y_order, scale, delta, BORDER_DEFAULT ); 

    return grad_x;
}

Mat make_absolute(Mat image)
{
    // not functional
    cout<<">> Going to make absolute image!"<<endl;
    Mat abs_image = image.clone();
    Mat _image = image.clone();

    try
    {
        convertScaleAbs( _image, abs_image, 0, 255 );
    }
    catch(const std::exception& e)
    {
        cout << e.what();
    }
    //abs_image = abs(image);
    // abs_image would be in CV_8U - grayscale 8-bit unsigned integer matrix
}


Mat get_show_edge_image(Mat signed_edges)
{
    // detected edges are signed 
    // to show them one possibility is this
    // gray is for not 
    // one must know that for the image with only kind of edges (positive or negative)
    // you don't get the gray background, as you are always calculating max and min.
    Mat show_edges;
    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(signed_edges, &minVal, &maxVal, &minLoc, &maxLoc);

    signed_edges.convertTo(show_edges, CV_8U, 255.0/(maxVal - minVal), -minVal * 255.0/(maxVal - minVal));
    return show_edges;
}

Mat get_blend_images(Mat im1, Mat im2, double a=0.5)
{
    double alpha = a;
    double beta = ( 1.0 - alpha );

    Mat both;
    addWeighted(im1, alpha, im2, beta, 0.0, both);
    return both;
}

Mat add_label(Mat image, char const* text_string = "42", double text_offset = 20)
{
    double scale = 0.5;
    int thick = 1;
    Point left_bot_text_corner(1, text_offset/2);
    Scalar wht = CV_RGB(255,255,255);
    putText(image, text_string, left_bot_text_corner, 
            FONT_HERSHEY_COMPLEX_SMALL, scale, wht, 
            thick, CV_AA);
    return image;
}

Mat join_images_updown(Mat im_bottom, Mat im_top, char const* text_new_top = "")
{
    // joins two images of same width
    // they must have same width!
    // adds text to the upper left corner    
    Size sz_top = im_top.size();
    if(im_bottom.empty())
        im_bottom = Mat::zeros(1, sz_top.width, CV_8UC3); // if it is the first image 
    Size sz_bot = im_bottom.size();
    int text_offset = 20;
    // whole image
    Mat both(sz_top.height + sz_bot.height+ text_offset, sz_bot.width, CV_8UC3);
    
    // place for text label
    Mat zeros = Mat::zeros(text_offset, sz_top.width, CV_8UC3);
    zeros.copyTo(both(Rect(0,0, sz_top.width, text_offset)));
    
    // individual images
    make_color(im_top).copyTo(both(Rect(0, 0 + text_offset, sz_top.width, sz_top.height)));
    make_color(im_bottom).copyTo(both(Rect(0, sz_top.height + text_offset, sz_bot.width, sz_bot.height)));
    
    both = add_label(both, text_new_top, text_offset);
    return both;
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// robot computer vision logic functions
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


int cv_loop()
{
    // init cam
    //set_raspicam_format(CV_8UC3); // colored
    set_raspicam_format(CV_8U); // grayscale
    while(1)
    {
        // read..
        Mat frame = grab_frame();
        prev_output = frame.clone();
        Mat empty;
        Mat image_queue = join_images_updown(empty, prev_output, "original");
        
        
        // ____________________________________________________
	// Region of interest (ROI) = get subimage
        // roi(x, y, w, h);
        unsigned roi_height = 50;
        int x_pos = 0;
        int y_pos = 50;
        Rect roi(x_pos, y_pos, prev_output.cols, roi_height);
        Mat img_roi = prev_output(roi);
        
        prev_output = img_roi;
        
        /*string str;
        snprintf(str, "ROI(x, y, w, h) = %i, %i, %i, %i", x_pos, y_pos, prev_output.cols, roi.height);
        image_queue = join_images_updown(image_queue, prev_output, str.c_str());*/
        image_queue = join_images_updown(image_queue, prev_output, "ROI");
        
        // ____________________________________________________
        // threshold
        Mat thresh;
        threshold(prev_output, thresh, 0, 255, THRESH_BINARY_INV|THRESH_OTSU);
        prev_output = thresh;
        image_queue = join_images_updown(image_queue, prev_output, "thresh");
        
        // ____________________________________________________
        // detect edges
        Mat signed_edges = get_x_edges(prev_output);
        prev_output = signed_edges; 
        
        // the signed edges should not be visualised directly, ergo call 
        Mat show_edges = get_show_edge_image(signed_edges);
        
        image_queue = join_images_updown(image_queue, show_edges, "show edges"); // prev=show_edges
        
        
        // ____________________________________________________
        // get the pixel positions of left and right edge of the followed black-line 
        double minVal, maxVal;
        // http://docs.opencv.org/3.1.0/db/d4e/classcv_1_1Point__.html
        // top & bottom line points x-coordinates
        // (as we are in the top_line_roi we don't get y-coordinates)
        Point top_minLoc, top_maxLoc;
        Point bottom_minLoc, bottom_maxLoc;
        
        Rect top_line_roi(0, 0, prev_output.cols, 1);
        Rect bottom_line_roi(0, roi_height-1, prev_output.cols, 1);
        
        minMaxLoc(signed_edges(top_line_roi), &minVal, &maxVal, &top_minLoc, &top_maxLoc);
        minMaxLoc(signed_edges(bottom_line_roi), &minVal, &maxVal, &bottom_minLoc, &bottom_maxLoc);

        // ____________________________________________________
        // draw the lines
        Mat show_edges_color;
        cvtColor(show_edges, show_edges_color, COLOR_GRAY2RGB);
        
        // change the y-coordinates of the bottom locations (as they are 0 now)
        Point y_offset(0, roi_height-1);
        bottom_minLoc += y_offset;
        bottom_maxLoc += y_offset;
        
        Scalar color_left_line = CV_RGB(255, 0, 0);
        Scalar color_right_line = CV_RGB(0, 255, 0); 
        
        line(show_edges_color, top_minLoc, bottom_minLoc, color_left_line, 1, CV_AA);       
        line(show_edges_color, top_maxLoc, bottom_maxLoc, color_right_line, 1, CV_AA);
        
        image_queue = join_images_updown(image_queue, show_edges_color, "2 lines");
        
        // ____________________________________________________
        // get the center line 
        Point top_center = (top_minLoc + top_maxLoc)/2;
        Point bottom_center = (bottom_minLoc + bottom_maxLoc)/2;
        
        Scalar color_center_line = CV_RGB(0, 0, 255); 
        
        line(show_edges_color, top_center, bottom_center, color_center_line, 1, CV_AA); 
        
        image_queue = join_images_updown(image_queue, show_edges_color, "center lines");
        // ____________________________________________________
        // get the center point y-coordinate
        unsigned radius = 3;
        Point center = (top_center + bottom_center) / 2;
        circle(show_edges_color, center, radius, color_center_line, 1, CV_AA);
        
        double center_x = (double)center.x;
        double width = (double)prev_output.cols-1;
        line_position =  (2*center_x / width) -1;
        
        image_queue = join_images_updown(image_queue, show_edges_color, "center point");
        // ____________________________________________________
        // add text with line_position value
        Mat show_edges_color_w_text = show_edges_color.clone();
        
        char text_string[100];
        sprintf(text_string, "-1<%.2lf<+1", line_position);
        
        double scale = 0.5;
        int thick = 1;
        putText(show_edges_color_w_text, text_string, center, 
                FONT_HERSHEY_COMPLEX_SMALL, scale, color_center_line, 
                thick, CV_AA);
        
        image_queue = join_images_updown(image_queue, show_edges_color_w_text, "line_pos_text");
        
        // ____________________________________________________
        // set the streamed image
        streamed_image = image_queue;
        
        // ____________________________________________________
	// write file on disk to be able to stream it
        write_image(stream_image_path, streamed_image, jpg_quality);
        
        // share variables with main thread
        
    }
    return 0;
}

int start_cv_loop_thread()
{
    // possible to add thread creation
    cv_loop();
    return 0;
}

int init_cv()
{
    // init camera input via raspicam library and try to grab images
    return init_raspicam(320, 240, CV_8UC3);
}

int start_cv()
{
    //start_mjpg_streamer(); // possible to start bash and mjpg_streamer?
    start_cv_loop_thread(); 
    return 0;
}
