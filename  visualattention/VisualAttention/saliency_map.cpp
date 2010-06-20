
#include "saliency_map.h"

VAMToolbox::VAMToolbox()
{
	m_typeOfVAMNormalize = VAM_ITERATIVE;
	
	m_numOfPyrLevel = 9;	
	m_numOfOri = 4;		

	m_featWeight.val[0] = 1/3.0;
	m_featWeight.val[1] = 1/3.0;
	m_featWeight.val[2] = 1/3.0;

	m_gaborSize = 9; 
	m_numOfGaborScale = 4; 
	m_gaborUL = 0.04; 
	m_gaborUH = 0.5;

	m_pOriginImg = NULL;
	m_ppImgPyr = NULL;
	m_ppIntPyr = NULL;
	m_pIntMap = NULL;
	m_pColorMap = NULL;
	m_pOriMap = NULL;
	m_pSaliencyMapOfInt = NULL;
	m_pSaliencyMapOfCol = NULL;
	m_pSaliencyMapOfOri = NULL;
}

VAMToolbox::~VAMToolbox()
{
	Release();
}

void VAMToolbox::Release()
{
	cvReleaseMat(&m_pIntMap);
	cvReleaseMat(&m_pColorMap);
	cvReleaseMat(&m_pOriMap);
	ReleaseBatch(&m_ppImgPyr, m_numOfPyrLevel);
	ReleaseBatch(&m_ppIntPyr, m_numOfPyrLevel);
	cvReleaseMatHeader(&m_pSaliencyMapOfInt);
	cvReleaseMatHeader(&m_pSaliencyMapOfCol);
	cvReleaseMatHeader(&m_pSaliencyMapOfOri);
}

IplImage* VAMToolbox::LoadImage(char* path)
{
	IplImage* img = cvLoadImage(path);
	if(img == NULL){
		return NULL;
	}
	LoadImage(img);
	return img;
}

void VAMToolbox::LoadImage(IplImage* img)
{
	if(m_pOriginImg != NULL){
		cvReleaseImage(&m_pOriginImg);
	}
	CvSize imgSize = cvGetSize(img);
	int minLen = cvCeil(pow(2.0, m_numOfPyrLevel-1));
	int minSrc = min(img->width, img->height);
	if(minSrc < minLen){
		double zoom = (double)minLen / (double)minSrc;
		imgSize.height = cvCeil(imgSize.height * zoom);
		imgSize.width = cvCeil(imgSize.width * zoom);
		IplImage* zoomImg = cvCreateImage(imgSize, img->depth, img->nChannels);
		assert(zoomImg != NULL);
		cvResize(img, zoomImg);
		m_pOriginImg = cvCreateImage(imgSize, IPL_DEPTH_32F, img->nChannels);
		assert(m_pOriginImg != NULL);
		cvConvertScale(zoomImg, m_pOriginImg, 1.0/255.0);
		cvReleaseImage(&zoomImg);
	}else{
		m_pOriginImg = cvCreateImage(imgSize, IPL_DEPTH_32F, img->nChannels);
		assert(m_pOriginImg != NULL);
		cvConvertScale(img, m_pOriginImg, 1.0/255.0);
	}
}

/**
 * 获取显著图
 * @param 
 * @return 失败返回NULL
 */
IplImage* VAMToolbox::GetSaliencyMap(IplImage* pSrcImg)
{
	if(pSrcImg != NULL){
		LoadImage(pSrcImg);
	}else if(m_pOriginImg == NULL){
		return NULL;
	}
	CvMat *pDstMap;

	BuildImgPyr();
	m_pSaliencyMapOfInt = GetIntMap();
	m_pSaliencyMapOfCol = GetColMap();
	m_pSaliencyMapOfOri = GetOriMap();

	pDstMap = cvCreateMat(m_pSaliencyMapOfInt->rows, m_pSaliencyMapOfInt->cols, CV_32FC1);
	cvAddWeighted(m_pSaliencyMapOfInt, m_featWeight.val[0], m_pSaliencyMapOfCol, m_featWeight.val[1], 0, pDstMap);
	cvAddWeighted(m_pSaliencyMapOfOri, m_featWeight.val[2], pDstMap, 1.0, 0, pDstMap);

	CvMat* pDstMap_oriSize = cvCreateMat(cvGetSize(m_pOriginImg).height, cvGetSize(m_pOriginImg).width, CV_32FC1);
	cvResize(pDstMap, pDstMap_oriSize, CV_INTER_LINEAR);
	cvNormalize(pDstMap_oriSize, pDstMap_oriSize, 1.0, 0.0, CV_MINMAX, NULL);
	IplImage* pDstImg = MatToImage(pDstMap_oriSize);
	//cvReleaseMatHeader(&pDstMap_oriSize);
	return pDstImg;
}

