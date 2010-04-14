
#include "saliency_map.h"

void CreateImagePyramid(IplImage *img,CvMat *pyrMat[])
{
	int i;
	IplImage *pyrImage[9];
	pyrImage[0]=cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
	cvCopy(img,pyrImage[0]);

	CvSize pyrSize=cvGetSize(img);
	for(i=1;i<=8;i++)
	{
		pyrSize.height /= 2;
		pyrSize.width  /= 2;
		pyrImage[i]=cvCreateImage(pyrSize,IPL_DEPTH_8U,3);
		cvPyrDown(pyrImage[i-1],pyrImage[i],CV_GAUSSIAN_5x5);
	}
	for(i=0;i<=8;i++)
	{
		cvGetMat(pyrImage[i],pyrMat[i]);
		cvReleaseImageHeader(&pyrImage[i]);
	}	
}






//将元素中小于零的部分统一赋为零值。
void Clamp(CvMat *src,CvMat *dst)
{
	int i,j;
	CvMat *tempSrc,*mask;
	tempSrc = cvCreateMat(src->rows,src->cols,src->type);
	mask = cvCreateMat(src->rows,src->cols,CV_8UC1);
	cvCopy(src,tempSrc);

	cvSetZero(dst);
	cvCmpS(tempSrc,0.0,mask,CV_CMP_GE);
	cvCopy(tempSrc,dst,mask);
}
//DoGFilter
void DoGFilter(float exSigma,float exC,float inhSigma,float inhC,int radius,CvMat *T)
{
	//关于二维数组作为函数参数进行传递：http://ziji.yo2.cn/archives/628720
	int x,y,width;
	float exParm1,exParm2,inhParm1,inhParm2,test;
	exParm1 = 0.5/(exSigma*exSigma);
	inhParm1 = 0.5/(inhSigma*inhSigma);
	exParm2 = exC*exC*exParm1/PI;
	inhParm2 = inhC*inhC*inhParm1/PI;
	width = radius+radius+1;

	for(x=-radius;x<=radius;x++)
		for(y=-radius;y<=radius;y++)
		{
			test = exParm2*exp(-(x*x+y*y)*exParm1)-inhParm2*exp(-(x*x+y*y)*inhParm1);
			cvSetReal2D(T,x+radius,y+radius,test);
			//cvSetReal2D(T,x+radius,y+radius,exParm2*exp(-(x*x+y*y)*exParm1)-inhParm2*exp(-(x*x+y*y)*inhParm1));
		}
}
//Iterative Localized Interations
void IterativeNorm(CvMat *src,CvMat *dst,int nIteration)
{
	int sz,width,radius,i,x,y,m,n;
	float exSigma,inhSigma,exC,inhC;
	CvMat *T;
	CvScalar TSum;
	CvScalar constInh;
	float overlapDoGSum=0,overlapConvSum=0;

	CvMat *tempSrc,*tempMat1,*tempMat2;
	tempSrc = cvCreateMat(src->rows,src->cols,CV_32FC1);
	tempMat1 = cvCreateMat(src->rows,src->cols,CV_32FC1);
	tempMat2 = cvCreateMat(src->rows,src->cols,CV_32FC1);
	cvConvert(src,tempSrc);

	sz=Min(src->rows,src->cols);
	exSigma = 0.02*sz;
	inhSigma = 0.25*sz;
	exC = 0.5;
	inhC = 1.5;
	constInh.val[0] = 0.02;
	radius = sz/8;
	width = radius*2+1;

	T = cvCreateMat(width,width,CV_32FC1);
	DoGFilter(exSigma,exC,inhSigma,inhC,radius,T);
	TSum=cvSum(T);


	for(i=0;i<nIteration;i++)
	{
		cvNormalize(tempSrc,tempSrc,1,0,CV_MINMAX,NULL);
		TrunDoGConv(tempSrc,dst,T);
		cvAdd(tempSrc,dst,tempMat1);
		cvSubS(tempMat1,constInh,dst);
		Clamp(dst,dst);
		cvCopy(dst,tempSrc);
	}
	cvNormalize(dst,dst,1,0,CV_MINMAX,NULL);
}
//Trucated DoG convolution
void TrunDoGConv(CvMat *src,CvMat *dst,CvMat *T)
{
	int xCenter,yCenter,x,y,width,radius;
	float overlapDoGSum=0,overlapConvSum=0;
	CvScalar TSum;
	TSum = cvSum(T);

	width = T->rows;
	radius = (width-1)/2;

	for(xCenter=0;xCenter<src->cols;xCenter++)
	{
		for(yCenter=0;yCenter<src->rows;yCenter++)
		{	
			overlapDoGSum = 0;
			overlapConvSum = 0;
			for(x=0;x<width;x++)
			{
				if(x+xCenter<=radius || x+xCenter>=T->cols+radius)
					continue;
				for(y=0;y<width;y++)
				{
					if(y+yCenter<=radius || y+yCenter>=T->rows+radius)
						continue;
					else
					{
						overlapDoGSum += cvGetReal2D(T,x,y);
						overlapConvSum += cvGetReal2D(src,xCenter+x-radius,yCenter+y-radius);
					}
				}
			}
			cvSetReal2D(dst,yCenter,xCenter,TSum.val[0]*overlapConvSum/overlapDoGSum);	
		}
	}
}
//% T = DoGFilter(sigma_ex,c_ex, sigma_in,c_in, width)
//%
//%   Compute the DoG filter used to inhibit surround stimuli in a contrast map
//% before combine the map into conspicuity feature map.
//%   
//%   sigma_ex    :   para for extraction 
//%   c_ex        :
//%   sigma_in    :   para for inhibition
//%   c_in        :
//%   width       :   the width of the filter pattern expected
//%
//
//function T=DoGFilter(sigma_ex,c_ex, sigma_in,c_in, radius)
//
//X = -radius:radius;
//Y = X;
//
//T = zeros(length(X),length(Y));
//
//over_sigma_ex2 = 0.5/(sigma_ex*sigma_ex);
//over_sigma_in2 = 0.5/(sigma_in*sigma_in);
//para_ex = c_ex*c_ex*over_sigma_ex2/pi;
//para_in = c_in*c_in*over_sigma_in2/pi;
//
//X2 = X.*X;
//Y2 = Y.*Y;
//for y=1:length(Y)
//for x=1:length(X)
//%T(y,x) = DOG(c_ex, sigma_ex, X(x), Y(y)) - DOG(c_in, sigma_in, X(x), Y(y));
//X2Y2 = X2(x)+Y2(y);
//T(y,x) = para_ex*exp(-over_sigma_ex2*X2Y2) - para_in*exp(-over_sigma_in2*X2Y2);
//end
//end
//
//%size(X), size(T)
//%figure(1);  mesh(X,Y,T);
//
//return;
//
//
//function d = DOG(c,sigma,x,y)
//sigma2 = sigma.*sigma;
//d = 0.5*(c.*c)/(pi*sigma2) * exp( - 0.5*(x.*x+y.*y)/sigma2 );


