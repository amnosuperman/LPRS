/*
 * Program to recognize characters
 */

#include "cv.h"
#include "highgui.h"
#include "ml.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "preprocessing.h"
#include "ocr.h"

//#include "preproc.c"

/*
char file_path[] = "../OCR/";

int train_samples = 50;
int classes= 10;
CvMat* trainData;
CvMat* trainClasses;

int size=40;

const int K=10;
CvKNearest *knn;
*/


void basicOCR::getData()
{
	IplImage* src_image;
	IplImage* prs_image;
	CvMat row,data;
	char file[255];
	int i,j;
	int counter=0;
	for(i =0; i<classes; i++){
		for( j = 0; j< train_samples[i]; j++){
			
			//Load file
            if(i<=25)
            {
                char c;
                c=97+i;
    			sprintf(file,"%s/%c/%c%d.jpg",file_path, c, c , j+1);
			    src_image = cvLoadImage(file,0);
			    if(!src_image){
				    printf("Error: Cant load image %s\n", file);
				}
				//exit(-1);
			}
            else
            {
    			sprintf(file,"%s/%d/%d%d.jpg",file_path, i-26, i-26 , j+1);
			    src_image = cvLoadImage(file,0);
			    if(!src_image){
				    printf("Error: Cant load image %s\n", file);
				}
				//exit(-1);
			}
			//process file
			prs_image = preprocessing(src_image, size, size);
			//Set class label
			cvGetRow(trainClasses, &row, counter);
			cvSet(&row, cvRealScalar(i));
			//Set data 
			cvGetRow(trainData, &row, counter);

			IplImage* img = cvCreateImage( cvSize( size, size ), IPL_DEPTH_32F, 1 );
			//convert 8 bits image to 32 float image
			cvConvertScale(prs_image, img, 0.0039215, 0);

			cvGetSubRect(img, &data, cvRect(0,0, size,size));
			
			CvMat row_header, *row1;
			//convert data matrix sizexsize to vecor
			row1 = cvReshape( &data, &row_header, 0, 1 );
			cvCopy(row1, &row, NULL);
			counter++;
		}
	}
}

void basicOCR::train()
{
	knn=new CvKNearest( trainData, trainClasses, 0, false, K );
}

int basicOCR::classify(IplImage* img, int showResult)
{

	IplImage *prs_image;
	CvMat data;
	CvMat* nearest=cvCreateMat(1,K,CV_32FC1);
	float result;

	//process file
	prs_image = preprocessing(img, size, size);
	
	//Set data 
	IplImage* img32 = cvCreateImage( cvSize( size, size ), IPL_DEPTH_32F, 1 );
	cvConvertScale(prs_image, img32, 0.0039215, 0);
	cvGetSubRect(img32, &data, cvRect(0,0, size,size));
	CvMat row_header, *row1;
	row1 = cvReshape( &data, &row_header, 0, 1 );

	result=knn->find_nearest(row1,K,0,0,nearest,0);
	
	int accuracy=0;
	for(int i=0;i<K;i++){
		if( nearest->data.fl[i] == result)
                    accuracy++;
	}

	float pre=100*((float)accuracy/(float)K);

	if(showResult==1){
		printf("|\t%.0f\t| \t%.2f%%  \t| \t%d of %d \t| \n",result,pre,accuracy,K);
		printf(" ---------------------------------------------------------------\n");
	}

	return (int) result;

}

/*void basicOCR::test(){

	IplImage* src_image;
	IplImage prs_image;
	CvMat row,data;
	char file[255];
	int i,j;
	int error=0;
	int testCount=0;
	for(i =0; i<classes; i++){
		for( j = 50; j< 50+train_samples; j++){
			
			sprintf(file,"%s%d/%d%d.pbm",file_path, i, i , j);
			src_image = cvLoadImage(file,0);
			if(!src_image){
				printf("Error: Cant load image %s\n", file);
				//exit(-1);
			}
			//process file
			prs_image = preprocessing(src_image, size, size);
			float r=classify(&prs_image,0);
			if((int)r!=i)
				error++;
			
			testCount++;
		}
	}
	float totalerror=100*(float)error/(float)testCount;
	printf("System Error: %.2f%%\n", totalerror);
	
}*/

basicOCR::basicOCR()
{

	//initial
	sprintf(file_path , "../characters");
    FILE *f;
    f=fopen("count.txt","r");
    char* d=NULL;
    //fscanf(f,"%d",&d);
    int no;
    size_t n;
    ssize_t x;
    int y=0;
    x=getline(&d,&n,f);
    
    while(x!=-1)
    {
        no=atoi(d);
        train_samples[y]=no;    
        x=getline(&d,&n,f);  
        y++;
    }
	
	int train_samples_total = 0;
	for(y=0;y<36;y++)
	{
	    train_samples_total+=train_samples[y];
    }
    classes= 36;
    size=40;
    
	trainData = cvCreateMat(train_samples_total, size*size, CV_32FC1);
	trainClasses = cvCreateMat(train_samples_total, 1, CV_32FC1);

	//Get data (get images and process it)
	getData();
	
	//train	
	train();
	
	//Test	
	//test();

}

