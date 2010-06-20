
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

void DispMat(CvMat* mat)
{
	IplImage* img = MatToImage(mat);
	cvNamedWindow("Image Show");
	cvShowImage("Image Show", img);
	cvWaitKey(0);
	cvDestroyWindow("Image Show");
	cvReleaseImageHeader(&img);
}

void SetValInRectRange(CvMat* mat, double val, int x, int y, int width, int height){
	for(int i = x; i < x + width; i++){
		for(int j = y; j < y + height; j++){
			cvmSet(mat, j, i, val);
		}
	}
}

IplImage* GetSpotlightImage(IplImage* srcImage, IplImage* salMap)
{
	IplImage* pSrcFloatImage = GetFloatImage(srcImage);
	cvDilate(salMap, salMap, NULL, 3);
	cvNormalize(salMap, salMap, 1.0, 0, CV_MINMAX, NULL);

	IplImage* tempSalImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_32F, srcImage->nChannels);
	for(int c=0; c < pSrcFloatImage->nChannels; c++){
		cvSetImageCOI(tempSalImage, c);
		cvSetImageCOI(salMap, 0);
		cvCopy(salMap, tempSalImage);
	}
	IplImage* dstImage = cvCloneImage(pSrcFloatImage);
	cvMul(pSrcFloatImage, tempSalImage, dstImage, 1);
	return dstImage;
}

IplImage* GetFloatImage(IplImage* srcImage)
{
	IplImage* dstImage = cvCreateImage(cvGetSize(srcImage), IPL_DEPTH_32F ,srcImage->nChannels);
	if(srcImage->depth == IPL_DEPTH_8U){
		cvConvertScale(srcImage, dstImage, 1/255.0);
	}else if(srcImage->depth == IPL_DEPTH_32F){
		cvCopyImage(srcImage, dstImage);
	}else{
		printf("\nIn GetFloatImage(), the depth of input image is not IPL_DEPTH_8U!");
		return NULL;
	}
	return dstImage;	
}