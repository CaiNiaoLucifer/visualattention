
#ifndef _VISUAL_ATTENTION_UTIL_
#define _VISUAL_ATTENTION_UTIL_

#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
using namespace std;

IplImage* MatToImage(CvMat* mat);
void Clamp(CvMat *src,CvMat *dst);
void ReleaseBatch(CvMat*** mat, int numOfMats);
void ReleaseBatch(CvMat**** mat, int numOfMats);
bool CheckNegative(CvMat* src);

#endif