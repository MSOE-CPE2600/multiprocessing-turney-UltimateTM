/**********************************************
*  Filename: jpegrw.c
*  Description: Modified program to create Mandelbrot images.
*  Author: Gabe Limberg
*  Date: 11/17/2025
***********************************************/
//  mandel.c
//  Based on example code found here:
//  https://users.cs.fiu.edu/~cpoellab/teaching/cop4610_fall22/project3.html
//
//  Converted to use jpg instead of BMP and other minor changes
//  
///
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h> // for command line argument parsing
#include <sys/wait.h> // for using wait()
#include <pthread.h>
#include "thread_par.h"
#include "jpegrw.h"	
#define MANDEL_AMOUNT 50 // amount of mandelbrot images to create

// local routines
static int iteration_to_color( int i, int max );
static int iterations_at_point( double x, double y, int max );
static void compute_image( imgRawImage *img, double xmin, double xmax,
									double ymin, double ymax, int max, int num_threads );
static void show_help();
void *compute_image_thread(void *arguments) { // Thread function to compute part of the Mandelbrot image
	thread_par *args = (thread_par *)arguments; // cast argument to thread_par pointer
	imgRawImage *img = args->img; // get image pointer
	double xmin = args->xmin;
	double xmax = args->xmax;
	double ymin = args->ymin;
	double ymax = args->ymax;
	int max = args->max;
	int width = args->width;
	int height = args->height;
	int tid = args->tid;
	int num_threads = args->num_threads;

	for (int j = 0; j < height; j++) {
		for (int i = tid; i < width; i += num_threads) {
			// Determine the point in x,y space for that pixel.
			double x = xmin + i * (xmax - xmin) / width;
			double y = ymin + j * (ymax - ymin) / height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x, y, max);

			// Set the pixel in the bitmap.
			setPixelCOLOR(img, i, j, iteration_to_color(iters, max));
		}
	}

	pthread_exit(NULL);
}

int main( int argc, char *argv[] ){
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.
	//const char *outfile = "mandel.jpg"; not using due to multiple files
	double xcenter = 0;
	double ycenter = 0;
	double xscale = 4;
	double yscale = 0; // calc later
	int    image_width = 1000;
	int    image_height = 1000;
	int    max = 1000;
	int MAX_PROC = 12; // default amount of processes if no argument presented in terminal
	int num_threads = 4; // default number of threads for each process

	// For each command line argument given,
	// override the appropriate configuration value.
	//c = getopt(argc,argv,"x:y:s:W:H:m:o:h"
	while((c = getopt(argc,argv,"x:y:s:W:H:m:h:p:"))!=-1) {
		switch(c) 
		{
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				xscale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			//case 'o':
				//outfile = optarg;
				//break;
			case 'h':
				show_help();
				exit(1);
				break;
			case 'p':
				MAX_PROC = atoi(optarg); // set max processes
				break;
			case 't':
				num_threads = atoi(optarg); // set number of threads per process
				break;

		}
	}

	int act_proc; // not really needed in batch mode, but we can keep it
	int i = 0;        // frame index

	while (i < MANDEL_AMOUNT) {

		act_proc = 0; // active processes, creates n batches based on MAX_PROC

		while (act_proc < MAX_PROC && i < MANDEL_AMOUNT) {

			xscale *= 0.9; // zoom in each iteration
			yscale = xscale / image_width * image_height;
			xcenter = -0.61; // pan to interesting area
			ycenter = -0.60; // interesting area 

			char filename[256];
			sprintf(filename, "mandel%d.jpg", i);

			int pid = fork(); // create new process

			if (pid == 0) { // Child process

				// Create a raw image of the appropriate size.
				imgRawImage* img = initRawImage(image_width, image_height);

				// Fill it with a black
				setImageCOLOR(img, 0);

				// Compute the Mandelbrot image
				compute_image(img, xcenter - xscale / 2, xcenter + xscale / 2, ycenter - yscale / 2,
				ycenter + yscale / 2, max, num_threads);

				// Save the image in the stated file.
				storeJpegImageFile(img, filename);

				// free the mallocs
				freeRawImage(img);

				exit(0);
			} else if (pid > 0) {
				// Parent process
				act_proc++;
				printf("mandel: x=%lf y=%lf xscale=%lf yscale=%1f max=%d outfile=%s\n",
					xcenter, ycenter, xscale, yscale, max, filename);
				fflush(stdout);

				i++; // move to next frame index
			}
		}

		while (act_proc > 0) {
			wait(NULL);
			act_proc--;
		}
	}

	return 0;
}





/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iter;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image(imgRawImage* img, double xmin, double xmax, double ymin, double ymax, int max, int num_threads )
{
	int i,j;

	int width = img->width;
	int height = img->height;

	// For every pixel in the image...
	if (num_threads <= 1) {
		for(j=0;j<height;j++) {

			for(i=0;i<width;i++) {

				// Determine the point in x,y space for that pixel.
				double x = xmin + i*(xmax-xmin)/width;
				double y = ymin + j*(ymax-ymin)/height;

				// Compute the iterations at that point.
				int iters = iterations_at_point(x,y,max);

				// Set the pixel in the bitmap.
				setPixelCOLOR(img,i,j,iteration_to_color(iters,max));
			}
		}
	} else {
		pthread_t *threads = malloc(num_threads * sizeof(pthread_t)); // array of threads
		thread_par *thread_args = malloc(num_threads * sizeof(thread_par)); /// array of thread arguments
		
		for (int t = 0; t < num_threads; t++) {
			thread_args[t].img   = img;
			thread_args[t].xmin  = xmin;
			thread_args[t].xmax  = xmax;
			thread_args[t].ymin  = ymin;
			thread_args[t].ymax  = ymax;
			thread_args[t].max   = max;
			thread_args[t].width = width;
			thread_args[t].height = height;
			thread_args[t].tid   = t;
			thread_args[t].num_threads = num_threads;

			pthread_create(&threads[t], NULL, compute_image_thread, &thread_args[t]); // create thread
		}

		// Wait for all threads
		for (int t = 0; t < num_threads; t++) {
			pthread_join(threads[t], NULL); // wait for thread to finish
		}

		free(threads); // free thread array
		free(thread_args);
	}

}

/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max )
{
	int color = 0xFFFFFF*iters/(double)max;
	return color;
}


//Show help message
void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates (X-axis). (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=1000)\n");
	printf("-H <pixels> Height of the image in pixels. (default=1000)\n");
	//printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}
