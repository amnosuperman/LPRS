#include "cv.h"
#include "highgui.h"
#include "ml.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "preprocessing.h"

#ifndef OCR_H
#define OCR_H

class basicOCR {
	public:
		int classify(IplImage* img,int showResult);
		basicOCR ();
//		void test();	
	private:
		char file_path[255];
		int train_samples[36];
		int classes;
		CvMat* trainData;
		CvMat* trainClasses;
		int size;
		static const int K=15;
		CvKNearest *knn;
		void getData();
		void train();
};

#endif
