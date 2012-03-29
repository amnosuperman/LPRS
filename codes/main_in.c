/* 
 * Program for license plate recognition
 * Team LPRS
 * CFI Project
 */
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <SerialStream.h>
#include <iostream>
#include <string.h>
#include "bg.h"  
#include "iso.h"
#include "yellow.h"                 
#include "rot.h"
#include "lpsplit.h"
#include "split.h"
#include "in_gate.h"
#include "ocr.h"
#include "preprocessing.h"

using namespace std;
using namespace LibSerial;

// main
int main(int argc,char** argv)
{

	// Declaration block
	int starting = 3,counter=0;
	int splity;
	char r;
	char* pnumber;
	IplImage *frame,*plate,*bgsub_img,*rotimg;	
    basicOCR ocr;
    
    // To do the processing with a picture and not the cam feed
    if(argc>2)
    {	
    	//cvWaitKey(0);
        int argno;
        for(argno=1;argno<argc;argno++)
        {
        	frame=cvLoadImage(argv[argno],-1);
        	cvNamedWindow("Image");
	        cvShowImage("Image",frame);
    	    cvWaitKey(500);
			plate=isolate_plate(frame);
		
			// Check for split and pass to OCR
			splity=split(plate);
			if(splity==-1)
			{
			    pnumber=char_ext(plate,ocr);
			}
			else
			{
			    IplImage *uplate,*lplate;
			    char* no1,* no2; 
			    uplate=cvCreateImage(cvSize(plate->width,splity),8,3);
			    lplate=cvCreateImage(cvSize(plate->width,plate->height-splity),8,3);
			    cvSetImageROI(plate,cvRect(0,0,plate->width,splity));
			    uplate=cvCloneImage(plate);
			    cvResetImageROI(plate);
			    cvSetImageROI(plate,cvRect(0,splity+1,plate->width,plate->height-splity));
			    lplate=cvCloneImage(plate);
			    cvResetImageROI(plate);
			    no1=char_ext(uplate,ocr);
			    no2=char_ext(lplate,ocr);
			    strcat(no1,no2);
			    strcpy(pnumber,no1);
			    //printf("d\n");
			}
			cvWaitKey(1000);
		//printf("%s\n",pnumber);
        }
    }

    else if(argc>1)
    {
        frame=cvLoadImage(argv[1],-1);
        cvNamedWindow("temp");
        cvShowImage("temp",frame);
        cvWaitKey(0);
		plate=isolate_plate(frame);
		
		// Check for split and pass to OCR
		splity=split(plate);
		if(splity==-1)
		{
		    pnumber=char_ext(plate,ocr);
		}
		else
		{
		    IplImage *uplate,*lplate;
		    char* no1=NULL,* no2=NULL; 
		    uplate=cvCreateImage(cvSize(plate->width,splity),8,3);
		    lplate=cvCreateImage(cvSize(plate->width,plate->height-splity),8,3);
		    cvSetImageROI(plate,cvRect(0,0,plate->width,splity));
		    uplate=cvCloneImage(plate);
		    cvResetImageROI(plate);
		    cvSetImageROI(plate,cvRect(0,splity+1,plate->width,plate->height-splity));
		    lplate=cvCloneImage(plate);
		    cvResetImageROI(plate);
		    no1=char_ext(uplate,ocr);
		    no2=char_ext(lplate,ocr);
		    strcat(no1,no2);
		    strcpy(pnumber,no1);
		    printf("d\n");
		}
		
    }

    else
    {
    // Initializing camera capture
	CvCapture *capture=cvCreateCameraCapture(2);
	if(!capture)
	{
		printf("Could not initiate capture...\n");
		//exit(0);
	}

	
	// Initializing serial port
	SerialStream serial_stream("/dev/ttyUSB0", ios_base::in | ios_base::out);	
	serial_stream.SetBaudRate( SerialStreamBuf::BAUD_1200 ) ;
	serial_stream.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 ) ;
	serial_stream.SetNumOfStopBits(1) ;
	
	// Starting program
	printf ("Starting in ");
    fflush (stdout);
    while (starting > 0)
    {
        printf ("\b\b\b %d...", starting);
    	fflush (stdout);
    	sleep (1);
    	starting--;
    }
  	printf ("\n");
	
	frame=cvQueryFrame(capture);
	
	codeBook *c=(codeBook *)malloc(640*480*sizeof(codeBook));

    // Starting to learn the background
    printf("Learning the background... Please wait\n");
    startlearnbackground(capture,c);
    printf("crossed bgsub\n");
		char lprs='b';
	cvNamedWindow("Camfeed");
	// Main loop
	while(1)
	{
		counter++;
		
		// Updating the background at regular intervals
		if(counter==10000)
		{
			updatebackground(capture,c);
			counter=0;
		}
		
		// Reading from the serial port
		serial_stream>>r;
		
		// Flush the buffer
		
		
		// If vehicle is present at the gate :
		frame=cvQueryFrame(capture);
		cvShowImage("Camfeed",frame);
		//lprs=cvWaitKey(10);
		printf("showing\n");
		
		if(r=='a')
		{
			frame=cvQueryFrame(capture);
			frame=cvQueryFrame(capture);
			bgsub_img=cvCreateImage(cvGetSize(frame),8,3);
			
			// Subtract the background from the image
			bgsub_img=diff(c,frame);
			bgsub_img=yellow(bgsub_img);
			
			rotimg=rot(bgsub_img);
			// Isolate the number plate from the bg subtracted image
			plate=cvCloneImage(isolate_plate(bgsub_img));
			
			// Check for split and pass to OCR
			splity=split(plate);
			
			if(splity==-1)
			{
			    pnumber=char_ext(plate,ocr);    
			}
			
			else
			{
			    IplImage *uplate,*lplate;
			    char* no1=NULL,* no2=NULL; 
			    uplate=cvCreateImage(cvSize(plate->width,splity),8,3);
			    lplate=cvCreateImage(cvSize(plate->width,plate->height-splity),8,3);
			    cvSetImageROI(plate,cvRect(0,0,plate->width,splity));
			    uplate=cvCloneImage(plate);
			    cvResetImageROI(plate);
			    cvSetImageROI(plate,cvRect(0,splity+1,plate->width,plate->height-splity));
			    lplate=cvCloneImage(plate);
			    cvResetImageROI(plate);
			    no1=char_ext(uplate,ocr);
			    no2=char_ext(lplate,ocr);
			    strcat(no1,no2);
			    strcpy(pnumber,no1);
			}
			
			// File input/output
            textdata(pnumber);            			
			// Send output to serial port for opening the gate
            serial_stream<<'b';
        }
    }
    free(c);
    }
    z :
    return 0;        							
}
