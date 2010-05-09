/**
 * Itti&Koch��ע����ģ�͵�ʵ��
 * ���ߣ�Ԭ���������
 * ʱ�䣺2010-05-01
 * ��ע��
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
	 * ����������ͼƬ
	 * @param imagePath ͼƬ�洢��·��
	 * @return ����ɹ�����0��ʧ�ܷ���-1
	 */
	int LoadImage(char* imagePath);

	/**
	 * ��ȡͼ��Ĵ�С
	 * @return ͼ��Ĵ�С
	 */
	CvSize& GetSize() const;

	/**
	 * ��ȡ����ͼ
	 * @param
	 * @return ʧ�ܷ���NULL
	 */
	IplImage* GetSaliencyMap();

	/**
	 * ��ȡǿ������������ͼ
	 * @param
	 * @return ʧ�ܷ���NULL
	 */
	IplImage* GetIntensityMap();

	/**
	 * ��ȡ��ɫ����������ͼ
	 * @param
	 * @return ʧ�ܷ���NULL
	 */
	IplImage* GetColorMap();

	/**
	 * ��ȡ��������������ͼ
	 * @param
	 * @return ʧ�ܷ���NULL
	 */
	IplImage* GetOrientationMap();

	/**
	 * ��ȡע����ת���켣
	 * @param
	 * @return ʧ�ܷ���NULL
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
	
	CvSize		m_size;				//ͼƬ��С
	IplImage*	m_pOriginImg;		//ԭʼͼƬ
	IplImage*	m_pIntMap;
	IplImage*	m_pColorMap;
	IplImage*	m_pOriMap;	
	CvMat**		m_ppImgPyr;
	CvMat**		m_ppIntPyr;		//Intensity Pyramid
	CvMat**		m_ppColPyr;		//Color Pyramid
	CvMat**		m_ppOriPyr;		//Orientation Pyramid
};

#endif