IplImage* VAMToolbox::GetSaliencyMapOfIntensity(IplImage* pSrcImg)
{
	if(m_pSaliencyMapOfInt == NULL){
		LoadImage(pSrcImg);
		BuildImgPyr();
		m_pSaliencyMapOfInt = GetIntMap();
	}
	CvMat* pDstMap_oriSize = cvCreateMat(cvGetSize(m_pOriginImg).height, cvGetSize(m_pOriginImg).width, CV_32FC1);
	cvResize(m_pSaliencyMapOfInt, pDstMap_oriSize, CV_INTER_LINEAR);
	cvNormalize(pDstMap_oriSize, pDstMap_oriSize, 1.0, 0.0, CV_MINMAX, NULL);
	IplImage* pDstImg = MatToImage(pDstMap_oriSize);
	//cvReleaseMatHeader(&pDstMap_oriSize);
	return pDstImg;
}

IplImage* VAMToolbox::GetSaliencyMapOfColor(IplImage* pSrcImg)
{
	if(m_pSaliencyMapOfCol == NULL){
		LoadImage(pSrcImg);
		BuildImgPyr();
		m_pSaliencyMapOfCol = GetColMap();
	}
	CvMat* pDstMap_oriSize = cvCreateMat(cvGetSize(m_pOriginImg).height, cvGetSize(m_pOriginImg).width, CV_32FC1);
	cvResize(m_pSaliencyMapOfCol, pDstMap_oriSize, CV_INTER_LINEAR);
	cvNormalize(pDstMap_oriSize, pDstMap_oriSize, 1.0, 0.0, CV_MINMAX, NULL);
	IplImage* pDstImg = MatToImage(pDstMap_oriSize);
	//cvReleaseMatHeader(&pDstMap_oriSize);
	return pDstImg;
}

IplImage* VAMToolbox::GetSaliencyMapOfOrientation(IplImage* pSrcImg)
{
	if(m_pSaliencyMapOfOri == NULL){
		LoadImage(pSrcImg);
		BuildImgPyr();
		m_pSaliencyMapOfOri = GetOriMap();
	}
	CvMat* pDstMap_oriSize = cvCreateMat(cvGetSize(m_pOriginImg).height, cvGetSize(m_pOriginImg).width, CV_32FC1);
	cvResize(m_pSaliencyMapOfOri, pDstMap_oriSize, CV_INTER_LINEAR);
	cvNormalize(pDstMap_oriSize, pDstMap_oriSize, 1.0, 0.0, CV_MINMAX, NULL);
	IplImage* pDstImg = MatToImage(pDstMap_oriSize);
	//cvReleaseMatHeader(&pDstMap_oriSize);
	return pDstImg;
}

	/**
	 * 获取强度特征的显著图
	 * @param
	 * @return 失败返回NULL
	 */
CvMat* VAMToolbox::GetIntMap()
{
	int numOfMaps;
	CvMat** ppIntFeatMap;
	CvMat *pTempMat, *pSumMat;
	BuildIntPyr();
	ppIntFeatMap = CalcIntFeatMap(m_ppIntPyr, &numOfMaps);
	
	pTempMat = cvCreateMat(m_ppIntPyr[4]->rows, m_ppIntPyr[4]->cols, CV_32FC1);
	assert(pTempMat != NULL);
	pSumMat = cvCreateMat(m_ppIntPyr[4]->rows, m_ppIntPyr[4]->cols, CV_32FC1);
	assert(pSumMat != NULL);
	cvResize(ppIntFeatMap[0], pSumMat);
	for(int i=1; i < numOfMaps; i++){
		if(ppIntFeatMap[i]->rows == m_ppIntPyr[4]->rows){
			cvAdd(pSumMat, ppIntFeatMap[i], pSumMat);
		}else{
			cvResize(ppIntFeatMap[i], pTempMat, CV_INTER_LINEAR);
			cvAdd(pSumMat, pTempMat, pSumMat);
		}
	}
	VAMNormalize(pSumMat, pSumMat);
	cvReleaseMat(&pTempMat);
	ReleaseBatch(&ppIntFeatMap, numOfMaps);
	return pSumMat;
}

/**
 * 获取颜色特征的显著图
 * @param
 * @return 失败返回NULL
 */
CvMat* VAMToolbox::GetColMap()
{
	int numOfMaps;
	CvMat** ppColFeatMap;
	CvMat *pTempMat, *pSumMat;
	CvMat **Rpyr, **Gpyr, **Bpyr, **Ypyr;
	
	if(m_ppIntPyr == NULL){
		BuildIntPyr();
	}
	BuildColPyr(&Rpyr, &Gpyr, &Bpyr, &Ypyr);
	ppColFeatMap = CalcColFeatMap(Rpyr, Gpyr, Bpyr, Ypyr, &numOfMaps);
	
	pTempMat = cvCreateMat(m_ppIntPyr[4]->rows, m_ppIntPyr[4]->cols, CV_32FC1);
	assert(pTempMat != NULL);
	pSumMat = cvCreateMat(m_ppIntPyr[4]->rows, m_ppIntPyr[4]->cols, CV_32FC1);
	assert(pSumMat != NULL);
	cvResize(ppColFeatMap[0], pSumMat);
	for(int i=1; i<numOfMaps; i++){
		if(ppColFeatMap[i]->rows == m_ppIntPyr[4]->rows){
			cvAdd(pSumMat, ppColFeatMap[i], pSumMat);
		}else{
			cvResize(ppColFeatMap[i], pTempMat, CV_INTER_LINEAR);
			cvAdd(pSumMat, pTempMat, pSumMat);
		}
	}
	VAMNormalize(pSumMat, pSumMat);
	cvReleaseMat(&pTempMat);
	ReleaseBatch(&ppColFeatMap, numOfMaps);
	ReleaseBatch(&Rpyr, m_numOfPyrLevel);
	ReleaseBatch(&Gpyr, m_numOfPyrLevel);
	ReleaseBatch(&Bpyr, m_numOfPyrLevel);
	ReleaseBatch(&Ypyr, m_numOfPyrLevel);
	return pSumMat;
}

