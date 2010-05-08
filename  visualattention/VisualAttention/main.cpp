

#include <stdio.h>
#include <math.h>
#include "saliency_map.h"
#include <time.h>
using namespace std;
using namespace cv;

int main()
{
	int i=0;
	IplImage *img;
	CvSize pyrSize;
	CvMat *saliencyMap;
	CvMat *intensityMap,*colorMap,*orientationMap,*tempMap;
	CvMat *pyrMat[9];
	char imgName[40];

	VAMToolbox vam;

	float timeCntBegin;
	float timeCntEnd;

	printf("*************************************************\n");
	printf("       Visual Attention Model Test\n");
	printf("*************************************************\n");
	while(TRUE)
	{
		printf("\nPlease type the image name,then you will get the saliency map.\n");
		printf("Image Name:  ");
		scanf("%s",imgName);
		if(vam.LoadImage(imgName) == -1)
		{
			printf("\n>>>>Fail to Load Image!!\n");
			system("pause");
			continue;
		}
		timeCntBegin = clock();
		pyrSize.height = vam.GetSize().height;
		pyrSize.width = vam.GetSize().width;

		for(i=0;i<=8;i++)
		{
			pyrMat[i]=cvCreateMatHeader(pyrSize.height,pyrSize.width,CV_8UC3);
			pyrSize.height /= 2;
			pyrSize.width /= 2;
		}

		CreateImagePyramid(img,pyrMat);	
		intensityMap = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
		orientationMap = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
		colorMap = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
		saliencyMap = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
		tempMap = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
		CompIntensity(intensityMap,pyrMat);
		CompColor(colorMap,pyrMat);
		CompOrientation(orientationMap,pyrMat);
		cvAdd(intensityMap,colorMap,saliencyMap);
		cvAdd(saliencyMap,orientationMap,tempMap);
		cvConvertScale(tempMap,saliencyMap,1.0/3.0,0);
		NonLinearAmp(saliencyMap,saliencyMap);
		cvNormalize(saliencyMap,saliencyMap,0,1,CV_MINMAX,NULL);

		timeCntEnd = clock();
		printf("\nThe running time is:%f (ms)",timeCntEnd-timeCntBegin);
		DispImageMat(img,saliencyMap);
		//-----------------------------------------------------------------	
		for(i=0;i<=8;i++)   //ÊÍ·Å×ÊÔ´
		{
			cvReleaseMat(&pyrMat[i]);
		}
		cvReleaseMat(&tempMap);
		cvReleaseMat(&saliencyMap);
		cvReleaseMat(&intensityMap);
		cvReleaseMat(&colorMap);
		cvReleaseImage(&img);
	}
	return 0;
}