//计算强度特牲的显著图
void CompIntensity(CvMat *IntensityMat,CvMat *pyrMat[])
{
	int i;
	CvMat *IPyr[9];              //强度特征金字塔
	//CvMat *rPyr[9],*gPyr[9],*bPyr[9];
	CvMat *IFeatureMap[6];       //6个强度特征图
	CvMat *f32IFeatureMap[6];    //浮点数据类型存储的特征图
	CvMat *IConMap;              //强度特征的Conspicuity Map
	double maxNorm;              //最大值
	CvScalar avgNorm;            //平均值
	CvMat *tempMap1,*tempMap2;   //计算过程中用到的临时变量
	int tempRows,tempCols;
	double parm;

////////////////////////////////////////////////
	GlobalImageSize.height = 512;
	GlobalImageSize.width = 512;
	////////////////////////////////////

	//------获取金字塔数据------
	for(i=0;i<=8;i++)  
	{
		IPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		cvCvtColor(pyrMat[i],IPyr[i],CV_BGR2GRAY);
		/*rPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		gPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		bPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		tempMap1 = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		cvSplit(pyrMat[i],bPyr[i],gPyr[i],rPyr[i],NULL);
		cvAdd(rPyr[i],gPyr[i],IPyr[i]);
		cvAdd(bPyr[i],IPyr[i],tempMap1);
		cvScale(tempMap1,IPyr[i],1.0/3.0,0);*/
	}


	//------计算6幅特征图------
	for(i=2;i<5;i++)  //Feature Map
	{
		tempRows=IPyr[i]->rows;
		tempCols=IPyr[i]->cols;
		tempMap1 = cvCreateMat(tempRows,tempCols,CV_8UC1);
		tempMap2 = cvCreateMat(tempRows,tempCols,CV_8UC1);

		IFeatureMap[2*i-4]=cvCreateMat(tempRows,tempCols,CV_8UC1);//I:2-5
		cvResize(IPyr[i+3],tempMap1);
		cvAbsDiff(IPyr[i],tempMap1,IFeatureMap[2*i-4]);

		IFeatureMap[2*i-3]=cvCreateMat(tempRows,tempCols,CV_8UC1);//I:2-6
		cvResize(IPyr[i+4],tempMap1);
		cvAbsDiff(IPyr[i],tempMap1,IFeatureMap[2*i-3]);
	}
	for(i=0;i<6;i++)
	{
		cvReleaseMat(&IPyr[i]);
	}

	//------对6幅特征图作正规化操作
	for(i=0;i<6;i++)
	{
		f32IFeatureMap[i]=cvCreateMat(IFeatureMap[i]->rows,IFeatureMap[i]->cols,CV_32FC1);
		NonLinearAmp(IFeatureMap[i],f32IFeatureMap[i]);
		//DispMat(IFeatureMap[i]);
		//IterativeNorm(IFeatureMap[i],f32IFeatureMap[i],1);
		//DispMat(f32IFeatureMap[i]);

		/*tempMap1=cvCreateMat(IFeatureMap[i]->rows,IFeatureMap[i]->cols,CV_32FC1);
		cvConvert(IFeatureMap[i],tempMap1);
		cvMinMaxLoc(tempMap1,NULL,&maxNorm,NULL,NULL,NULL);
		avgNorm=cvAvg(tempMap1,NULL);
		cvNormalize(tempMap1,tempMap1,0,1,CV_MINMAX,NULL);
		parm = (maxNorm-avgNorm.val[0])*(maxNorm-avgNorm.val[0]);
		cvScale(tempMap1,f32IFeatureMap[i],parm,0);
		cvNormalize(f32IFeatureMap[i],f32IFeatureMap[i],0,1,CV_MINMAX,NULL);*/
	}
	for(i=0;i<6;i++)
	{
		cvReleaseMat(&IFeatureMap[i]);
	}

	//------计算The Conspicuity Maps
	IConMap=cvCreateMat(pyrMat[4]->rows,pyrMat[4]->cols,CV_32FC1);
	tempMap1=cvCreateMat(pyrMat[4]->rows,pyrMat[4]->cols,CV_32FC1);
	tempMap2=cvCreateMat(pyrMat[4]->rows,pyrMat[4]->cols,CV_32FC1);

	//------对6幅Conspicuity Maps的数据进行平均得到最终的强度特征显著图
	cvSetZero(IConMap);
	for(i=0;i<6;i++)
	{
		cvResize(f32IFeatureMap[i],tempMap1);
		cvCopy(IConMap,tempMap2);
		cvAdd(tempMap1,tempMap2,IConMap);	
	}
	tempMap1=cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
	cvResize(IConMap,tempMap1);
	cvConvertScale(tempMap1,IntensityMat,1/6.0,0);
	cvNormalize(IntensityMat,IntensityMat,0,1,CV_MINMAX,NULL);
	//-----释放空间
	for(i=0;i<6;i++)
	{
		cvReleaseMat(&f32IFeatureMap[i]);
	}
	cvReleaseMat(&tempMap1);
	cvReleaseMat(&tempMap2);
	cvReleaseMat(&IConMap);
}

