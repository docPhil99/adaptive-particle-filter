//
//  main.cpp
//  WH_tracker
//
//  Created by Phil Birch on 23/06/2014.
//  Copyright (c) 2014 Phil Birch. All rights reserved.
//

#include <iostream>
#include "WHT.h"


_particles* initialize(CvCapture** capture,gsl_rng** rng)
{
	printf("INITIALIZING TRACKING DATABASE\n\n");
    
	//initialize particles
	_particles* database = (_particles*)malloc( Nmax * sizeof( particles ) );
	
	printf("CONNECTING TO CAMERA(s)\n\n");
	//initialize Captures
	*capture = cvCaptureFromFile("http://139.184.100.71/mjpg/video.mjpg");
	
	gsl_rng_env_setup();
	*rng=gsl_rng_alloc(gsl_rng_mt19937);
    
    
	return database;
}

int main(int argc, const char * argv[])
{

    CvCapture* capture = NULL;
	gsl_rng* rng;
	IplImage* img = NULL;
	_particles* database= initialize(&capture,&rng);
	_track* track =(_track*)malloc( 1 * sizeof( t ) );
	gsl_rng_set( rng, time(NULL) );
	//selectObject(capture, track, false);
	bool firstRun = true;
	int key = 0;
	while(1)
	{
		if(track->selection==true)
		{
			img = grabFrame(track, img);
			int x = processFrame(img, track, database, firstRun, rng);
			firstRun = false;
		}
		else
		{
			selectObject(capture,track,false);
		}
        
		key=cvWaitKey(10);
		if(key == 32)
			break;
		else if(key == 27)
		{
			printf("\n\nRESET\n");
			cvDestroyWindow("Particles");
			free(database);
			free(track);
			_particles* database = (_particles*)malloc( Nmax * sizeof( particles ) );
			_track* track =(_track*)malloc( 1 * sizeof( t ) );
			firstRun = true;
			track->selection = false;
		}
		
	}
    return 0;
}

