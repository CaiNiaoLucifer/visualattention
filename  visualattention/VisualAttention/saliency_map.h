
#ifndef _VISUAL_ATTENTION_SALIENCY_MAP_
#define _VISUAL_ATTENTION_SALIENCY_MAP_

#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>
#include "util.h"
#include "const.h"
using namespace std;

void ColorNorm(CvMat *rMat, CvMat *gMat,CvMat *bMat,CvMat *dstMat,uchar flag);
void CompIntensity(CvMat *IntensityMat,CvMat *pyrMat[]);
void CompColor(CvMat *ColorMap,CvMat *pyrMat[]);
void CompOrientation(CvMat *OrientationMap,CvMat *pyrMat[]);
void CreateImagePyramid(IplImage *img,CvMat *pyrMat[]);
void NonLinearAmp(CvMat *src,CvMat *dst);
void MakeGaborFilter(CvMat *Gr, CvMat *Gi, int s, int n, double Ul, double Uh, int scale, int orientation, int flag);
void GaborFilterImage(CvMat *src,CvMat *dst,int scale,int ori);
void DispImageMat(IplImage *img,CvMat *mat);
void DoGFilter(float exSigma,float exC,float inhSigma,float inhC,int radius,CvMat *T);
void IterativeNorm(CvMat *src,CvMat *dst,int nIteration);
void TrunDoGConv(CvMat *src,CvMat *dst,CvMat *T);
void Clamp(CvMat *src,CvMat *dst);


#endif