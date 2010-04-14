#ifndef _VISUAL_ATTENTION_UTIL_
#define _VISUAL_ATTENTION_UTIL_

#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
using namespace std;

void DispMat(CvMat	*matImage);
void DispDoubleMat(CvMat *mat1,CvMat *mat2);
void PrintMatData(CvMat *mat,char* msg);
void SimDispMat(CvMat *matImage);
void DispImage(IplImage *img);


#endif