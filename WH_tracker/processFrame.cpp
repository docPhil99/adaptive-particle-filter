#include "WHT.h"
#include "ParticleFilter.h"

 

int processFrame(InputArray Mimg, _track* track, _particles* database, bool firstrun, gsl_rng* rng)
{
	//convert RGB to HSV
    Mat HSV;
	cvtColor(Mimg, HSV, CV_BGR2HSV);
    imshow("HSV", HSV);
    IplImage HSVIpl=HSV;
    Mat MMimg=Mimg.getMat();
    IplImage img=MMimg;

	if(firstrun==true)
	{
		printf("\nTRACKING...");
		track->refHistogram = calculateRefHistogram(&HSVIpl, track);

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
		predict(database, track, &img, rng);
        update(database, track, &HSVIpl);
		resample(database, track);
	}

	displayParticles(database, track, &img);
	

    
 return 0;
}


