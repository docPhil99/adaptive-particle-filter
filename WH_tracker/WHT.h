//
//  WHT.h
//  WH_tracker
//
//  Created by Phil Birch on 23/06/2014.
//  Copyright (c) 2014 Phil Birch. All rights reserved.
//

#ifndef WH_tracker_WHT_h
#define WH_tracker_WHT_h

//#include <opencv2/core/utility.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <opencv2/features2d/features2d.hpp>
#include "opencv2/legacy/legacy.hpp"
#include "gsl/gsl_rng.h"
#include "gsl/gsl_randist.h"

using namespace cv;
using namespace std;

#define Nmax 250							//maximum number of Particles
#define Nmin 50								//minimum number of Patticles


#define N 3									//number of cameras



typedef struct _histogram
{
	CvHistogram* hue;						//hue histogram
	CvHistogram* sat;						//saturation histogram
	CvHistogram* val;						//value histogram
}histogram;


typedef struct _particles
{
	bool active;
	int x;
	int y;
	double distance;
	double weight;
	
}particles;


typedef struct _camera
{
	int id;
	CvCapture* capture;
	
}camera;


typedef struct _track
{
	int x;
	int y;
	CvCapture* capture;
	bool selection;
	_histogram* refHistogram;
	int activeparticles;
	int scale;
}t;




//void add(int,int);
void selectObject(CvCapture*,_track*,bool);
void updateCameraInfo(_track*);
IplImage* grabFrame(_track*,IplImage*);
int processFrame(IplImage*, _track*, _particles*, bool, gsl_rng*);

#endif
