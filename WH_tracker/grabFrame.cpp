#include "WHT.h"
 

IplImage* grabFrame(_track* track, IplImage* videoFrame)
{
	//updateCameraInfo(track);

	videoFrame = cvQueryFrame(track->capture);
	return(videoFrame);

}