//计算颜色牲的显著图
void CompColor(CvMat *ColorMap,CvMat *pyrMat[])
{
	int i;
	CvMat *rPyr[9],*gPyr[9],*bPyr[9],*RPyr[9],*GPyr[9],*BPyr[9],*YPyr[9];
	CvMat *RGFeatureMap[6],*BYFeatureMap[6];
	CvMat *f32RGFeatureMap[6],*f32BYFeatureMap[6];    //浮点数据类型存储的特征图
	CvMat *CConMap;                                   //Conspicuity Map
	CvMat *tempMap1,*tempMap2,*tempMap3;            //计算过程中用到的临时变量
	double maxNorm;              //最大值
	CvScalar avgNorm;            //平均值
	int tempRows,tempCols;
	double parm;

	//----提取RGBY金字塔数据
	for(i=0;i<=8;i++)
	{
		rPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		gPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		bPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		RPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		GPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		BPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		YPyr[i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);

		cvSplit(pyrMat[i],bPyr[i],gPyr[i],rPyr[i],NULL);
		ColorNorm(rPyr[i],gPyr[i],bPyr[i],RPyr[i],'R');
		ColorNorm(rPyr[i],gPyr[i],bPyr[i],GPyr[i],'G');
		ColorNorm(rPyr[i],gPyr[i],bPyr[i],BPyr[i],'B');
		ColorNorm(rPyr[i],gPyr[i],bPyr[i],YPyr[i],'Y');
	}
	for(i=0;i<=8;i++)
	{
		cvReleaseMat(&rPyr[i]);
		cvReleaseMat(&gPyr[i]);
		cvReleaseMat(&bPyr[i]);
	}

	//-----计算Feature Map
	for(i=2;i<5;i++)
	{
		tempRows=RPyr[i]->rows;
		tempCols=RPyr[i]->cols;
		tempMap1 = cvCreateMat(tempRows,tempCols,CV_8UC1);
		tempMap2 = cvCreateMat(tempRows,tempCols,CV_8UC1);

		RGFeatureMap[2*i-4]=cvCreateMat(tempRows,tempCols,CV_8UC1);//RG:2-5
		tempMap3=cvCreateMat(RPyr[i+3]->rows,RPyr[i+3]->cols,CV_8UC1);
		cvSub(RPyr[i],GPyr[i],tempMap1);
		cvSub(RPyr[i+3],GPyr[i+3],tempMap3);
		cvResize(tempMap3,tempMap2);
		cvAbsDiff(tempMap1,tempMap2,RGFeatureMap[2*i-4]);

		RGFeatureMap[2*i-3]=cvCreateMat(tempRows,tempCols,CV_8UC1);//RG:2-6
		tempMap3=cvCreateMat(RPyr[i+4]->rows,RPyr[i+4]->cols,CV_8UC1);
		cvSub(RPyr[i],GPyr[i],tempMap1);
		cvSub(RPyr[i+4],GPyr[i+4],tempMap3);
		cvResize(tempMap3,tempMap2);
		cvAbsDiff(tempMap1,tempMap2,RGFeatureMap[2*i-3]);

		BYFeatureMap[2*i-4]=cvCreateMat(tempRows,tempCols,CV_8UC1);//BY:2-5
		tempMap3=cvCreateMat(BPyr[i+3]->rows,BPyr[i+3]->cols,CV_8UC1);
		cvSub(BPyr[i],YPyr[i],tempMap1);
		cvSub(BPyr[i+3],YPyr[i+3],tempMap3);
		cvResize(tempMap3,tempMap2);
		cvAbsDiff(tempMap1,tempMap2,BYFeatureMap[2*i-4]);

		BYFeatureMap[2*i-3]=cvCreateMat(tempRows,tempCols,CV_8UC1);//BY:2-6
		tempMap3=cvCreateMat(BPyr[i+4]->rows,BPyr[i+4]->cols,CV_8UC1);
		cvSub(BPyr[i],YPyr[i],tempMap1);
		cvSub(BPyr[i+4],YPyr[i+4],tempMap3);
		cvResize(tempMap3,tempMap2);
		cvAbsDiff(tempMap1,tempMap2,BYFeatureMap[2*i-3]);
	}
	for(i=0;i<=8;i++)
	{
		cvReleaseMat(&RPyr[i]);
		cvReleaseMat(&GPyr[i]);
		cvReleaseMat(&BPyr[i]);
		cvReleaseMat(&YPyr[i]);
	}

	//-----Feature Map 的正规化操作
	for(i=0;i<6;i++)
	{
		tempMap1=cvCreateMat(RGFeatureMap[i]->rows,RGFeatureMap[i]->cols,CV_32FC1);

		f32RGFeatureMap[i]=cvCreateMat(RGFeatureMap[i]->rows,RGFeatureMap[i]->cols,CV_32FC1);
		f32BYFeatureMap[i]=cvCreateMat(BYFeatureMap[i]->rows,BYFeatureMap[i]->cols,CV_32FC1);
		NonLinearAmp(RGFeatureMap[i],f32RGFeatureMap[i]);
		NonLinearAmp(BYFeatureMap[i],f32BYFeatureMap[i]);
		/*cvConvert(RGFeatureMap[i],tempMap1);
		cvMinMaxLoc(tempMap1,NULL,&maxNorm,NULL,NULL,NULL);
		avgNorm=cvAvg(tempMap1,NULL);
		cvNormalize(tempMap1,tempMap1,0,1,CV_MINMAX,NULL);
		parm=(maxNorm-avgNorm.val[0])*(maxNorm-avgNorm.val[0]);
		cvScale(tempMap1,f32RGFeatureMap[i],parm,0);
		cvNormalize(f32RGFeatureMap[i],f32RGFeatureMap[i],0,1,CV_MINMAX,NULL);
		f32BYFeatureMap[i]=cvCreateMat(BYFeatureMap[i]->rows,BYFeatureMap[i]->cols,CV_32FC1);
		cvConvert(BYFeatureMap[i],tempMap1);
		cvMinMaxLoc(tempMap1,NULL,&maxNorm,NULL,NULL,NULL);
		avgNorm=cvAvg(tempMap1,NULL);
		cvNormalize(tempMap1,tempMap1,0,1,CV_MINMAX,NULL);
		parm=(maxNorm-avgNorm.val[0])*(maxNorm-avgNorm.val[0]);
		cvScale(tempMap1,f32BYFeatureMap[i],parm,0);
		cvNormalize(f32BYFeatureMap[i],f32BYFeatureMap[i],0,1,CV_MINMAX,NULL);*/
	}

	for(i=0;i<6;i++)
	{
		cvReleaseMat(&RGFeatureMap[i]);
		cvReleaseMat(&BYFeatureMap[i]);
	}

	//----计算The Conspicuity Maps
	CConMap=cvCreateMat(pyrMat[4]->rows,pyrMat[4]->cols,CV_32FC1);
	tempMap1=cvCreateMat(pyrMat[4]->rows,pyrMat[4]->cols,CV_32FC1);
	tempMap2=cvCreateMat(pyrMat[4]->rows,pyrMat[4]->cols,CV_32FC1);

	cvSetZero(CConMap);
	for(i=0;i<6;i++)
	{
		cvResize(f32RGFeatureMap[i],tempMap1);
		cvCopy(CConMap,tempMap2);
		cvAdd(tempMap1,tempMap2,CConMap);

		cvResize(f32BYFeatureMap[i],tempMap1);
		cvCopy(CConMap,tempMap2);
		cvAdd(tempMap1,tempMap2,CConMap);
	}

	tempMap1=cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
	cvResize(CConMap,tempMap1);
	cvConvertScale(tempMap1,ColorMap,1/12.0,0);
	cvNormalize(ColorMap,ColorMap,0,1,CV_MINMAX,NULL);	

	//-----释放空间
	for(i=0;i<6;i++)
	{
		cvReleaseMat(&f32RGFeatureMap[i]);
		cvReleaseMat(&f32BYFeatureMap[i]);
	}
	cvReleaseMat(&tempMap1);
	cvReleaseMat(&tempMap2);
	cvReleaseMat(&tempMap3);
	cvReleaseMat(&CConMap);
}