/**
 * 获取方向特征的显著图
 * @param
 * @return 失败返回NULL
 */
CvMat* VAMToolbox::GetOriMap()
{
	int numOfMaps;
	CvMat** ppOriFeatMap;
	CvMat ***pppOriPyr;
	CvMat *pTempMat, *pSumMat;

	if(m_ppIntPyr == NULL){
		BuildIntPyr();
	}
	pppOriPyr = BuildOriPyr();

	ppOriFeatMap = CalcOriFeatMap(pppOriPyr, &numOfMaps);

	pTempMat = cvCreateMat(m_ppIntPyr[4]->rows, m_ppIntPyr[4]->cols, CV_32FC1);
	assert(pTempMat != NULL);
	pSumMat = cvCreateMat(m_ppIntPyr[4]->rows, m_ppIntPyr[4]->cols, CV_32FC1);
	assert(pSumMat != NULL);
	cvResize(ppOriFeatMap[0], pSumMat);
	for(int i=1; i<numOfMaps; i++){
		if(ppOriFeatMap[i]->rows == m_ppIntPyr[4]->rows){
			cvAdd(pSumMat, ppOriFeatMap[i], pSumMat);
		}else{
			cvResize(ppOriFeatMap[i], pTempMat, CV_INTER_LINEAR);
			cvAdd(pSumMat, pTempMat, pSumMat);
		}
	}
	VAMNormalize(pSumMat, pSumMat);
	cvReleaseMat(&pTempMat);
	ReleaseBatch(&ppOriFeatMap, numOfMaps);
	for(int i=0; i<m_numOfOri; i++){
		ReleaseBatch(&pppOriPyr[i], m_numOfPyrLevel);
	}
	ReleaseBatch(&pppOriPyr, m_numOfOri);
	return pSumMat; 
}

	/**
	 * 获取注意力转换轨迹
	 * @param
	 * @return 失败返回NULL
	 */
void VAMToolbox::GetVisualPath()
{
	return;
}

void VAMToolbox::BuildImgPyr()
{
	assert(m_pOriginImg != NULL);
	m_ppImgPyr = (CvMat**)calloc(m_numOfPyrLevel, sizeof(CvMat*));
	assert(m_ppImgPyr != NULL);

	//The first level of pyramid is origin image
	m_ppImgPyr[0] = cvCreateMatHeader(m_pOriginImg->height, m_pOriginImg->width, CV_32FC3);
	assert(m_ppImgPyr[0] != NULL);
	cvGetMat(m_pOriginImg, m_ppImgPyr[0]);
	CvSize levelSize = cvGetSize(m_pOriginImg);
	for(int i=1; i < m_numOfPyrLevel; i++){
		levelSize.height /= 2;
		levelSize.width  /= 2;
		m_ppImgPyr[i] = cvCreateMat(levelSize.height, levelSize.width, CV_32FC3);
		assert(m_ppImgPyr[i] != NULL);
		cvResize(m_ppImgPyr[i-1], m_ppImgPyr[i], CV_INTER_LINEAR);
	}
}

CvMat** VAMToolbox::BuildIntPyr()
{
	if(m_ppImgPyr == NULL){
		BuildImgPyr();
	}
	m_ppIntPyr = (CvMat**)calloc(m_numOfPyrLevel, sizeof(CvMat*));
	assert(m_ppIntPyr != NULL);
	for(int i=0; i < m_numOfPyrLevel; i++)  {
		m_ppIntPyr[i] = cvCreateMat(m_ppImgPyr[i]->rows, m_ppImgPyr[i]->cols, CV_32FC1);
		assert(m_ppIntPyr[i] != NULL);
		cvCvtColor(m_ppImgPyr[i], m_ppIntPyr[i], CV_BGR2GRAY);
	}
	return m_ppIntPyr;
}

