#include "WHT.h"
#include "ParticleFilter.h"



void calculateHistogram(IplImage* img, _histogram* histogram)
{
	IplImage* channel = cvCreateImage( cvGetSize(img), 8, 1 );
	
	int hist_size = 256;       
	float range[]={0,256};
	float* ranges[] = { range };
	
	cvSetImageCOI(img,3);
	cvCopy(img,channel);
	cvSetImageCOI(img,0);

	cvCalcHist( &channel, histogram->hue, 0, NULL );
	cvNormalizeHist(histogram->hue,1);

	cvSetImageCOI(img,2);
	cvCopy(img,channel);
	cvSetImageCOI(img,0);
		 
	cvCalcHist( &channel, histogram->sat, 0, NULL );
	cvNormalizeHist(histogram->sat,1);

	cvSetImageCOI(img,1);
	cvCopy(img,channel);
	cvSetImageCOI(img,0);
		 
	cvCalcHist( &channel, histogram->val, 0, NULL );
	cvNormalizeHist(histogram->val,1);

	cvReleaseImage(&channel);		
}

_histogram* calculateRefHistogram(IplImage* HSVImg, _track* track)
{
	//select area covered by the object 20x20 pixels
	CvRect selectionRegion = cvRect(track->x-10,track->y-10,20,20);
	cvSetImageROI(HSVImg, selectionRegion);

	int hist_size = 256;      
	float range[]={0,256};
	float* ranges[] = { range };

	CvHistogram *h=cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

	_histogram* histogram = (_histogram*)malloc( 3 * sizeof( h ) );

	histogram->hue = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	histogram->sat = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	histogram->val = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	
	calculateHistogram(HSVImg,histogram);

	cvReleaseHist(&h);

	return histogram;
}

_histogram* calculateCurrentHistogram(IplImage* HSVImg)
{

	int hist_size = 256;      
	float range[]={0,256};
	float* ranges[] = { range };

	CvHistogram *h=cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);

	_histogram* histogram = (_histogram*)malloc( 3 * sizeof( h ) );

	histogram->hue = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	histogram->sat = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	histogram->val = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
	
	calculateHistogram(HSVImg,histogram);

	cvReleaseHist(&h);

	return histogram;
}

void predict(_particles* database, _track* track, IplImage* image, gsl_rng* rng)
{
	for(int i=0;i<Nmax;i++)
	{
		int x = gsl_ran_gaussian( rng, track->scale ) + database[i].x;
		int y = gsl_ran_gaussian( rng, track->scale ) + database[i].y;
						
		if(x>=image->width)
			x = image->width-20;
		if(x<=10)
			x = 11;
		if(y>=image->height)
			y = image->height-20;	
		if(y<=10)
			y = 11;

		database[i].x = x;
		database[i].y = y;
	}
}

void update(_particles* database, _track* track, IplImage* HSVImg)
{

	double totalWeight = 0, hueDistance = 0, satDistance = 0, valDistance = 0, likelihood = 0;

	//loop through all the particles
	for(int i=0;i<Nmax;i++)
	{
		//selection only those particles which are active
		if(database[i].active == true)
		{
			//select area covered around the particle 20x20 pixels
			CvRect selectionRegion = cvRect(database[i].x-10,database[i].y-10,20,20);
			cvSetImageROI(HSVImg, selectionRegion);
	
			//calculate current histogram for 'i'th particle
			_histogram* h = calculateCurrentHistogram(HSVImg);

			hueDistance = cvCompareHist(h->hue,track->refHistogram->hue,CV_COMP_BHATTACHARYYA);
			satDistance = cvCompareHist(h->sat,track->refHistogram->sat,CV_COMP_BHATTACHARYYA);
			valDistance = cvCompareHist(h->val,track->refHistogram->val,CV_COMP_BHATTACHARYYA);

			cvReleaseHist(&h->hue);
			cvReleaseHist(&h->sat);
			cvReleaseHist(&h->val);
			free(h);

			database[i].distance = 1 - ((hueDistance + satDistance + valDistance)/3);

			likelihood = cte2_color * exp((database[i].distance-1)*cte1_color);	

			database[i].weight = database[i].weight * likelihood;

			totalWeight = totalWeight + database[i].weight;
			
			cvResetImageROI(HSVImg);
		}
	}
	for(int i=0;i<Nmax;i++)
	{
		//selection only those particles which are active
		if(database[i].active == true)
		{
			database[i].weight = database[i].weight/totalWeight;
			
		}
	}
	
}



void resample(_particles* database, _track* track)
{
	//cout<<"Resample"<<"\n";
    int x = 0;
	int y = 0;
	//loop through all the particles
	for(int i=0;i<Nmax;i++)
	{
		//selection only those particles which are active
		if(database[i].active == true)
		{
			x = x + database[i].x;
			y = y + database[i].y;
		}
	}
	
	track->x = x/track->activeparticles;
	track->y = y/track->activeparticles;

	double squared_weight = 0;
	for(int i=0;i<Nmax;i++)
	{
		//selection only those particles which are active
		if(database[i].active == true)
		{
			squared_weight = squared_weight + (database[i].weight*database[i].weight);
		}
	}

	double N_threshold = 6*track->activeparticles/10;

	if(1/squared_weight<N_threshold)
	{
		track->activeparticles = 50;

		_particles* new_particles = (_particles*)malloc( Nmax * sizeof( particles ) );
		
		resampleparticles(database,new_particles,Nmax);

		for(int i=0;i<Nmax;i++)
		{
			database[i] = new_particles[i];
		}

		free(new_particles);

		int counter = 0;
		for(int i=0;i<Nmax;i++)
		{
			counter = counter + 1;
			if(counter < track->activeparticles)
				database[i].active = true;
			else
				database[i].active = false;

			database[i].weight = 0.01;
		}
		track->scale = 20;
		
	}
	else
	{
		if(track->activeparticles<Nmax)
			track->activeparticles = track->activeparticles + 20;
		if(track->activeparticles>Nmax)
			track->activeparticles = Nmax;

		int counter = 0;
		for(int i=0;i<Nmax;i++)
		{
			database[i].weight = 0.01;
			counter = counter + 1;
			if(counter < track->activeparticles)
				database[i].active = true;
			else
				database[i].active = false;
		}

		track->scale = track->scale + 5;
		
	}

	qsort( database, Nmax, sizeof( *database ), &particle_cmp );
	
	
}


int particle_cmp( const void* p1, const  void* p2 )
{
	_particles* _p1 = (_particles*)p1;
	_particles* _p2 = (_particles*)p2;

	if( _p1->weight > _p2->weight )
		return -1;
	if( _p1->weight < _p2->weight )
	    return 1;
	return 0;
}


void resampleparticles( _particles* database, _particles* new_particles, int n )
{
	int i, j, np, k = 0;
	
	qsort( database, n, sizeof( *database ), &particle_cmp );
	
	


	for( i = 0; i < n; i++ )
	{
		np = cvRound( database[i].weight * n );
		
		for( j = 0; j < np; j++ )
		{
			new_particles[k++] = database[i];
		
			if( k == n )
				goto exit;
		}
    }

	while( k < n )
		new_particles[k++] = database[0];

	exit:
		k = n;
}

void displayParticles(_particles* database, _track* track, IplImage* img)
{
	int counter = 0;
	for(int i=0;i<Nmax;i++)
	{
		if(database[i].active == true)
		{
			counter++;
			cvCircle(img, cvPoint(database[i].x,database[i].y), 3, cvScalar(255,0,0), 1);
		}
	}

	cvShowImage( "Particles",img);
}