//计算方向特性的显著图
void CompOrientation(CvMat *OrientationMap,CvMat *pyrMat[])
{
	int i,j;
	CvMat *OPyr[GABOR_ORI][TOTALSCALE];   //方向特征金字塔
	CvMat *OFeatureMap[GABOR_ORI][6];       //6个强度特征图
	CvMat *OConMap;              //强度特征的Conspicuity Map
	CvMat *tempMap1,*tempMap2;   //计算过程中用到的临时变量
	int tempRows,tempCols;

	//------获取金字塔数据------
	for(i=0;i<TOTALSCALE;i++)  
	{
		tempMap1 = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_8UC1);
		tempMap2 = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_32FC1);
		cvCvtColor(pyrMat[i],tempMap1,CV_BGR2GRAY);
		cvConvert(tempMap1,tempMap2);
		for(j=0;j<GABOR_ORI;j++)
		{
			OPyr[j][i] = cvCreateMat(pyrMat[i]->rows,pyrMat[i]->cols,CV_32FC1);
			GaborFilterImage(tempMap2,OPyr[j][i],i,j);
		}
	}
    
	//------计算6幅特征图------

	for(i=2;i<5;i++)  //Feature Map
	{
		tempRows=pyrMat[i]->rows;
		tempCols=pyrMat[i]->cols;
		for(j=0;j<GABOR_ORI;j++)
		{
			tempMap1 = cvCreateMat(tempRows,tempCols,CV_32FC1);
			tempMap2 = cvCreateMat(tempRows,tempCols,CV_32FC1);

			OFeatureMap[j][2*i-4]=cvCreateMat(tempRows,tempCols,CV_32FC1);//I:2-5
			cvResize(OPyr[j][i+3],tempMap1);
			cvAbsDiff(OPyr[j][i],tempMap1,OFeatureMap[j][2*i-4]);

			OFeatureMap[j][2*i-3]=cvCreateMat(tempRows,tempCols,CV_32FC1);//I:2-6
			cvResize(OPyr[j][i+4],tempMap1);
			cvAbsDiff(OPyr[j][i],tempMap1,OFeatureMap[j][2*i-3]);
		}
		
	}
	for(i=0;i<TOTALSCALE;i++)
	{
		for(j=0;j<GABOR_ORI;j++)
		{
			cvReleaseMat(&OPyr[j][i]);
		}
	}

	//------计算The Conspicuity Maps
	OConMap=cvCreateMat(pyrMat[4]->rows,pyrMat[4]->cols,CV_32FC1);
	tempMap1=cvCreateMat(pyrMat[4]->rows,pyrMat[4]->cols,CV_32FC1);
	tempMap2=cvCreateMat(pyrMat[4]->rows,pyrMat[4]->cols,CV_32FC1);
	cvSetZero(OConMap);
	for(i=0;i<GABOR_ORI;i++)
	{
		for(j=0;j<6;j++)
		{
			NonLinearAmp(OFeatureMap[i][j],OFeatureMap[i][j]);//------对6幅特征图作正规化操作
			cvResize(OFeatureMap[i][j],tempMap1);
			cvCopy(OConMap,tempMap2);
			cvAdd(tempMap1,tempMap2,OConMap);
		}
	}
	
	tempMap1=cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
	cvResize(OConMap,tempMap1);
	cvConvertScale(tempMap1,OrientationMap,1.0/double(6*GABOR_ORI),0);
	cvNormalize(OrientationMap,OrientationMap,0,1,CV_MINMAX,NULL);
	//-----释放空间
	for(i=0;i<GABOR_ORI;i++)
	{
		for(j=0;j<6;j++)
		{
			cvReleaseMat(&OFeatureMap[i][j]);
		}
	}
	cvReleaseMat(&tempMap1);
	cvReleaseMat(&tempMap2);
	cvReleaseMat(&OConMap);
}
//用全局的非线性放大法进行图像的正规化操作
void NonLinearAmp(CvMat *src,CvMat *dst)
{
	double max;
	CvScalar avg;
	CvMat *tempMat;
	float parm;
	tempMat = cvCreateMat(src->rows,src->cols,CV_32FC1);
	cvConvert(src,tempMat);
	cvMinMaxLoc(tempMat,NULL,&max,NULL,NULL,NULL);
	avg=cvAvg(tempMat,NULL);
	cvNormalize(tempMat,tempMat,0,1,CV_MINMAX,NULL);
	parm=(max-avg.val[0])*(max-avg.val[0]);
	cvScale(tempMat,dst,parm,0);
	cvNormalize(dst,dst,0,1,CV_MINMAX,NULL);
}






