#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ocr.h"
//#include"mainclass.c"

//IplImage* imagen;
//IplImage* imagen_color;
IplImage* smooth;
IplImage* threshold;
IplImage* open_morf;
IplImage* img_contornos;
CvSeq* contour;
CvSeq* contourLow;

void rev(char* str,int count) {

    char x;
	int i;
	for(i=0;i<(count/2);i++)
	{
		x=str[i];
		str[i]=str[count-1-i];
		str[count-1-i]=x;
    }
}


char* char_ext(IplImage* imagen,basicOCR ocr  )
{
	
	//cvNamedWindow("temp");
	//cvShowImage("temp",imagen);
	//cvWaitKey(0);
	//char* plate=NULL;
	char* no=(char*)malloc(20*sizeof(char));
//-------------------------------------	-----------------------------------------------
    //NUMBER ISOLATION

	//Create needed images
	smooth= cvCreateImage(cvGetSize(imagen), imagen->depth, 1);
	threshold= cvCreateImage(cvGetSize(imagen), imagen->depth, 1);
	open_morf= cvCreateImage(cvGetSize(imagen), imagen->depth, 1);
	
	//Init variables for countours
	contour = 0;
	contourLow = 0;
	//Create storage needed for contour detection
	CvMemStorage* storage = cvCreateMemStorage(0);
	//Smooth image
	cvSmooth(imagen, smooth, CV_GAUSSIAN, 3, 0, 0, 0);
	
	CvScalar avg;
	CvScalar avgStd;
	cvAvgSdv(smooth, &avg, &avgStd, NULL);
	//printf("Avg: %f\nStd: %f\n", avg.val[0], avgStd.val[0]);
	//threshold image
	cvThreshold(smooth, threshold, (int)avg.val[0]+4*(int)(avgStd.val[0]/8), 255, CV_THRESH_BINARY_INV);
	//Morfologic filters
	cvErode(threshold, open_morf, NULL,1); 
	cvDilate(open_morf, open_morf, NULL,1); 
	//Duplicate image for countour
	img_contornos=cvCloneImage(open_morf);
	
	//Search countours in preprocesed image
	cvFindContours( img_contornos, storage, &contour, sizeof(CvContour),
			CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0) );
	//Optimize contours, reduce points
	contourLow=cvApproxPoly(contour, sizeof(CvContour), storage,CV_POLY_APPROX_DP,1,1);
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//NUMBER RECOGNITION
	CvRect rect;
	int carea=0,area=0;
	int count=0;
    int match;	
    int w,h;
    w=imagen->width;
    h=imagen->height;
	area=(w)*(h);
//	printf("area : %d, %d %d\n",area,w,h);
	//printf("\n%d\n",area/26);
	
	char name[6];
	//static int width;
	for( ; contourLow != 0; contourLow = contourLow->h_next )
	{		

		rect=cvBoundingRect(contourLow,0);
		cvSetImageROI(smooth,rect);
		IplImage *temp22=cvCreateImage(cvSize(rect.width,rect.height),IPL_DEPTH_8U,1);
		IplImage *temp23=cvCreateImage(cvSize(rect.width,rect.height),IPL_DEPTH_8U,1);
		cvCopy(smooth,temp22,NULL);
		carea=rect.width*rect.height;

		/*if((rect.width>rect.height)||(carea>(area/6))||(carea<(area/25)))
		{	
		    cvReleaseImage(&temp22);
		    continue;
		}*/
		
		if((carea<(area/4))&&(carea>(area/25)))
		{
			static int width = temp22->width;
			sprintf(name,"char%d",count);
			cvNamedWindow(name);
			cvMoveWindow(name,840 - count*3*width,10);
			cvThreshold(temp22, temp23, (int)avg.val[0]+4*(int)(avgStd.val[0]/8), 255, CV_THRESH_BINARY);

			cvShowImage(name,temp23);
			cvWaitKey(500);
			match=ocr.classify(temp23,0);
			if(match<=25)
			    no[count]=97+match;
			else
			    no[count]=48+match-26;

			count++;

		}

		cvReleaseImage(&temp22);
		cvReleaseImage(&temp23);		
		cvResetImageROI(smooth);
	}
	cvWaitKey(0);

	no[count]='\0';
	rev(no,count);
	//strcpy(plate,no);
	//printf("\n%d\n",count);
//-------------------------------------------------------------------------------------------------------------------------------------
	//cvReleaseImage(&imagen_color);
	cvReleaseImage(&imagen);
	cvReleaseImage(&open_morf);
	cvReleaseImage(&img_contornos);	
	return no;
	free(no);
}

