#include "WHT.h"
#include "ParticleFilter.h"

 

int processFrame(IplImage* img, _track* track, _particles* database, bool firstrun, gsl_rng* rng)
{
	//convert RGB to HSV
	IplImage* HSVImg = cvCreateImage( cvGetSize(img), img->depth, 3 );
	cvCvtColor(img,HSVImg,CV_BGR2HSV);
	



	if(firstrun==true)
	{
		printf("\nTRACKING...");
		track->refHistogram = calculateRefHistogram(HSVImg, track);

		//initialize particles database
		for(int i=0;i<track->activeparticles;i++)
		{
			database[i].x = track->x;
			database[i].y = track->y;
			database[i].weight = 0.01;
			database[i].active = true;
		}
	}
	else
	{
		//particle filter steps
		predict(database, track, img, rng);
		update(database, track, HSVImg);
		resample(database, track);
	}

	displayParticles(database, track, img);

	cvReleaseImage(&HSVImg);
	return 0;
}


