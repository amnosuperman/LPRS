#include "cv.h"
#include "highgui.h"

#include "rot.h"

IplImage* rot(IplImage* img)
{

	IplImage* img1=cvCloneImage(img);
	IplImage* img2=cvCreateImage(cvSize(img->height,img->width),IPL_DEPTH_8U,3);
	cvFlip(img1,img1,1);
	cvTranspose(img1,img2);
	// release the image
	cvReleaseImage(&img1 );
	return img2;

}
	
		
