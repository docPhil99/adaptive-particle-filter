//#include "stdafx.h"
#include "WHT.h"

#define sigma_color 0.2							
#define cte1_color 1/(2*sigma_color*sigma_color)								
#define sq2pi sqrt(2*3.14159)	
#define cte2_color 1/(sq2pi*sigma_color)

void calculateHistogram(IplImage*, _histogram*);
_histogram* calculateRefHistogram(IplImage*, _track*);
_histogram* calculateCurrentHistogram(IplImage*);
void predict(_particles*, _track*, IplImage*, gsl_rng*);
void update(_particles*, _track*, IplImage*);
void resampleparticles( _particles*, _particles*, int);
int particle_cmp( const void*, const void*);
void resample(_particles*, _track*);
void displayParticles(_particles*, _track*, IplImage*); 