//Gabor滤波
//void Gabor(CvMat *src, CvMat *dst)
//{
//	int x,y;
//	double sigma=6,g,gr,gi;
//	int M=6,N=6;
//	for(x=0;x<M;x++)
//	{
//		for(y=0;y<N;y++)
//		{
//			x=cvGetReal2D(src,x,y);
//			gr=1/(2*PI*sigma*sigma)*exp(-(x*x+y*y)/2/sigma/sigma)*cos();
//		}
//	}
//	
//}
//RGBY颜色的正规化操作，以消除强度对颜色的影响
void ColorNorm(CvMat *rMat, CvMat *gMat,CvMat *bMat,CvMat *dstMat,uchar flag)
{
	int tempHeight = rMat->rows;
	int tempWidth = rMat->cols;

	int m,n;
	int tempData=0,r=0,g=0,b=0;

	for(m=0;m<tempHeight;m++)
	{
		for(n=0;n<tempWidth;n++)
		{	
			r = (int)cvGetReal2D(rMat,m,n);
			g = (int)cvGetReal2D(gMat,m,n);
			b = (int)cvGetReal2D(bMat,m,n);

			/*	r=((rMat->data.ptr+rMat->step*m))[n];
			g=((gMat->data.ptr+gMat->step*m))[n];
			b=((bMat->data.ptr+bMat->step*m))[n];*/

			switch(flag)
			{
			case 'R':
				tempData = r-(g+b)/2;
				break;
			case 'G':
				tempData = g-(r+b)/2;
				break;
			case 'B':
				tempData = b-(r+g)/2;
				break;
			case 'Y':
				tempData = r+g-2*(Abs(r-g)+b);
				break;
			default:
				break;
			}
			if(tempData<0)
				tempData=0;
			cvSetReal2D(dstMat,m,n,tempData);
			//(rMat->data.ptr+rMat->step*m)[n]=tempData;
		}
		//cvSetReal2D(dstMat,m,n,tempData);
	}			
}



