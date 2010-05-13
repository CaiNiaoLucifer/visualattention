
#include "util.h"
#include "const.h"

IplImage* MatToImage(CvMat* mat)
{
	IplImage *img;
	CvSize imgSize = cvSize(mat->cols, mat->rows);
	
	if(mat->type == CV_8UC1){
		img = cvCreateImageHeader(imgSize, IPL_DEPTH_8U, 1);
	}else if(cvGetElemType(mat) == CV_32FC1){
		img = cvCreateImageHeader(imgSize, IPL_DEPTH_32F, 1);
	}else if(cvGetElemType(mat) == CV_8UC3){
		img = cvCreateImageHeader(imgSize, IPL_DEPTH_8U, 3);
	}else if(cvGetElemType(mat) == CV_32FC3){
		img = cvCreateImageHeader(imgSize, IPL_DEPTH_32F, 3);
	}
	cvGetImage(mat, img);
	return img;
}

void ReleaseBatch(CvMat*** mat, int numOfMats)
{
	if(numOfMats <= 0 || mat == NULL){
		return;
	}
	CvMat** &ppm = *mat;
	for(int i=0; i<numOfMats; i++){
		cvReleaseMat(&ppm[i]);
	}
	free(ppm);
	ppm = NULL;
}

void ReleaseBatch(CvMat**** mat, int numOfMats)
{
	if(numOfMats <= 0 || mat == NULL){
		return;
	}
	CvMat*** &ppm = *mat;
	for(int i=0; i<numOfMats; i++){
		free(ppm[i]);
		ppm[i] = NULL;
	}
	//free(*mat);
	//*mat = NULL;
}
//将元素中小于零的部分统一赋为零值。
void Clamp(CvMat *src,CvMat *dst)
{
	CvMat *tempSrc,*mask;
	tempSrc = cvCreateMat(src->rows,src->cols,src->type);
	mask = cvCreateMat(src->rows,src->cols,CV_8UC1);
	cvCopy(src,tempSrc);

	cvSetZero(dst);
	cvCmpS(tempSrc,0.0,mask,CV_CMP_GE);
	cvCopy(tempSrc,dst,mask);
}

bool CheckNegative(CvMat* src)
{
	for(int r=0; r<src->rows; r++){
		for(int c=0; c<src->cols; c++){
			if(cvmGet(src, r, c) < 0){
				return true;
			}
		}
	}
	return false;
}