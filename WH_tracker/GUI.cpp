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
	 switch(event) {
        /* left button down */
        case CV_EVENT_LBUTTONDOWN:
    		selectionflag = true;
            cout<<"Selected x "<<x<<" y "<<y<<"\n";
            break;
       
        /* mouse move */
        case CV_EVENT_MOUSEMOVE:
             //do nothing
	        break;
    }
}

void selectObject(InputArray img, _track* track, bool flag)
{
	selectionflag = flag;
	
		namedWindow( "SelectionWindow", 1 );
		imshow( "SelectionWindow",img);
		setMouseCallback( "SelectionWindow", on_mouse);
        while(!selectionflag)
            waitKey(10);
	

	//track->capture = capture;
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