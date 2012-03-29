#include <cv.h>
#include <highgui.h>
#include <stdio.h>
IplImage* imagen;
IplImage* smooth;
IplImage* threshold;
IplImage* open_morf;
int main( int argc, char** argv )
{
	//Check if user specify image to process
	if(argc >= 2 )
	{
		char* filename= argv[1];
		//load image  in gray level
		imagen=cvLoadImage(filename,0);
	}
	else
	{
		printf("Use:\n\t%s image\n",argv[0]);
		return 0;
	}	
//------------------------------------------------------------------------------------
//NUMBER ISOLATION

	//Create needed images
	smooth= cvCreateImage(cvSize(imagen->width, imagen->height), IPL_DEPTH_8U, 1);
	threshold= cvCreateImage(cvSize(imagen->width, imagen->height), IPL_DEPTH_8U, 1);
	open_morf= cvCreateImage(cvSize(imagen->width, imagen->height), IPL_DEPTH_8U, 1);
	cvSmooth(imagen, smooth, CV_GAUSSIAN, 3, 0, 0, 0);
	
	CvScalar avg;
	CvScalar avgStd;
	cvAvgSdv(smooth, &avg, &avgStd, NULL);
	//printf("Avg: %f\nStd: %f\n", avg.val[0], avgStd.val[0]);
	//threshold image
	cvThreshold(smooth, threshold, (int)avg.val[0]+4*(int)(avgStd.val[0]/8), 255, CV_THRESH_BINARY);
	//Morfologic filters
	cvErode(threshold, open_morf, NULL,1); 
	cvDilate(open_morf, open_morf, NULL,1); 
	int lprs=cvSaveImage(argv[2],open_morf,&lprs);
	//Duplicate image for countour
	cvReleaseImage(&imagen);
	cvReleaseImage(&open_morf);
	return 0;
}