void VAMToolbox::DecoupleHue(CvMat *rMat, CvMat *gMat,CvMat *bMat, CvMat* iMat, CvMat *dstMat, uchar type)
{
	int rows = rMat->rows;
	int cols = rMat->cols;

	int m, n;
	double val, r, g, b, max, threshold;
	cvMinMaxLoc(iMat, NULL, &max, NULL, NULL, NULL);
	threshold = 0.1 * max;

	switch(type)
	{
	case 'R':
		for(m=0; m < rMat->rows; m++){
			for(n=0; n < rMat->cols; n++){
				if(cvmGet(iMat, m, n) < threshold){
					val = 0;
				}else{
					r = cvmGet(rMat, m, n);
					g = cvmGet(gMat, m, n);
					b = cvmGet(bMat, m, n);
					val = r-(g+b)/2;
				}
				cvmSet(dstMat, m, n, val);
			}
		}
		break;
	case 'G':
		for(m=0; m < rows; m++){
			for(n=0; n < cols; n++){
				if(cvmGet(iMat, m, n) < threshold){
					val = 0;
				}else{
					r = cvmGet(rMat, m, n);
					g = cvmGet(gMat, m, n);
					b = cvmGet(bMat, m, n);
					val = g-(r+b)/2;
				}
				cvmSet(dstMat, m, n, val);
			}
		}
		break;
	case 'B':
		for(m=0; m < rows; m++){
			for(n=0; n < cols; n++){
				if(cvmGet(iMat, m, n) < threshold){
					val = 0;
				}else{
					r = cvmGet(rMat, m, n);
					g = cvmGet(gMat, m, n);
					b = cvmGet(bMat, m, n);
					val = b-(r+g)/2;
				}
				cvmSet(dstMat, m, n, val);
			}
		}
		break;
	case 'Y':
		for(m=0; m < rows; m++){
			for(n=0; n < cols; n++){
				if(cvmGet(iMat, m, n) < threshold){
					val = 0;
				}else{
					r = cvmGet(rMat, m, n);
					g = cvmGet(gMat, m, n);
					b = cvmGet(bMat, m, n);
					val = r+g-2*(abs(r-g)+b);
				}
				cvmSet(dstMat, m, n, val);
			}
		}
		break;
	default:
		return;
	}
}

void VAMToolbox::BuildColPyr(CvMat*** pppRpyr, CvMat*** pppGpyr, CvMat*** pppBpyr, CvMat*** pppYpyr)
{
	CvMat** &R = *pppRpyr;
	CvMat** &G = *pppGpyr;
	CvMat** &B = *pppBpyr;
	CvMat** &Y = *pppYpyr;

	R = (CvMat**)calloc(m_numOfPyrLevel, sizeof(CvMat*));
	assert(R != NULL);
	G = (CvMat**)calloc(m_numOfPyrLevel, sizeof(CvMat*));
	assert(G != NULL);
	B = (CvMat**)calloc(m_numOfPyrLevel, sizeof(CvMat*));
	assert(B != NULL);
	Y = (CvMat**)calloc(m_numOfPyrLevel, sizeof(CvMat*));
	assert(Y != NULL);
	
	int rows, cols;
	CvMat *r, *g, *b;
	for(int i=0; i < m_numOfPyrLevel; i++){
		rows = m_ppImgPyr[i]->rows;
		cols = m_ppImgPyr[i]->cols;

		r = cvCreateMat(rows, cols, CV_32FC1);
		assert(r != NULL);
		g = cvCreateMat(rows, cols, CV_32FC1);
		assert(g != NULL);
		b = cvCreateMat(rows, cols, CV_32FC1);
		assert(b != NULL);
		R[i] = cvCreateMat(rows, cols, CV_32FC1);
		assert(R[i] != NULL);
		G[i] = cvCreateMat(rows, cols, CV_32FC1);
		assert(G[i] != NULL);
		B[i] = cvCreateMat(rows, cols, CV_32FC1);
		assert(B[i] != NULL);
		Y[i] = cvCreateMat(rows, cols, CV_32FC1);
		assert(Y[i] != NULL);

		cvSplit(m_ppImgPyr[i], b, g, r, NULL);
		DecoupleHue(r, g, b, m_ppIntPyr[i], R[i], 'R');
		DecoupleHue(r, g, b, m_ppIntPyr[i], G[i], 'G');
		DecoupleHue(r, g, b, m_ppIntPyr[i], B[i], 'B');
		DecoupleHue(r, g, b, m_ppIntPyr[i], Y[i], 'Y');
		
		cvReleaseMat(&r);
		cvReleaseMat(&g);
		cvReleaseMat(&b);
	}
}

CvMat*** VAMToolbox::BuildOriPyr()
{
	CvMat*** pppOriPyr = (CvMat***)calloc(m_numOfOri, sizeof(CvMat**));
	assert(pppOriPyr != NULL);
	for(int i=0; i < m_numOfPyrLevel; i++){
		pppOriPyr[i] = (CvMat**)calloc(m_numOfPyrLevel, sizeof(CvMat*));
		assert(pppOriPyr[i] != NULL);
	}
	for(int i=0; i < m_numOfOri; i++){
		for(int j=0; j < m_numOfPyrLevel; j++){
			pppOriPyr[i][j] = cvCreateMat(m_ppIntPyr[j]->rows, m_ppIntPyr[j]->cols, CV_32FC1);
			assert(pppOriPyr[i][j] != NULL);
			GaborFilterImage(m_ppIntPyr[j], pppOriPyr[i][j], j, i);
			//DispMat(pppOriPyr[i][j]);
		}	
	}
	return pppOriPyr;
}