/*
 --------------------------------------------------------------------------------------
The GaborFilteredImg provides the outputs of the Gabor filter bank
-----------------------------------------------------------------------------------------
void GaborFilteredImg(CvMat *FilteredImg_real, CvMat *FilteredImg_imag, CvMat *img, int side, double Ul, double Uh, 
int scale, int orientation, int flag)
{
int h, w, xs, ys, border, r1, r2, r3, r4, hei, wid, s, n;
CvMat *IMG, *IMG_imag, *Gr, *Gi, *Tmp_1, *Tmp_2, *F_1, *F_2, *G_real, *G_imag, *F_real, *F_imag;
double temp;

border = side;
hei = img->height;
wid = img->width;

 FFT2 
xs = (int) pow(2.0, ceil(log2((double)(img->height+2.0*border))));
ys = (int) pow(2.0, ceil(log2((double)(img->width+2.0*border))));

IMG=cvCreateMat(xs,ys,CV_32FC1);
CreateMatrix(&IMG, xs, ys);

r1 = img->width+border;
r2 = img->width+2*border;
for (h=0;h<border;h++) {
for (w=0;w<border;w++)
{
temp=cvGetReal2D(img,border-1-h,border-1-w);
cvSetReal2D(IMG,h,w,temp);
}
IMG->data[h][w] = img->data[border-1-h][border-1-w];
for (w=border;w<r1;w++)
{
temp=cvGetReal2D(img,border-1-h,w-border);
cvSetReal2D(IMG,h,w,temp);
}
IMG->data[h][w] = img->data[border-1-h][w-border];
for (w=r1;w<r2;w++)
{
temp=cvGetReal2D(img,border-1-h,img->width-w+border-1);
cvSetReal2D(IMG,h,w,temp);
}
IMG->data[h][w] = img->data[border-1-h][2*img->width-w+border-1];
}

r1 = img->height+border;
r2 = img->width+border;
r3 = img->width+2*border;
for (h=border;h<r1;h++) {
for (w=0;w<border;w++)
{
temp=cvGetReal2D(img,h-border,border-1-w);
cvSetReal2D(IMG,h,w,temp);
}
IMG->data[h][w] = img->data[h-border][border-1-w];
for (w=border;w<r2;w++)
{
temp=cvGetReal2D(img,h-border,w-border);
cvSetReal2D(IMG,h,w,temp);
}
IMG->data[h][w] = img->data[h-border][w-border];
for (w=r2;w<r3;w++)
{
temp=cvGetReal2D(img,h-border,2*img->width-w+border-1);
cvSetReal2D(IMG,h,w,temp);
}
IMG->data[h][w] = img->data[h-border][2*img->width-w+border-1];
}

r1 = img->height+border;
r2 = img->height+2*border;
r3 = img->width+border;
r4 = img->width+2*border;
for (h=r1;h<r2;h++) {
for (w=0;w<border;w++)
{
temp=cvGetReal2D(img,2*img->height-h+border-1,border-1-w);
cvSetReal2D(IMG,h,w,temp);
}
IMG->data[h][w] = img->data[2*img->height-h+border-1][border-1-w];
for (w=border;w<r3;w++)
{
temp=cvGetReal2D(img,2*img->height-h+border-1,w-border);
cvSetReal2D(IMG,h,w,temp);
}
IMG->data[h][w] = img->data[2*img->height-h+border-1][w-border];
for (w=r3;w<r4;w++)
{
temp=cvGetReal2D(img,2*img->height-h+border-1,2*img->width-w+border-1);
cvSetReal2D(IMG,h,w,temp);
}
IMG->data[h][w] = img->data[2*img->height-h+border-1][2*img->width-w+border-1];
}

F_real=cvCreateMat(xs,ys,CV_32FC1);
F_imag=cvCreateMat(xs,ys,CV_32FC1);
IMG_imag=cvCreateMat(xs,ys,CV_32FC1);


CreateMatrix(&F_real, xs, ys);
CreateMatrix(&F_imag, xs, ys);
CreateMatrix(&IMG_imag, xs, ys);

cvDFT()
Mat_FFT2(F_real, F_imag, IMG, IMG_imag);

 ----------- compute the Gabor filtered output ------------- 

CreateMatrix(&Gr, 2*side+1, 2*side+1);
CreateMatrix(&Gi, 2*side+1, 2*side+1);
CreateMatrix(&Tmp_1, xs, ys);
CreateMatrix(&Tmp_2, xs, ys);
CreateMatrix(&F_1, xs, ys);
CreateMatrix(&F_2, xs, ys);
CreateMatrix(&G_real, xs, ys);
CreateMatrix(&G_imag, xs, ys);

for (s=0;s<scale;s++) {
for (n=0;n<orientation;n++) {
Gabor(Gr, Gi, s+1, n+1, Ul, Uh, scale, orientation, flag);
Mat_Copy(F_1, Gr, 0, 0, 0, 0, 2*side, 2*side);
Mat_Copy(F_2, Gi, 0, 0, 0, 0, 2*side, 2*side);
Mat_FFT2(G_real, G_imag, F_1, F_2);

Mat_Product(Tmp_1, G_real, F_real);
Mat_Product(Tmp_2, G_imag, F_imag);
Mat_Substract(IMG, Tmp_1, Tmp_2);

Mat_Product(Tmp_1, G_real, F_imag);
Mat_Product(Tmp_2, G_imag, F_real);
Mat_Sum(IMG_imag, Tmp_1, Tmp_2);

Mat_IFFT2(Tmp_1, Tmp_2, IMG, IMG_imag);

Mat_Copy(FilteredImg_real, Tmp_1, s*hei, n*wid, 2*side, 2*side, hei+2*side-1, wid+2*side-1);
Mat_Copy(FilteredImg_imag, Tmp_2, s*hei, n*wid, 2*side, 2*side, hei+2*side-1, wid+2*side-1);
}
}

FreeMatrix(Gr);
FreeMatrix(Gi);
FreeMatrix(Tmp_1);
FreeMatrix(Tmp_2);
FreeMatrix(F_1);
FreeMatrix(F_2);
FreeMatrix(G_real);
FreeMatrix(G_imag);
FreeMatrix(F_real);
FreeMatrix(F_imag);
FreeMatrix(IMG);
FreeMatrix(IMG_imag);
}*/

