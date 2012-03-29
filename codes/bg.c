/* 
 * Codes for background subtraction
 */
 
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "bg.h"
#include <stdio.h>
#define CHANNELS 3

//Function List
//	update_codebook(uchar* p,codeBook &c,int numChannels/*unsigned* cbBounds=10*/)
//	clear_stale_entries(codeBook &c)
//	background_diff(uchar* p,codeBook &c,int numChannels,int minMod,int maxMod)
//	find_connected_components(IplImage *mask)
//	startlearnbackground(CvCapture *capture,codeBook * c)

int update_codebook(uchar* p,codeBook &c,int numChannels/*unsigned* cbBounds=10*/)
{
	unsigned int high[3],low[3];
	int n;
	for(n=0; n<numChannels; n++)
	{
		high[n] = *(p+n)+10;
		if(high[n] > 255) high[n] = 255;
		low[n] = *(p+n)-10;
		if(low[n] < 0) low[n] = 0;
	}
	int matchChannel;
	// SEE IF THIS FITS AN EXISTING CODEWORD
	//
	int i;
	for(i=0; i<c.numEntries; i++)
	{
		matchChannel = 0;
		for(n=0; n<numChannels; n++)
		{
			if((c.cb[i]->learnLow[n] <= *(p+n)) &&(*(p+n) <= c.cb[i]->learnHigh[n]))  //Found an entry for this channel
			{
				matchChannel++;
			}	
		}
		if(matchChannel == numChannels) //If an entry was found
		{
			c.cb[i]->t_last_update = c.t;
			//adjust this codeword for the first channel
			for(n=0; n<numChannels; n++)
			{
				if(c.cb[i]->max[n] < *(p+n))
				{
					c.cb[i]->max[n] = *(p+n);
				}
				else if(c.cb[i]->min[n] > *(p+n))
				{
					c.cb[i]->min[n] = *(p+n);
				}
			}
			break;
		}
	}
	// OVERHEAD TO TRACK POTENTIAL STALE ENTRIES
	//
	for(int s=0; s<c.numEntries; s++)
	{
		// Track which codebook entries are going stale:
		//
		int negRun = c.t - c.cb[s]->t_last_update;
		if(c.cb[s]->stale < negRun) c.cb[s]->stale = negRun;
	}
	// ENTER A NEW CODEWORD IF NEEDED
	//	
	if(i == c.numEntries) //if no existing codeword found, make one
	{
		code_element **foo = new code_element* [c.numEntries+1];
		for(int ii=0; ii<c.numEntries; ii++) 
		{
			foo[ii] = c.cb[ii];
		}
		foo[c.numEntries] = new code_element;
		if(c.numEntries) delete [] c.cb;
		c.cb = foo;
		for(n=0; n<numChannels; n++) 
		{
			c.cb[c.numEntries]->learnHigh[n] = high[n];
			c.cb[c.numEntries]->learnLow[n] = low[n];
			c.cb[c.numEntries]->max[n] = *(p+n);
			c.cb[c.numEntries]->min[n] = *(p+n);
		}
		c.cb[c.numEntries]->t_last_update = c.t;
		c.cb[c.numEntries]->stale = 0;
		c.numEntries += 1;
	}
	// SLOWLY ADJUST LEARNING BOUNDS
	//
	for(n=0; n<numChannels; n++)
	{
		if(c.cb[i]->learnHigh[n] < high[n]) c.cb[i]->learnHigh[n] += 1;
		if(c.cb[i]->learnLow[n] > low[n]) c.cb[i]->learnLow[n] -= 1;
	}
	return(i);
}