CvMat** VAMToolbox::CalcIntFeatMap(CvMat** ppIntPyr, int* numOfMaps)
{
	CvMat** ppIntFeatMap;
	CvMat *tempMap;
	int rows, cols;

	ppIntFeatMap = (CvMat**)calloc(6, sizeof(CvMat*));
	assert(ppIntFeatMap != NULL);

	for(int i=2;i<5;i++){
		rows = ppIntPyr[i]->rows;
		cols = ppIntPyr[i]->cols;
		tempMap = cvCreateMat(rows, cols, CV_32FC1);

		ppIntFeatMap[2*i-4] = cvCreateMat(rows, cols, CV_32FC1);//I:2-5
		cvResize(ppIntPyr[i+3], tempMap);
		cvAbsDiff(ppIntPyr[i], tempMap,ppIntFeatMap[2*i-4]);

		ppIntFeatMap[2*i-3] = cvCreateMat(rows, cols, CV_32FC1);//I:2-6
		cvResize(ppIntPyr[i+4], tempMap);
		cvAbsDiff(ppIntPyr[i], tempMap,ppIntFeatMap[2*i-3]);
	}
	cvReleaseMat(&tempMap);

	for(int i=0; i<6; i++){
		VAMNormalize(ppIntFeatMap[i], ppIntFeatMap[i]);
	}

	*numOfMaps = 6;
	return ppIntFeatMap;
}

CvMat** VAMToolbox::CalcColFeatMap(CvMat** R, CvMat** G, CvMat** B, CvMat** Y, int* numOfMaps)
{
	int rows, cols;
	CvMat **ppColFeatMap, **ppRGFeatMap, **ppBYFeatMap;
	CvMat *tempMap1, *tempMap2, *tempMap3, *tempMap4;

	ppColFeatMap = (CvMat**)calloc(12, sizeof(CvMat*));
	assert(ppColFeatMap != NULL);
	ppRGFeatMap = ppColFeatMap;
	ppBYFeatMap = ppColFeatMap + 6;
	for(int i=2; i<5; i++){
		rows = R[i]->rows;
		cols = R[i]->cols;
		tempMap1 = cvCreateMat(rows, cols, CV_32FC1);
		tempMap2 = cvCreateMat(rows, cols, CV_32FC1);
		tempMap3 = cvCreateMat(R[i+3]->rows, R[i+3]->cols, CV_32FC1);
		tempMap4 = cvCreateMat(R[i+4]->rows, R[i+4]->cols, CV_32FC1);

		ppRGFeatMap[2*i-4] = cvCreateMat(rows, cols, CV_32FC1); //RG:2-5
		cvSub(R[i], G[i], tempMap1);
		cvSub(R[i+3], G[i+3], tempMap3);
		cvResize(tempMap3, tempMap2);
		cvAbsDiff(tempMap1, tempMap2, ppRGFeatMap[2*i-4]);

		ppRGFeatMap[2*i-3] = cvCreateMat(rows, cols, CV_32FC1); //RG:2-6
		cvSub(R[i], G[i], tempMap1);
		cvSub(R[i+4], G[i+4], tempMap4);
		cvResize(tempMap4,tempMap2);
		cvAbsDiff(tempMap1, tempMap2, ppRGFeatMap[2*i-3]);

		ppBYFeatMap[2*i-4] = cvCreateMat(rows, cols, CV_32FC1); //BY:2-5
		cvSub(B[i], Y[i], tempMap1);
		cvSub(B[i+3], Y[i+3], tempMap3);
		cvResize(tempMap3, tempMap2);
		cvAbsDiff(tempMap1, tempMap2, ppBYFeatMap[2*i-4]);

		ppBYFeatMap[2*i-3] = cvCreateMat(rows, cols, CV_32FC1); //BY:2-6
		cvSub(B[i], Y[i], tempMap1);
		cvSub(B[i+4], Y[i+4], tempMap4);
		cvResize(tempMap4, tempMap2);
		cvAbsDiff(tempMap1, tempMap2, ppBYFeatMap[2*i-3]);
	}
	cvReleaseMat(&tempMap1);
	cvReleaseMat(&tempMap2);
	cvReleaseMat(&tempMap3);
	cvReleaseMat(&tempMap4);

	for(int i=0; i<12; i++){
		VAMNormalize(ppColFeatMap[i], ppColFeatMap[i]);
	}
	
	*numOfMaps = 12;
	return ppColFeatMap;
}

