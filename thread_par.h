/**********************************************
*  Filename: thread_par.h
*  Description: Header file for thread arguments.
*  Author: Gabe Limberg
*  Date: 11/25/2025
***********************************************/
#include "jpegrw.h"	
#ifndef THREAD_ARGS_H
#define THREAD_ARGS_H

typedef struct {
    imgRawImage *img;
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    int max;
    int width;
    int height;
    int tid;
    int num_threads;
} thread_par;

#endif
