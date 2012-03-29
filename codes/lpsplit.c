#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

#include "lpsplit.h"
int split(IplImage * img)
{
    //IplImage* img=cvCreateImage(cvSize(imgo->width,imgo->height),imgo->depth,1);
	int height,width,step,channels,s,sum=0,sum1=0,st;
	uchar *data;
	
	int i,j,k;
	
    //cvCvtColor(imgo,img,CV_BGR2GRAY);
	
	// get the image data
	height= img->height;
	width= img->width;
	step= img->widthStep;
	channels = img->nChannels;
	data= (uchar *)img->imageData;
	s=height/2;

	//maximise total row intensity
	for(i=(s-(height/15));i<(s+(height/15));i++) //s can be set to 20,30 or 40 
    {
        sum=0;
        for(j=0;j<width;j++) { 
            for(k=0;k<channels;k++)
		    sum=sum+data[i*step+j*channels+k];
        }
        if(sum1<=sum) {
            sum1=sum;
            st=i;
        }
        //printf("sum%d=%d\n",i,sum);
    }
    if(sum1<(200*width))
        st=-1;

//    printf("MAX : %d @ %d\n",sum1,st);
//    CvPoint p=cvPoint(0,st);
//    CvPoint q=cvPoint(width,st);
//    cvLine(img,p,q,CV_RGB(255,0,0),1,8,0);
	// show the image
	//cvReleaseImage(&img );
	return st;
}