//------------------------------------------------------------------------------------------------------
//The Gabor function generates a Gabor filter with the selected index 's' and 'n' (scale and orientation, 
//respectively) from a Gabor filter bank. This filter bank is designed by giving the range of spatial 
//frequency (Uh and Ul) and the total number of scales and orientations used to partition the spectrum. 
//
//The returned filter is stored in 'Gr' (real part) and 'Gi' (imaginary part).
//--------------------------------------------------------------------------------------------------------
void MakeGaborFilter(CvMat *Gr, CvMat *Gi, int s, int n, double Ul, double Uh, int scale, int orientation, int flag)
{
	double base, a, u0, z, Uvar, Vvar, Xvar, Yvar, X, Y, G, t1, t2, m;
	int x, y, side;
	double temp;

	base = Uh/Ul;
	a = pow(base, 1.0/(double)(scale-1));

	u0 = Uh/pow(a, (double) scale-s);

	Uvar = (a-1.0)*u0/((a+1.0)*sqrt(2.0*log(2.0)));

	z = -2.0*log(2.0)*(Uvar*Uvar)/u0;
	Vvar = tan(PI/(2*orientation))*(u0+z)/sqrt(2.0*log(2.0)-z*z/(Uvar*Uvar));

	Xvar = 1.0/(2.0*PI*Uvar);
	Yvar = 1.0/(2.0*PI*Vvar);

	t1 = cos(PI/orientation*(n-1.0));
	t2 = sin(PI/orientation*(n-1.0));

	side = (int) (Gr->height-1)/2;

	for (x=0;x<2*side+1;x++) {
		for (y=0;y<2*side+1;y++) {
			X = (double) (x-side)*t1+ (double) (y-side)*t2;
			Y = (double) -(x-side)*t2+ (double) (y-side)*t1;
			G = 1.0/(2.0*PI*Xvar*Yvar)*pow(a, (double) scale-s)*exp(-0.5*((X*X)/(Xvar*Xvar)+(Y*Y)/(Yvar*Yvar)));
			cvSetReal2D(Gr,x,y,G*cos(2.0*PI*u0*X));
			cvSetReal2D(Gi,x,y,G*sin(2.0*PI*u0*X));
		}
	}

	//if flag = 1, then remove the DC from the filter 
		if (flag == 1) {
			m = 0;
			for (x=0;x<2*side+1;x++)
				for (y=0;y<2*side+1;y++)
					m += cvGetReal2D(Gr,x,y);

			m /= pow((double) 2.0*side+1, 2.0);

			for (x=0;x<2*side+1;x++)
				for (y=0;y<2*side+1;y++)
				{
					temp=cvGetReal2D(Gr,x,y);
					temp -= m;
					cvSetReal2D(Gr,x,y,temp);
				}
		}	
}




void GaborFilterImage(CvMat *src,CvMat *dst,int scale,int ori)
{
	CvMat *Gr,*Gi;
	Gr = cvCreateMat(GABOR_SIZE,GABOR_SIZE,CV_32FC1);
	Gi = cvCreateMat(GABOR_SIZE,GABOR_SIZE,CV_32FC1);
	MakeGaborFilter(Gr,Gi,scale,ori,GABOR_UL,GABOR_UH,GABOR_SCALE,GABOR_ORI,0);
	cvFilter2D(src,dst,Gr,cvPoint(-1,-1));
	cvNormalize(dst,dst,1,0,CV_MINMAX,NULL);
}