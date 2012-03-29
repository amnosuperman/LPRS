#include <cv.h>
#include <highgui.h>

#include "yellow.h"

IplImage* yellow(IplImage *frame) 
{
	IplImage *hsv;
	IplImage* b = cvCreateImage( cvGetSize(frame), 8, 1 );
	IplImage* g = cvCreateImage( cvGetSize(frame),8, 1 );
	IplImage* r= cvCreateImage( cvGetSize(frame),8, 1 );
	IplImage* b1 = cvCreateImage( cvGetSize(frame),8, 1 );
	IplImage* g1 = cvCreateImage( cvGetSize(frame),8, 1 );
	IplImage* r1 = cvCreateImage( cvGetSize(frame),8, 1 );
	hsv = cvCreateImage( cvGetSize(frame),8, 3 );
	cvSplit(frame,r,g,b,NULL);
	cvEqualizeHist(r,r1);
	cvEqualizeHist(b,b1);
	cvEqualizeHist(g,g1);
	cvMerge(r1,g1,b1,NULL,frame);
	cvCvtColor(frame,hsv,CV_BGR2HSV );
	IplImage* imgThreshed = cvCreateImage(cvGetSize(frame), 8, 1);
	cvInRangeS(hsv, cvScalar(20, 80, 80), cvScalar(40, 255, 255), imgThreshed);
	cvMorphologyEx( imgThreshed,imgThreshed, 0, 0, CV_MOP_CLOSE,1 );
	uchar *q=(uchar*)frame->imageData;
	uchar *p=(uchar*)imgThreshed->imageData;
	int x,y;
	for(x=0;x<(frame->height);x++)
	{
		for(y=0;y<(frame->width);y++)
		{
			if(*(p+(x*imgThreshed->widthStep)+y)==255)
			{
				*(q+(x*frame->widthStep)+y*3)=255;
				*(q+(x*frame->widthStep)+y*3+1)=255;
				*(q+(x*frame->widthStep)+y*3+2)=255;
			}
		}
	}
	return frame;
}
	