CvMat** VAMToolbox::CalcOriFeatMap(CvMat*** pppOriPyr, int *numOfMaps)
{
	CvMat** ppOriFeatMap;
	CvMat *tempMap;
	int rows, cols, pos;
	
	ppOriFeatMap = (CvMat**)calloc(24, sizeof(CvMat*));
	assert(ppOriFeatMap != NULL);

	for(int i=2; i<5; i++){
		rows = pppOriPyr[0][i]->rows;
		cols = pppOriPyr[0][i]->cols;
		tempMap = cvCreateMat(rows, cols, CV_32FC1);
		assert(tempMap != NULL);
		for(int j=0; j < m_numOfOri; j++){
			pos = j*6 + 2*i - 4;
			ppOriFeatMap[pos] = cvCreateMat(rows, cols, CV_32FC1);//I:2-5
			assert(ppOriFeatMap[pos] != NULL);
			cvResize(pppOriPyr[j][i+3], tempMap);
			cvAbsDiff(pppOriPyr[j][i],  tempMap, ppOriFeatMap[pos]);

			pos = j*6 + 2*i - 3;
			ppOriFeatMap[pos] = cvCreateMat(rows, cols, CV_32FC1);//I:2-6
			assert(ppOriFeatMap[pos] != NULL);
			cvResize(pppOriPyr[j][i+4], tempMap);
			cvAbsDiff(pppOriPyr[j][i], tempMap, ppOriFeatMap[pos]);
		}
		cvReleaseMat(&tempMap);
	}
	for(int i=0; i<24; i++){
		VAMNormalize(ppOriFeatMap[i], ppOriFeatMap[i]);
#ifdef _VAM_SHOW_ORI_FEATMAP_
		CvMat* mm = cvCreateMat(m_pOriginImg->height, m_pOriginImg->width, CV_32FC1);
		cvResize(ppOriFeatMap[i], mm);
		DispMat(mm);
		cvReleaseMat(&mm);
#endif		
	}
	*numOfMaps = 24;
	return ppOriFeatMap;
}

void VAMToolbox::VAMNormalize(CvMat* src, CvMat* dst)
{
	switch(m_typeOfVAMNormalize){
		case VAM_NAVIE_SUM:
			cvNormalize(src, dst, 1.0, 0.0, CV_MINMAX, NULL);
			break;
		case VAM_LINEAR_COMBINATIOIN:
			break;
		case VAM_NON_LINEAR_AMPLIFICATION:
			NonLinearAmp(src, dst);
			break;
		case VAM_ITERATIVE:
			IterativeNorm(src, dst, 10);
			break;
		default:
			break;
	}
}

//DoGFilter
CvMat* VAMToolbox::GetDogFilter(double exSigma, double exC, double inhSigma, double inhC, int radius)
{
	int x, y, width;
	double exParm1, exParm2, inParm1, inParm2, val;

	exParm1 = 0.5 / (exSigma*exSigma);
	inParm1 = 0.5 / (inhSigma*inhSigma);
	exParm2 = exC * exC * exParm1 / CV_PI;
	inParm2 = inhC * inhC * inParm1 / CV_PI;
	
	width = radius+radius+1;
	CvMat *T = cvCreateMat(width, width, CV_32FC1);
	assert(T != NULL);
	for(x = -radius; x <= radius; x++){
		for(y = -radius; y <= radius; y++){
			val = exParm2 * exp(-(x*x + y*y) * exParm1) 
				- inParm2 * exp(-(x*x + y*y) * inParm1);
			cvmSet(T, x + radius, y + radius, val);
		}
	}
	return T;
}
//Iterative Localized Interations
void VAMToolbox::IterativeNorm(CvMat *src, CvMat *dst, int nIteration)
{
	int sz, width, radius, i;
	double exSigma, inhSigma, exC, inhC;
	CvScalar constInh;

	sz = min(src->rows,src->cols);
	exSigma = 0.02*sz;
	inhSigma = 0.25*sz;
	exC = 0.5;
	inhC = 1.5;
	constInh.val[0] = 0.02;
	radius = sz/8;
	if(radius <= 0){
		radius = 1;
	}else if(radius > 4){
		radius = 4;
	}
	width = radius*2+1;

	CvMat *T = GetDogFilter(exSigma, exC, inhSigma, inhC, radius);
	CvMat *pResultMat = cvCloneMat(src);
	assert(pResultMat != NULL);
	cvNormalize(pResultMat, pResultMat, 1.0, 0.0, CV_MINMAX, NULL);
	CvMat *pTempMat = cvCreateMat(src->rows, src->cols, src->type);
	assert(pTempMat != NULL);
	for(i=0; i < nIteration; i++){
		cvCopy(pResultMat, pTempMat, NULL);
		TrunConv(pTempMat, pResultMat, T);
		cvAdd(pTempMat, pResultMat, pResultMat);
		cvSubS(pResultMat, constInh, pResultMat);
		cvThreshold(pResultMat, pResultMat, 0.0, 0, CV_THRESH_TOZERO);
	}
	cvCopy(pResultMat, dst, NULL);
	cvReleaseMat(&pResultMat);
}

