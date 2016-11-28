/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   robot_cv.h
 * Author: Daniel Davidek
 *
 * Created on 25. listopadu 2016, 14:30
 */

#ifndef ROBOT_CV_H
#define ROBOT_CV_H

#ifdef __cplusplus
extern "C" {
#endif

// these functions are needed from main.c
// main is compiled as normal C (gcc), 
// but robot_cv.cpp is compiled as C++ (gpp), because of OpenCV
// but we still want to call these functions from the main.c
int init_cv();
int start_cv();
extern double line_position; // defined in robot_cv.cpp


#ifdef __cplusplus
}
#endif

#endif /* ROBOT_CV_H */

