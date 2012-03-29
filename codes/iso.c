/* 
 * Codes for plate isolation
 */
 
#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <stdlib.h>
#include "iso.h"

#define MINCHAR = 3

IplImage* isolate_plate(IplImage *origImage)
{
	CvSeq *contour,*contourLow;
	contour=0;contourLow=0;
	CvMemStorage* storage = cvCreateMemStorage(0);
	IplImage* originalImage =cvCreateImage(cvGetSize(origImage),IPL_DEPTH_8U,1);
	if(origImage->nChannels!=1)
	{
		cvCvtColor(origImage,originalImage,CV_BGR2GRAY);
	}
	else
		originalImage = origImage;
	IplImage* edgeImage =cvCreateImage(cvGetSize(originalImage),IPL_DEPTH_16S,1);
	IplImage* absedgeImage =cvCreateImage(cvGetSize(originalImage),IPL_DEPTH_8U,1);
	cvSobel(originalImage,edgeImage,1,1,3);
	cvConvertScaleAbs(edgeImage,absedgeImage);
	const int ind=absedgeImage->height,ind1=absedgeImage->width;	
	double r[ind];
	double c[50];	
	uchar *p;
	int x,y,w,z;

//	row pixel value summation (horizontal projection)
	p=(uchar *)absedgeImage->imageData;
	for(x=0;x<(absedgeImage->height);x++)
	{
		r[x]=0.0;
		for(y=0;y<(absedgeImage->width);y++)
		{
			r[x]+=*p;
			p++;
		}
	}

//	setting threshold for finding peaks
	CvMat M,M1;
	cvInitMatHeader(&M,ind, 1, CV_64FC1, r );

	CvScalar mean,stddev,mean1,stddev1;
	cvAvgSdv(&M,&mean,&stddev);
	double val=0.0,max=0.0,thresh=mean.val[0]+((0.0*stddev.val[0]));
	int start[100],stop[100],start1[20],stop1[20];
	int flag=0,i=0,k=0,flag1=0,i1=0,k1=0;
	for(x=0;x<(absedgeImage->height);x++)
	{
		if(r[x]>thresh)
		{
			if(flag==1)
			{}
			else
			{
				start[i]=x;
				flag=1;	
			}
		}
		else
		{
			if(flag==1)
			{
				stop[i]=x;
				flag=0;
				i++;
			}
			else
			{}
		}
	}

//	combining regions with small gaps
	for(y=0;y<(i-1);y++)
	{
		l :
		if(((start[y+1]-stop[y])<=5)&&(y<(i-1)))
		{
			for(k=y;k<(i-1);k++)
			{	stop[k]=stop[k+1];}
			for(k=(y+1);k<(i-1);k++)
			{	start[k]=start[k+1];}
			i--;
			goto l;
		}
	}

//	ignoring very thin regions
	for(y=0;y<(i-1);y++)
	{
		l2 :
		if(((stop[y]-start[y])<=10.0)&&(y<(i-1)))
		{
			for(k=y;k<(i-1);k++)
			{	stop[k]=stop[k+1];}
			for(k=y;k<(i-1);k++)
			{	start[k]=start[k+1];}
			i--;
			goto l2;
		}
		
	}

	if((stop[i-1]-start[i-1])<=10)
		i--;	
	k=0;
	max=0.0;
	w=i;
//-----------------------------------------------------------------------------------------------------------------------------------------------

//      taking vertical projection within identified regions
	IplImage *temp,*temp1,*temp2;	
	int count[10];
	double max1[10];
	int coord[10][4];
	for(z=0;z<w;z++)
	{
		count[z]=0;		
		i=(absedgeImage->width)/50;
		int j;
		IplImage *subimg=cvCreateImage(cvSize(i,(stop[z]-start[z])),8,1);
		CvScalar avg;
		for(j=0;j<50;j++)
		{
			cvSetImageROI(absedgeImage,cvRect((j*i),start[z],i,(stop[z]-start[z])));
			cvCopy(absedgeImage,subimg);
			cvResetImageROI(absedgeImage);
			avg=cvAvg(subimg,NULL);
			c[j]=avg.val[0];
		}			
		cvReleaseImage(&subimg);		
		cvInitMatHeader(&M1,50, 1, CV_64FC1, c );
		cvAvgSdv(&M1,&mean1,&stddev1);
		double thresh1=mean1.val[0]/*+((1.0*stddev1.val[0]))*/;
		flag1=i1=0;
		for(x=0;x<50;x++)
		{
			if(c[x]>thresh1)
			{
				if(flag1==1)
				{}
				else
				{
					start1[i1]=x;
					flag1=1;	
				}
			}
			else
			{
				if(flag1==1)
				{
					stop1[i1]=x;
					flag1=0;
					i1++;
				}
				else
				{}
			}
		}

//		combining regions with small gaps
		for(y=0;y<(i1-1);y++)
		{
			l1 :
			if(((start1[y+1]-stop1[y])<=2)&&(y<(i1-1)))
			{
				for(k=y;k<(i1-1);k++)
				{	stop1[k]=stop1[k+1];}
				for(k=(y+1);k<(i1-1);k++)
				{	start1[k]=start1[k+1];}
				i1--;
				goto l1;
			}
		}

//		ignoring very narrow regions
		for(y=0;y<(i1-1);y++)
		{
			l3 :
			if(((stop1[y]-start1[y])<=2)&&(y<(i1-1)))
			{
				for(k1=y;k1<(i1-1);k1++)
				{	stop1[k1]=stop1[k1+1];}
				for(k1=y;k1<(i1-1);k1++)
				{	start1[k1]=start1[k1+1];}
				i1--;
			goto l3;
			}
		}
		if((stop1[i1-1]-start1[i1-1])<=2)
		{	i1--;}	
	
//		finding the best rectangle in each horizontal strip
		k=0;
		max=0.0;
		for(x=0;x<i1;x++)
		{
			val=0.0;	
			for(y=start1[x];y<stop1[x];y++)
			{
				val+=c[y];
			}
			if(max<(val/(stop1[x]-start1[x])))
			{
				max=(val/(stop1[x]-start1[x]));
				k=x;
			}
		}
		max1[z]=max;

//		checking if the identified region could be number plate based on contour method
		cvSetImageROI(originalImage,cvRect((start1[k]*i),start[z],(stop1[k]-start1[k])*i,(stop[z]-start[z])));
		temp=cvCreateImage(cvSize((stop1[k]-start1[k])*i,(stop[z]-start[z])),8,1);
		temp1=cvCreateImage(cvSize((stop1[k]-start1[k])*i,(stop[z]-start[z])),8,1);
		cvCopy(originalImage,temp);
		coord[z][0]=start1[k]*i;coord[z][1]=stop1[k]*i;coord[z][2]=start[z];coord[z][3]=stop[z];
		int area;
		int carea;
		area=(stop1[k]-start1[k])*i*(stop[z]-start[z]);
		CvScalar avg2;
		CvScalar avgStd2;
		cvAvgSdv(temp, &avg2, &avgStd2, NULL);
		cvThreshold(temp, temp1, (int)avg2.val[0]+4*(int)(avgStd2.val[0]/8), 255, CV_THRESH_BINARY_INV);
		//Morfologic filters
		cvErode(temp1, temp1, NULL,1); 
		cvDilate(temp1, temp1, NULL,1); 
		//Duplicate image for countour
		temp2=cvCloneImage(temp1);
		//Search countours in preprocesed image
		cvFindContours( temp2, storage, &contour, sizeof(CvContour),CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0) );
		//Optimize contours, reduce points
		contourLow=cvApproxPoly(contour, sizeof(CvContour), storage,CV_POLY_APPROX_DP,1,1);
		CvRect rect;	

		for( ; contourLow != 0; contourLow = contourLow->h_next )
		{
			//CvScalar color = CV_RGB(0,0,0);
			//We can draw the contour of object
			//cvDrawContours(temp,contourLow, color, color, -1, 0, 8, cvPoint(0,0) );
			//Or detect bounding rect of contour	
			CvRect rect;
			CvPoint pt1, pt2;
			rect=cvBoundingRect(contourLow, 0);
			pt1.x = rect.x;
            		pt2.x = (rect.x+rect.width);
            		pt1.y = rect.y;
            		pt2.y = (rect.y+rect.height);
			carea=rect.width*rect.height;
			if((carea<(area/6))&&(carea>(area/25)))
				count[z]++;
		}
		cvResetImageROI(originalImage);			
	}
