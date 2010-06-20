
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
void DispMat(CvMat* mat);
void SetValInRectRange(CvMat* mat, double val, int x, int y, int width, int height);
IplImage* GetSpotlightImage(IplImage* srcImage, IplImage* salMap);
IplImage* GetFloatImage(IplImage* srcImage);
#endif