//Truncated Filter
void VAMToolbox::TrunConv(CvMat *src, CvMat *dst, CvMat *T)
{
    cvFilter2D(src, dst, T);
 //   double minVal, maxVal;
	//cvMinMaxLoc(dst, &minVal, &maxVal, NULL, NULL, NULL);
	////printf("\nMinVal = %f, maxVal = %f", minVal, maxVal);*/
	//int radius = (T->rows-1)/2;
	//SetValInRectRange(dst, minVal, 0, 0, dst->cols, radius);
	//SetValInRectRange(dst, minVal, 0, dst->rows - radius, dst->cols, radius);
	//SetValInRectRange(dst, minVal, 0, radius, radius, dst->rows - 2*radius);
	//SetValInRectRange(dst, minVal, dst->cols - radius, radius, radius, dst->rows - 2*radius);
	

	//assert(src != NULL && dst != NULL && T != NULL);
	//CvMat* pResultMat = cvCreateMat(src->rows, src->cols, src->type);
	//assert(pResultMat != NULL);
	//
	//if(min(src->rows, src->cols) <= T->rows){ //the size of input mat is not larger the template
	//	TrunConvInRange(src, pResultMat, T, 0, src->rows-1, 0, src->cols-1);
	//}else{
	//	cvFilter2D(src, pResultMat, T);
	//	int radius = (T->rows-1)/2;
	//	TrunConvInRange(src, pResultMat, T, 0, radius - 1, 0, src->cols - 1);
	//	TrunConvInRange(src, pResultMat, T, src->rows - radius, src->rows - 1, 0, src->cols - 1);
	//	TrunConvInRange(src, pResultMat, T, radius, src->rows - radius - 1, 0, radius - 1);
	//	TrunConvInRange(src, pResultMat, T, radius, src->rows - radius - 1, 
	//		src->cols - radius - 1, src->cols - 1);
	//	
	//}
	//cvCopy(pResultMat, dst, NULL);
	//cvReleaseMat(&pResultMat);

	/*int rCen, cCen, r, c, radius;
	double TSum, overlapTSum, overlapMulSum;
	
	CvScalar tempTSum;
	tempTSum = cvSum(T);
	TSum = tempTSum.val[0];

	radius = (T->rows - 1) / 2;
	CvMat *pResultMat = cvCreateMat(src->rows, src->cols, src->type);
	assert(pResultMat != NULL);
	for(rCen=0; rCen < radius; rCen++){
		for(cCen=0; cCen < src->cols; cCen++){	
			overlapTSum = 0.0;
			overlapMulSum = 0.0;
			for(r = -radius; r <= radius; r++){
				for(c = -radius; c<= radius; c++){
					if(r + rCen < 0 || r + rCen >= src->rows
						|| c + cCen < 0 || c + cCen >= src->cols){
							continue;
					}
					overlapTSum += cvmGet(T, r + radius, c + radius);
					overlapMulSum += cvmGet(T, r + radius, c+ radius)
						* cvmGet(src, r + rCen, c + cCen);
				}
			}
			cvmSet(pResultMat, rCen, cCen, TSum * overlapMulSum / overlapTSum);	
		}
	}
	cvCopy(pResultMat, dst, NULL);
	cvReleaseMat(&pResultMat);*/
}

void VAMToolbox::TrunConvInRange(CvMat* src, CvMat* dst, CvMat* T, int rowStart, int rowEnd, int colStart, int colEnd)
{
	assert(src != dst);
	int rCen, cCen, r, c, radius;
	double TSum, overlapTSum, overlapMulSum;
	
	CvScalar tempTSum;
	tempTSum = cvSum(T);
	TSum = tempTSum.val[0];

	radius = (T->rows - 1) / 2;
	for(rCen = rowStart; rCen <= rowEnd; rCen++){
		for(cCen = colStart; cCen <= colEnd; cCen++){	
			overlapTSum = 0.0;
			overlapMulSum = 0.0;
			for(r = -radius; r <= radius; r++){
				for(c = -radius; c<= radius; c++){
					if(r + rCen < 0 || r + rCen >= src->rows
						|| c + cCen < 0 || c + cCen >= src->cols){
							continue;
					}
					overlapTSum += cvmGet(T, r + radius, c + radius);
					overlapMulSum += (cvmGet(T, r + radius, c+ radius) * cvmGet(src, r + rCen, c + cCen));
				}
			}
			cvmSet(dst, rCen, cCen, TSum * overlapMulSum / overlapTSum);	
		}
	}
}
//用全局的非线性放大法进行图像的正规化操作
void VAMToolbox::NonLinearAmp(CvMat *src,CvMat *dst)
{
	int num;
	double avg, threshold = 0.1;
	cvNormalize(src, dst, 1.0, 0.0, CV_MINMAX, NULL);
	FindLocalMaxima(dst, threshold, &num, &avg);
	
	if(num > 1){
		double parm = (1.0 - avg) * (1.0 - avg);
		cvScale(dst, dst, parm, 0);
	}
}

