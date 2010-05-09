

#include <stdio.h>
#include <math.h>
#include "saliency_map.h"
#include <time.h>
using namespace std;

void TestGetSaliencyMap();
void TestInBatch();

int main()
{
	TestInBatch();
	return 0;
}

void TestGetSaliencyMap()
{
	VAMToolbox vam;
	IplImage *pSrcImg, *pSaliencyMap;
	clock_t timeBegin, timeEnd;
	char* imgName;
	//imgName = "sailboats.png";
	//imgName = "horse.jpg";
	imgName = "faces.png";
	pSrcImg = vam.LoadImage(imgName);
	if(pSrcImg == NULL){
		printf("\n>>>>Fail to Load Image: %s!!\n", imgName);
		system("pause");
		exit(-1);
	}
	timeBegin = clock();
	pSaliencyMap = vam.GetSaliencyMap(pSrcImg);
	timeEnd = clock();
	printf("\nThe running time is:%.2f sec.", (timeEnd - timeBegin)/(double)CLK_TCK);
	
	cvNamedWindow("Origin Image", CV_WINDOW_AUTOSIZE);
	cvShowImage("Origin Image", pSrcImg);
	cvNamedWindow("Saliency Map",CV_WINDOW_AUTOSIZE);
	cvShowImage("Saliency Map", pSaliencyMap);
	cvWaitKey(0);
	cvDestroyAllWindows();
	//cvReleaseImage(&pSaliencyMap);
	cvReleaseImage(&pSrcImg);
}
void TestInBatch()
{
	VAMToolbox vam;
	IplImage *pSrcImg, *pSaliencyMap;
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
		timeEnd = clock();
		printf("\nThe running time is:%.2f sec.", (timeEnd - timeBegin)/(double)CLK_TCK);

		cvNamedWindow("Origin Image", CV_WINDOW_AUTOSIZE);
		cvShowImage("Origin Image", pSrcImg);
		cvNamedWindow("Saliency Map",CV_WINDOW_AUTOSIZE);
		cvShowImage("Saliency Map", pSaliencyMap);
		int k = cvWaitKey(0);
		if(k == 27){ //Esc
			return;
		}
		cvDestroyAllWindows();
		//cvReleaseImage(&pSaliencyMap);
		cvReleaseImage(&pSrcImg);
		vam.Release();
	}
}
