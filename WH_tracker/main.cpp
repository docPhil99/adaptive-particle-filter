//
//  main.cpp
//  WH_tracker
//
//  Created by Phil Birch on 23/06/2014.
//  Copyright (c) 2014 Phil Birch. All rights reserved.
//

#include <iostream>
#include "WHT.h"


int main(int argc, const char * argv[])
{

    //load a video
     VideoCapture cap("/Volumes/LocalDataHD/tafj0/Documents/MATLAB/Videos/pets1.avi");
    
    //skip some frames
    for(int p=0;p<40;p++)
        cap.grab();
    
    
	gsl_rng* rng;
    Mat img;
    //setup random number generator
    gsl_rng_env_setup();
	rng=gsl_rng_alloc(gsl_rng_mt19937);
    gsl_rng_set( rng, time(NULL) );
	
    //malloc particle database
    _particles* database = (_particles*)malloc( Nmax * sizeof( particles ) );
	
	//malloc track
	_track* track =(_track*)malloc( 1 * sizeof( t ) );
    track->selection=false;
    
	bool firstRun = true;
	int key = 0;
    long frame=0;
	while(1)  //main loop
	{
        cout<<frame<<"\n";
        frame++;
		if(track->selection==true) //an object has been selected by the user, track it
		{
			cap>>img;
          	int x = processFrame(img, track, database, firstRun, rng);
			firstRun = false;
		}
		else   //no object has been selected so select on
		{
            cap>>img;
			selectObject(img,track,false);
		}
        
		key=cvWaitKey(10);
		if(key == 32)   //press space key
			break;
		else if(key == 114)   //press 'r'r key to reset track
		{
			cout<<"\n\nRESET\n";
            cout.flush();
			cvDestroyWindow("Particles");
			free(database);  // this releasing of track and database causes a memory hole and it needs to be removed.
			free(track);
			database = (_particles*)malloc( Nmax * sizeof( particles ) );
			track =(_track*)malloc( 1 * sizeof( t ) );
			firstRun = true;
			track->selection = false;
		}
		
	}
    destroyAllWindows();
    return 0;
}

