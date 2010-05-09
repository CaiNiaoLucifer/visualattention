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

class VAMToolbox
{
public:
	VAMToolbox();
	~VAMToolbox();

	/**
	 * 载入待处理的图片
	 * @param imagePath 图片存储的路径
	 * @return 载入成功返回0，失败返回-1
	 */
	int LoadImage(char* imagePath);

	/**
	 * 获取图像的大小
	 * @return 图像的大小
	 */
	CvSize& GetSize() const;

	/**
	 * 获取显著图
	 * @param
	 * @return 失败返回NULL
	 */
	IplImage* GetSaliencyMap();

	/**
	 * 获取强度特征的显著图
	 * @param
	 * @return 失败返回NULL
	 */
	IplImage* GetIntensityMap();

	/**
	 * 获取颜色特征的显著图
	 * @param
	 * @return 失败返回NULL
	 */
	IplImage* GetColorMap();

	/**
	 * 获取方向特征的显著图
	 * @param
	 * @return 失败返回NULL
	 */
	IplImage* GetOrientationMap();

	/**
	 * 获取注意力转换轨迹
	 * @param
	 * @return 失败返回NULL
	 */
	void GetVisualPath();

private:
	void BuildImgPyr();
	void BuildIntPyr();
	void BuildColPyr();
	void BuildOriPyr();

	void NormFeatMap(CvMat* src, CvMat* dst);

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

private:
	int			m_typeOfNormFeatMap;

	int			m_numOfPyrLevel;	//9
	int			m_numOfOri;			//4
	
	//Parameter of Gabor
	int			m_gaborSize;//9
	int			m_numOfGaborScale; //4
	float		m_gaborUL; //0.04
	float		m_gaborUH; //0.5
	
	CvSize		m_size;				//图片大小
	IplImage*	m_pOriginImg;		//原始图片
	IplImage*	m_pIntMap;
	IplImage*	m_pColorMap;
	IplImage*	m_pOriMap;	
	CvMat**		m_ppImgPyr;
	CvMat**		m_ppIntPyr;		//Intensity Pyramid
	CvMat**		m_ppColPyr;		//Color Pyramid
	CvMat**		m_ppOriPyr;		//Orientation Pyramid
};

#endif