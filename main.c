/* 
 * File:   main.c
 * Author: Daniel Davidek
 *
 * Created on 9. listopadu 2016, 14:44
 */

// removed from project config
// -lwiringPi
// and -I dir ../../GIT/RaspBot/src/RaspberryPi/shared

#include <stdio.h>
#include <stdlib.h>
//#include <wiringPi.h>
#include <pthread.h>

// i2c
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h> // ioctl()
#include <fcntl.h>
#include <linux/i2c-dev.h>

#include <unistd.h> // write()

#include "robot_cv.h"
    
    
/*
 * 
 */
int main(int argc, char** argv) {
    
    int ret;
    //pthread_t thread ;
    
    // ____________________________________________________
    // robot_cv setup
    
    // This function needs to be called only once
    ret = init_cv();
    if (ret != 0)
    {
        printf("Error during initialization of camera! Exiting main program!");
        return -1;
    }
    
    // ____________________________________________________
    // other robot setup
    // .. motors, i2c ..
    
    // ____________________________________________________
    // robot_cv start
    // This funciton starts the loop of the computer vision algorithm.
    // It is possible to rewrite it to be called in a separate thread.
    start_cv();

    /*
    // Create threads
    
    if(pthread_create(&thread, NULL, &ThI2CRefresh, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    */
    // Infinity loop            
    for (;;)
    {
        printf("\rPrint from main thread: line_position=%lf", line_position);
    }
    
    return (EXIT_SUCCESS);
}