//	identifying the most probable number plate
	max=0.0;
	k=-1;
	for(z=0;z<w;z++)
	{
		if((count[z]>0)&&(count[z]<15))
		{
			if(max1[z]>max)
			{
				max=max1[z];
				k=z;
			}
		}
	}
	if(k==-1)
		return 0;
	temp=cvCreateImage(cvSize(coord[k][1]-coord[k][0]+15,coord[k][3]-coord[k][2]+5),8,1);
	cvSetImageROI(originalImage,cvRect(coord[k][0]-15,coord[k][2]-5,coord[k][1]-coord[k][0]+15,coord[k][3]-coord[k][2]+5));
	cvCopy(originalImage,temp,NULL);
	cvResetImageROI(originalImage);
	cvNamedWindow("Plate");
	cvShowImage("Plate",temp);
	cvWaitKey(500);
	//cvReleaseWindow("Plate");
	//temp->width=coord[k][1]-coord[k][0]+5;
	//temp->height=coord[k][3]-coord[k][2];
//---------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------
	cvReleaseImage(&originalImage);
	cvReleaseImage(&edgeImage);
	cvReleaseImage(&absedgeImage);
	cvReleaseImage(&temp1);
	cvReleaseImage(&temp2);
	return temp;
	cvReleaseImage(&temp);
}
