

#include <stdio.h>
#include <math.h>
#include "saliency_map.h"
#include <time.h>
using namespace std;

void TestGetSaliencyMap();
void TestInBatch();

int main()
{
	//TestGetSaliencyMap();
	TestInBatch();
	return 0;
}

void TestGetSaliencyMap()
{
	VAMToolbox vam;
	IplImage *pSrcImg, *pSaliencyMap, *pSaliencyMapOfInt, *pSaliencyMapOfCol, *pSaliencyMapOfOri;
	clock_t timeBegin, timeEnd;
	char* imgName;
	imgName = "sailboats.jpg";
	//imgName = "balloons.jpg";
	//imgName = "faces.png";
	//imgName = "testori.jpg";
	pSrcImg = vam.LoadImage(imgName);
	if(pSrcImg == NULL){
		printf("\n>>>>Fail to Load Image: %s!!\n", imgName);
		system("pause");
		exit(-1);
	}
	timeBegin = clock();
	pSaliencyMap = vam.GetSaliencyMap(pSrcImg);
	pSaliencyMapOfInt = vam.GetSaliencyMapOfIntensity();
	pSaliencyMapOfCol = vam.GetSaliencyMapOfColor();
	pSaliencyMapOfOri = vam.GetSaliencyMapOfOrientation();
	timeEnd = clock();
	printf("\nThe running time is:%.2f sec.", (timeEnd - timeBegin)/(double)CLK_TCK);
	
	cvNamedWindow("Origin Image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Origin Image", pSrcImg);
	cvNamedWindow("Saliency Map",CV_WINDOW_AUTOSIZE);
	cvShowImage("Saliency Map", pSaliencyMap);
	cvNamedWindow("Saliency Map Of Intensity", CV_WINDOW_AUTOSIZE);
	cvShowImage("Saliency Map Of Intensity", pSaliencyMapOfInt);
	cvNamedWindow("Saliency Map Of Color", CV_WINDOW_AUTOSIZE);
	cvShowImage("Saliency Map Of Color", pSaliencyMapOfCol);
	cvNamedWindow("Saliency Map Of Orientation", CV_WINDOW_AUTOSIZE);
	cvShowImage("Saliency Map Of Orientation", pSaliencyMapOfOri);
	cvWaitKey(0);
	cvDestroyAllWindows();
	//cvReleaseImage(&pSaliencyMap);
	cvReleaseImage(&pSrcImg);
//	cvReleaseImage(&pSaliencyMapOfCol);
//	cvReleaseImage(&pSaliencyMapOfInt);
//	cvReleaseImage(&pSaliencyMapOfOri);
}
void TestInBatch()
{
	VAMToolbox vam;
	IplImage *pSrcImg, *pSaliencyMap, 
		*pSaliencyMapOfInt, *pSaliencyMapOfCol, *pSaliencyMapOfOri,
		*pSpotlightImage;
	char imgName[64];
	clock_t timeBegin, timeEnd;
	printf("*************************************************\n");
	printf("       Visual Attention Model Test\n");
	printf("*************************************************\n");
	while(TRUE){
		printf("\nPlease type the image name,then you will get the saliency map.\n");
		printf("Image Name:  ");
		scanf("%s",imgName);
		pSrcImg = vam.LoadImage(imgName);
		if(pSrcImg == NULL){
			printf("\n>>>>Fail to Load Image: %s!!\n", imgName);
			system("pause");
			continue;
		}
		timeBegin = clock();
		pSaliencyMap = vam.GetSaliencyMap(pSrcImg);
		pSaliencyMapOfInt = vam.GetSaliencyMapOfIntensity();
		pSaliencyMapOfCol = vam.GetSaliencyMapOfColor();
		pSaliencyMapOfOri = vam.GetSaliencyMapOfOrientation();
		//pSpotlightImage = GetSpotlightImage(pSrcImg, pSaliencyMap);
		timeEnd = clock();
		printf("\nThe running time is:%.2f sec.", (timeEnd - timeBegin)/(double)CLK_TCK);

		cvNamedWindow("Origin Image", CV_WINDOW_AUTOSIZE);
		cvShowImage("Origin Image", pSrcImg);
		cvNamedWindow("Saliency Map",CV_WINDOW_AUTOSIZE);
		cvShowImage("Saliency Map", pSaliencyMap);
		cvNamedWindow("Saliency Map Of Intensity", CV_WINDOW_AUTOSIZE);
		cvShowImage("Saliency Map Of Intensity", pSaliencyMapOfInt);
		cvNamedWindow("Saliency Map Of Color", CV_WINDOW_AUTOSIZE);
		cvShowImage("Saliency Map Of Color", pSaliencyMapOfCol);
		cvNamedWindow("Saliency Map Of Orientation", CV_WINDOW_AUTOSIZE);
		cvShowImage("Saliency Map Of Orientation", pSaliencyMapOfOri);
		int k = cvWaitKey(0);
		if(k == 27){ //Esc
			return;
		}
		cvDestroyAllWindows();
		//cvReleaseImage(&pSaliencyMap);
		cvReleaseImage(&pSrcImg);
//		cvReleaseImage(&pSaliencyMapOfCol);
//		cvReleaseImage(&pSaliencyMapOfInt);
//		cvReleaseImage(&pSaliencyMapOfOri);
		vam.Release();
	}
}
