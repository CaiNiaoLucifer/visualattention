

#include <stdio.h>
#include <math.h>
#include "saliency_map.h"
#include <time.h>
using namespace std;

int main()
{
	VAMToolbox vam;
	IplImage *pSrcImg, *pSaliencyMap;
	char imgName[64];
	float timeBegin, timeEnd;
	printf("*************************************************\n");
	printf("       Visual Attention Model Test\n");
	printf("*************************************************\n");
	while(TRUE){
		printf("\nPlease type the image name,then you will get the saliency map.\n");
		printf("Image Name:  ");
		scanf("%s",imgName);
		pSrcImg = cvLoadImage(imgName);
		if(pSrcImg == NULL){
			printf("\n>>>>Fail to Load Image: %s!!\n", imgName);
			system("pause");
			continue;
		}
		timeBegin = clock();
		pSaliencyMap = vam.GetSaliencyMap(pSrcImg);
		timeEnd = clock();
		printf("\nThe running time is:%f (ms)", timeEnd - timeBegin);
		
		cvNamedWindow("Saliency Map",CV_WINDOW_AUTOSIZE);
		cvShowImage("Saliency Map",pSaliencyMap);
		cvWaitKey(0);
		cvDestroyAllWindows();
		cvReleaseImage(&pSaliencyMap);
		cvReleaseImage(&pSrcImg);
	}
	return 0;
}
