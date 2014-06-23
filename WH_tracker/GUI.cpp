//Contains code regarding the C GUI. The main objective is to select object for 
//tracking and return its coordinates.


#include "WHT.h"

int sel_x = 0;
int sel_y = 0;
bool selectionflag = false;
IplImage* videoFrame = NULL;

static void on_mouse(int event, int x, int y, int flags, void *param)
{
	sel_x = x;
	sel_y = y;

	selectionflag = false;
	IplImage* selectionImage = cvCloneImage(videoFrame);
    switch(event) {
        /* left button down */
        case CV_EVENT_LBUTTONDOWN:
            cvRectangle(selectionImage,
                        cvPoint(x - 10, y - 10),
                        cvPoint(x + 10, y + 10),
                        cvScalar(0, 0, 255, 0), 2, 8, 0);
            cvShowImage("SelectionWindow", selectionImage);
			selectionflag = true;
            break;
       
        /* mouse move */
        case CV_EVENT_MOUSEMOVE:
			cvRectangle(selectionImage,
                        cvPoint(x - 10, y - 10),
                        cvPoint(x + 10, y + 10),
                        cvScalar(0, 0, 255, 0), 2, 8, 0);
			cvShowImage("SelectionWindow", selectionImage);
			cvReleaseImage(&selectionImage);
            break; 
    }
}

void selectObject(CvCapture* capture, _track* track, bool flag)
{
	selectionflag = flag;
	while(selectionflag==false)
	{
		videoFrame = cvQueryFrame(capture);
		cvNamedWindow( "SelectionWindow", 1 );
		cvShowImage( "SelectionWindow",videoFrame);
		cvSetMouseCallback( "SelectionWindow", on_mouse);
		cvWaitKey(10);
	}

	track->capture = capture;
	track->x = sel_x;
	track->y = sel_y;	
	track->scale = 20;
	track->activeparticles = Nmax;

	if(selectionflag == true)
	{
		track->selection = true;
		cvDestroyWindow("SelectionWindow");
	}
	else
	{
		track->selection = false;
		
	}
}