void VAMToolbox::FindLocalMaxima(CvMat* src, double threshold, int* num, double* avg)
{
	double sum = 0.0;
	double val, temp;
	int lmaxCnt = 0;
	int i, j, m, n;
	bool flag = false;
	//ignore the pixel at boarder
	for(i=1; i < src->rows-1; i++){
		for(j=1; j < src->cols-1; j++){
			flag = true;
			val = cvmGet(src, i, j);
			if(val < threshold){
				continue;
			}
			for(m=-1; flag && m <= 1; m++){ //compare with the neighbor
				for(n=-1; flag && n <= 1; n++){
					temp = cvmGet(src, i+m, j+n);
					if(m !=0 && n != 0 && val < temp){
						flag = false;
					}
					
				}
			}
			if(flag){
				lmaxCnt++;
				sum += val;
				j++;  //the next pixel is absolutely is not maxima 
			}
		}
	}
	*avg = sum/lmaxCnt;
	*num = lmaxCnt;
}
//------------------------------------------------------------------------------------------------------
//The Gabor function generates a Gabor filter with the selected index 's' and 'n' (scale and orientation, 
//respectively) from a Gabor filter bank. This filter bank is designed by giving the range of spatial 
//frequency (Uh and Ul) and the total number of scales and orientations used to partition the spectrum. 
//
//The returned filter is stored in 'Gr' (real part) and 'Gi' (imaginary part).
//--------------------------------------------------------------------------------------------------------
void VAMToolbox::MakeGaborFilter(CvMat *Gr, CvMat *Gi, int s, int n, double Ul, double Uh, int scale, int orientation, int type)
{
	/*float stdDev, elongation, angle, omega, filterPeriod;
	int filterSize;

	float major_stddev, minor_stddev, max_stddev, rtDeg, co, si, major_sigq, minor_sigq;
	int sz;

	major_stddev = stdDev;
	minor_stddev = major_stddev * elongation;
	max_stddev = max(major_stddev, minor_stddev);

	sz = filterSize;
	if(sz == -1){
		sz = ceil(max_stddev*sqrt(10));
	}else{
		sz = floor(sz/2); 
	}

	rtDeg = CV_PI / 180.0 * angle;
	omega = 2 * CV_PI / filterPeriod;
	co = cos(rtDeg);
	si = -sin(rtDeg);
	major_sigq = 2 * major_stddev * major_stddev;
	minor_sigq = 2 * minor_stddev * minor_stddev;

	int vlen = 2*sz + 1;
	float vco = vec * co
		vec = [-sz:sz];
	vlen = length(vec);
	vco = vec*co;
	vsi = vec*si;

	major = repmat(vco',1,vlen) + repmat(vsi,vlen,1);
		major2 = major.^2;
	minor = repmat(vsi',1,vlen) - repmat(vco,vlen,1);
		minor2 = minor.^2;

	phase0 = exp(- major2 / major_sigq - minor2 / minor_sigq);

	% create the actual filters
		for p = 1:length(gaborParams.phases)
			psi = pi / 180 * gaborParams.phases(p);
	result = cos(omega * major + psi) .* phase0;

	% enforce disk shape?
		if (makeDisk)
			result((major2+minor2) > (gaborParams.filterSize/2)^2) = 0;
	end

		% normalization
		result = result - mean(result(:));
	filter(:,:,p) = result / sqrt(sum(result(:).^2));
	end*/

	double base, a, u0, z, Uvar, Vvar, Xvar, Yvar, X, Y, G, t1, t2, m;
	int x, y, side;
	double temp;

	base = Uh/Ul;
	a = pow(base, 1.0/(double)(scale-1));

	u0 = Uh/pow(a, (double) scale-s);

	Uvar = (a-1.0)*u0/((a+1.0)*sqrt(2.0*log(2.0)));

	z = -2.0*log(2.0)*(Uvar*Uvar)/u0;
	Vvar = tan(CV_PI/(2*orientation))*(u0+z)/sqrt(2.0*log(2.0)-z*z/(Uvar*Uvar));

	Xvar = 1.0/(2.0*CV_PI*Uvar);
	Yvar = 1.0/(2.0*CV_PI*Vvar);

	t1 = cos(CV_PI/orientation*(n-1.0));
	t2 = sin(CV_PI/orientation*(n-1.0));

	side = (int) (Gr->height-1)/2;

	for (x=0;x<2*side+1;x++) {
		for (y=0;y<2*side+1;y++) {
			X = (double) (x-side)*t1+ (double) (y-side)*t2;
			Y = (double) -(x-side)*t2+ (double) (y-side)*t1;
			G = 1.0/(2.0*CV_PI*Xvar*Yvar)*pow(a, (double) scale-s)*exp(-0.5*((X*X)/(Xvar*Xvar)+(Y*Y)/(Yvar*Yvar)));
			cvmSet(Gr,x,y,G*cos(2.0*CV_PI*u0*X));
			cvmSet(Gi,x,y,G*sin(2.0*CV_PI*u0*X));
		}
	}
	//if flag = 1, then remove the DC from the filter 
		if (type == 1) {
			m = 0;
			for (x=0;x<2*side+1;x++)
				for (y=0;y<2*side+1;y++)
					m += cvmGet(Gr,x,y);

			m /= pow((double) 2.0*side+1, 2.0);

			for (x=0;x<2*side+1;x++){
				for (y=0;y<2*side+1;y++){
					temp=cvmGet(Gr,x,y);
					temp -= m;
					cvmSet(Gr,x,y,temp);
				}
			}
		}	
}

void VAMToolbox::GaborFilterImage(CvMat *src, CvMat *dst, int scale, int ori)
{
	CvMat *Gr,*Gi;
	Gr = cvCreateMat(m_gaborSize, m_gaborSize, CV_32FC1);
	Gi = cvCreateMat(m_gaborSize, m_gaborSize,CV_32FC1);
	MakeGaborFilter(Gr, Gi, scale, ori, m_gaborUL, m_gaborUH, m_numOfGaborScale, m_numOfOri, 1);
	cvFilter2D(src,dst,Gr,cvPoint(-1,-1));
	//cvNormalize(dst, dst, 1, 0, CV_MINMAX, NULL);
}