int clear_stale_entries(codeBook &c)
{
	int staleThresh = c.t>>1;
	int *keep = new int [c.numEntries];
	int keepCnt = 0;
	// SEE WHICH CODEBOOK ENTRIES ARE TOO STALE
	//
	for(int i=0; i<c.numEntries; i++)
	{
		if(c.cb[i]->stale > staleThresh)
		keep[i] = 0; //Mark for destruction
		else
		{
			keep[i] = 1; //Mark to keep
			keepCnt += 1;
		}
	}
	// KEEP ONLY THE GOOD
	//
	c.t = 0;
	//Full reset on stale tracking
	code_element **foo = new code_element* [keepCnt];
	int k=0;
	for(int ii=0; ii<c.numEntries; ii++)
	{
		if(keep[ii])
		{
			foo[k] = c.cb[ii];
			//We have to refresh these entries for next clearStale
			foo[k]->t_last_update = 0;
			k++;
		}
	}
	// CLEAN UP
	//
	delete [] keep;
	delete [] c.cb;
	c.cb = foo;
	int numCleared = c.numEntries - keepCnt;
	c.numEntries = keepCnt;
	return(numCleared);
}
uchar background_diff(uchar* p,codeBook  &c,int numChannels,int minMod,int maxMod) 
{
	int matchChannel;
	// SEE IF THIS FITS AN EXISTING CODEWORD
	//
	int i;
	for(i=0; i<c.numEntries; i++) 
	{
		matchChannel = 0;
		for(int n=0; n<numChannels; n++) 
		{
			if((c.cb[i]->min[n] - minMod <= *(p+n)) &&(*(p+n) <= c.cb[i]->max[n] + maxMod)) 
			{
				matchChannel++; //Found an entry for this channel
			} 
			else 
			{
				break;
			}
		}
		if(matchChannel == numChannels) 
		{
			break; //Found an entry that matched all channels
		}
	}
	if(i >= c.numEntries) return(255);
	return(0);
}
void find_connected_components(IplImage *mask) 
{
//	IplImage *im=cvCreateImage(cvSize(mask->width,mask->height),8,1);
//	cvCvtColor(mask,im,CV_BGR2GRAY);	
	static CvMemStorage* mem_storage = NULL;
	static CvSeq* contours= NULL;
	//CLEAN UP RAW MASK
	//
	cvMorphologyEx( mask, mask, 0, 0, CV_MOP_OPEN,1 );
	cvMorphologyEx( mask, mask, 0, 0, CV_MOP_CLOSE,1 );
	//FIND CONTOURS AROUND ONLY BIGGER REGIONS
	//
	if( mem_storage==NULL ) 
	{
		mem_storage = cvCreateMemStorage(0);
	} 
	else 
	{
		cvClearMemStorage(mem_storage);
	}
	CvContourScanner scanner = cvStartFindContours(mask,mem_storage,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	CvSeq* c;
	int numCont = 0;
	while( (c = cvFindNextContour( scanner )) != NULL ) 
	{
		double len = cvContourPerimeter( c );
		// calculate perimeter len threshold:
		//
		double q = (mask->height + mask->width)/4;
		//Get rid of blob if its perimeter is too small:
		//
		if( len < q ) 
		{
			cvSubstituteContour( scanner, NULL );
		} 
		else 
		{
			// Smooth its edges if its large enough
			//
			CvSeq* c_new;
			c_new = cvApproxPoly(c,sizeof(CvContour),mem_storage,CV_POLY_APPROX_DP,2,0);
			cvSubstituteContour( scanner, c_new );
			numCont++;
		}
	}
	contours = cvEndFindContours( &scanner );
	// PAINT THE FOUND REGIONS BACK INTO THE IMAGE
	//
	const CvScalar CVX_WHITE = CV_RGB(0xff,0xff,0xff);
	const CvScalar CVX_BLACK = CV_RGB(0x00,0x00,0x00);
	cvZero( mask );
	IplImage *maskTemp;
	for( c=contours; c != NULL; c = c->h_next ) 
	{
		cvDrawContours(mask,c,CVX_WHITE,CVX_BLACK,-1,CV_FILLED,8);
	}
}

void startlearnbackground(CvCapture *capture,codeBook  *c)
{
	IplImage* frame=cvQueryFrame(capture);
	int i,x,y;
	uchar *p;
	cvNamedWindow("Bg sub");
	for(i=0;i<=100;i++)
	{
		frame=cvQueryFrame(capture);
		cvShowImage("Bg sub",frame);
		p=(uchar *)frame->imageData;	
		cvWaitKey(20);		
		for(x=0;x<(frame->width);x++)
		{
			for(y=0;y<(frame->height);y++)
			{
				update_codebook(p,c[x + y*640],3);	
				if((i%40==0)&&(i>0))
					clear_stale_entries(c[x + y*640]);
				p+=3;
			}
		}
	}
	printf("Finished studying the background\n");
	cvReleaseImage(&frame);
	delete p;
}

void updatebackground(CvCapture *capture,codeBook *c)
{
	IplImage* frame=cvQueryFrame(capture);
	int i,x,y;
	uchar *p;
	for(i=0;i<=20;i++)
	{
		frame=cvQueryFrame(capture);
		p=(uchar *)frame->imageData;	
		cvWaitKey(10);		
		for(x=0;x<(frame->width);x++)
		{
			for(y=0;y<(frame->height);y++)
			{
				update_codebook(p,c[x + y*640],3);	
				clear_stale_entries(c[x + y*640]);
				p+=3;
			}
		}
	}
	for(x=0;x<(frame->width);x++)
	{
		for(y=0;y<(frame->height);y++)
		{
			clear_stale_entries(c[x + y*640]);
			p+=3;
		}
	}
	cvReleaseImage(&frame);
	delete p;
}

IplImage* diff(codeBook *c,IplImage *frame)
{
	IplImage* frame1=cvCreateImage(cvSize(frame->width,frame->height),8,1);
	IplImage* frame2=cvCreateImage(cvSize(frame->width,frame->height),8,3);
	int x,y;
	uchar *p,*q,*r;
	p=(uchar*)frame->imageData;
	q=(uchar*)frame1->imageData;		
	for(x=0;x<(frame->width);x++)
	{
		for(y=0;y<(frame->height);y++)
		{
			*q=background_diff(p,c[x + y*640],3,3,3);
			p+=3;
			q+=1;
		}
	}		
	find_connected_components(frame1);
	r=(uchar*)frame2->imageData;
	p=(uchar*)frame->imageData;
	q=(uchar*)frame1->imageData;		
	for(x=0;x<(frame->width);x++)
	{
		for(y=0;y<(frame->height);y++)
		{
			if((*q)==0)
			{	*r=0;*(r+1)=0;*(r+2)=0;
			}
			else
			{
				*r=*p;*(r+1)=*(p+1);*(r+2)=*(p+2);
			}
			p+=3;				
			r+=3;
			q+=1;
		}
	}		
	delete p;
	delete q;
	delete r;
	cvReleaseImage(&frame1);	
	return frame2;
	cvReleaseImage(&frame2);
}	

