/**
 * Itti&Koch的注意力模型的实现
 * 作者：袁宇，杨帆，侯贺曦
 * 时间：2010-05-01
 * 备注：
 */

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

#define _VAM_DEBUG_
#define _VAM_SHOW_ORI_FEATMAP_

class VAMToolbox
{
public:
	VAMToolbox();
	~VAMToolbox();
	
	/**
	 * 载入图像
	 * @param path 图像文件的路径
	 */
	IplImage* LoadImage(char* path);
	void LoadImage(IplImage* img);

	/**
	 * 获取显著图
	 * @param
	 * @return 失败返回NULL
	 */
	IplImage* GetSaliencyMap(IplImage* pSrcImg = NULL);

	/**
	 * 获取强度特征的显著图
	 * @param
	 * @return 失败返回NULL
	 */
	IplImage* GetSaliencyMapOfIntensity(IplImage* pSrcImg = NULL);

	/**
	 * 获取颜色特征的显著图
	 * @param
	 * @return 失败返回NULL
	 */
	IplImage* GetSaliencyMapOfColor(IplImage* pSrcImg = NULL);

	/**
	 * 获取方向特征的显著图
	 * @param
	 * @return 失败返回NULL
	 */
	IplImage* GetSaliencyMapOfOrientation(IplImage* pSrcImg = NULL);


	/**
	 * 获取注意力转换轨迹
	 * @param
	 * @return 失败返回NULL
	 */
	void GetVisualPath();

	/**
	 * 释放占用的资源
	 */
	void Release();

private:
	/**
	 * Get the saliency map of each feature
	 */
	CvMat* GetIntMap();
	CvMat* GetColMap();
	CvMat* GetOriMap();
	
	/**
	 * Build the pyramid of input image and each feature
	 */
	void		BuildImgPyr();
	CvMat**		BuildIntPyr();
	void		BuildColPyr(CvMat*** pppRpyr, CvMat*** pppGpyr, CvMat*** pppBpyr, CvMat*** pppYpyr);
	CvMat***	BuildOriPyr();
	void DecoupleHue(CvMat *rMat, CvMat *gMat,CvMat *bMat, CvMat* iMat, CvMat *dstMat, uchar type);
	
	/**
	 *  Calculate the feature map of each feature
	 */
	CvMat** CalcIntFeatMap(CvMat** ppIntPyr, int* numOfMaps);
	CvMat** CalcColFeatMap(CvMat** R,  CvMat** G, CvMat** B, CvMat** Y, int* numOfMaps);
	CvMat** CalcOriFeatMap(CvMat*** pppOriPyr, int *numOfMaps);

	/**
	 *  Function to do the normalize
	 */
	void VAMNormalize(CvMat* src, CvMat* dst);
	void NonLinearAmp(CvMat *src,CvMat *dst);
	void IterativeNorm(CvMat *src,CvMat *dst,int nIteration);
	void TrunConv(CvMat *src,CvMat *dst,CvMat *T);
	void TrunConvInRange(CvMat* src, CvMat* dst, CvMat* T, int rowStart, int rowEnd, int colStart, int colEnd);
	CvMat* GetDogFilter(double exSigma, double exC, double inhSigma, double inhC, int radius);
	void FindLocalMaxima(CvMat* src, double threshold, int* num, double* avg);

	/**
	 *  Function of gabor filter
	 */
	void MakeGaborFilter(CvMat *Gr, CvMat *Gi, int s, int n, double Ul, double Uh, int scale, int orientation, int flag);
	void GaborFilterImage(CvMat *src,CvMat *dst,int scale,int ori);

private:
	int			m_typeOfVAMNormalize;
	CvScalar	m_featWeight;

	int			m_numOfPyrLevel;	//9
	int			m_numOfOri;			//4
	
	//Parameter of Gabor
	int			m_gaborSize;//9
	int			m_numOfGaborScale; //4
	double		m_gaborUL; //0.04
	double		m_gaborUH; //0.5
	
	IplImage*	m_pOriginImg;		//原始图片
	CvMat*		m_pIntMap;
	CvMat*		m_pColorMap;
	CvMat*		m_pOriMap;	
	CvMat**		m_ppImgPyr;
	CvMat**		m_ppIntPyr;		//Intensity Pyramid
	CvMat*		m_pSaliencyMapOfInt;
	CvMat*		m_pSaliencyMapOfCol;
	CvMat*		m_pSaliencyMapOfOri;
};

#endif