
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
void CvSizeCopy(CvSize* pDst, const CvSize* pSrc)
{
	memcpy(pDst, pSrc, sizeof(CvSize));
}

//显示矩阵格式储存的图像
void DispMat(CvMat	*matImage)
{
	IplImage *tempImage;
	CvMat *tempMat;
	tempMat = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,matImage->type);
	if(cvGetElemType(matImage)==CV_8UC1)
		tempImage = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_8U,1);
	else if(cvGetElemType(matImage)==CV_32FC1)
		tempImage = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_32F,1);
	else if(cvGetElemType(matImage)==CV_8UC3)
		tempImage = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_8U,3);
	else if(cvGetElemType(matImage)==CV_32FC3)
		tempImage = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_32F,3);

	cvResize(matImage,tempMat);
	cvGetImage(tempMat,tempImage);
	cvNamedWindow("Display Matrix Image",CV_WINDOW_AUTOSIZE);
	cvShowImage("Display Matrix Image",tempImage);
	cvWaitKey(0);
	cvDestroyWindow("Display Matrix Image");
	cvReleaseImage(&tempImage);
	cvReleaseMat(&tempMat);
}

//同时显示两个矩阵的图像，便于比较
void DispDoubleMat(CvMat *mat1,CvMat *mat2)
{
	IplImage *tempImage1,*tempImage2;
	CvMat *tempMat1,*tempMat2;
	//	tempImage=cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_32F,1);
	//	tempMat = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
	if(cvGetElemType(mat1)==CV_8UC1)
	{
		tempImage1 = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_8U,1);
		tempMat1 = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_8UC1);
	}
	else if(cvGetElemType(mat1)==CV_32FC1)
	{
		tempImage1 = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_32F,1);
		tempMat1 = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
	}else if(cvGetElemType(mat1)==CV_8UC3)
	{
		tempImage1 = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_8U,3);
		tempMat1 = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_8UC3);
	}else if(cvGetElemType(mat1)==CV_32FC3)
	{
		tempImage1 = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_32F,3);
		tempMat1 = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC3);
	}

	if(cvGetElemType(mat2)==CV_8UC1)
	{
		tempImage2 = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_8U,1);
		tempMat2 = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_8UC1);
	}
	else if(cvGetElemType(mat2)==CV_32FC1)
	{
		tempImage2 = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_32F,1);
		tempMat2 = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
	}else if(cvGetElemType(mat2)==CV_8UC3)
	{
		tempImage2 = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_8U,3);
		tempMat2 = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_8UC3);
	}else if(cvGetElemType(mat2)==CV_32FC3)
	{
		tempImage2 = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_32F,3);
		tempMat2 = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC3);
	}

	cvResize(mat1,tempMat1);
	cvGetImage(tempMat1,tempImage1);
	cvNamedWindow("Image 1",CV_WINDOW_AUTOSIZE);
	cvShowImage("Image 1",tempImage1);

	cvResize(mat2,tempMat2);
	cvGetImage(tempMat2,tempImage2);
	cvNamedWindow("Image 2",CV_WINDOW_AUTOSIZE);
	cvShowImage("Image 2",tempImage2);

	cvWaitKey(0);
	cvDestroyWindow("Image 1");
	cvDestroyWindow("Image 2");
	cvReleaseImage(&tempImage1);
	cvReleaseMat(&tempMat1);
	cvReleaseImage(&tempImage2);
	cvReleaseMat(&tempMat2);
}
//同时打印一张图片与一个矩阵图像
void DispImageMat(IplImage *img,CvMat *mat)
{
	IplImage *tempImage;
	CvMat *tempMat;


	////////////////////////////////////////////////
	GlobalImageSize.height = 512;
	GlobalImageSize.width = 512;
	////////////////////////////////////

	//	tempImage=cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_32F,1);
	//	tempMat = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,CV_32FC1);
	tempMat = cvCreateMat(GlobalImageSize.height,GlobalImageSize.width,mat->type);

	if(cvGetElemType(mat)==CV_8UC1)
		tempImage = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_8U,1);
	else if(cvGetElemType(mat)==CV_32FC1)
		tempImage = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_32F,1);
	else if(cvGetElemType(mat)==CV_8UC3)
		tempImage = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_8U,3);
	else if(cvGetElemType(mat)==CV_32FC3)
		tempImage = cvCreateImageHeader(GlobalImageSize,IPL_DEPTH_32F,3);

	cvResize(mat,tempMat);
	cvGetImage(tempMat,tempImage);
	cvNamedWindow("Matrix Image",CV_WINDOW_AUTOSIZE);
	cvShowImage("Matrix Image",tempImage);
	cvNamedWindow("Image",CV_WINDOW_AUTOSIZE);
	cvShowImage("Image",img);
	cvWaitKey(0);
	cvDestroyWindow("Matrix Image");
	cvDestroyWindow("Image");
	cvReleaseImage(&tempImage);
	cvReleaseMat(&tempMat);
}
//打印矩阵元素
void PrintMatData(CvMat *mat,char* msg)
{
	printf("The Element of Matrix %s:\n",msg);
	for(int i=1;i<10;i++)
		printf("%f\n",cvGetReal2D(mat,1,i));
	printf("\n");
}

//显示图像
void DispImage(IplImage *img)
{
	cvNamedWindow("Display Image",CV_WINDOW_AUTOSIZE);
	cvShowImage("Display Image",img);
	cvWaitKey(0);
	cvDestroyWindow("Display Image");
	cvReleaseImage(&img);
}
//另一种简单方式用于显示矩阵格式储存的图像
void SimDispMat(CvMat *matImage)
{
	IplImage *tempImage;

	CvSize   imageSize;
	imageSize.height=matImage->rows;
	imageSize.width =matImage->cols;
	tempImage=cvCreateImageHeader(imageSize,IPL_DEPTH_8U,3);

	cvGetImage(matImage,tempImage);
	cvNamedWindow("Simple Display Matrix Image",CV_WINDOW_AUTOSIZE);
	cvShowImage("Simple Display Matrix Image",tempImage);
	cvWaitKey(0);
	cvDestroyWindow("Display Matrix Image");
	cvReleaseImage(&tempImage);
}


/*
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
void ColorNorm(IplImage *r, IplImage *g, IplImage *b, IplImage *I,IplImage *dstImage,uchar flag)
{
int tempHeight = r->height;
int tempWidth = r->width;
int tempStep = r->widthStep/sizeof(unsigned char);

int idx=0,m=0,n=0;
char* dstData = dstImage->imageData;
char* rData = r->imageData;
char* gData = g->imageData;
char* bData = b->imageData;
char* IData = I->imageData;

char maxIntensity=0;   //寻找最大强度值
for(m=0;m<tempHeight;m++)
{
for(n=0;n<tempWidth;n++)
{
idx=m*tempStep+n;
if(IData[idx] > maxIntensity)
maxIntensity=IData[idx];
}
}

int threshold=maxIntensity/10; //人眼视觉注意的强度阈值

for(m=0;m<tempHeight;m++)
{
for(n=0;n<tempWidth;n++)
{
idx=m*tempStep+n;
if(IData[idx]<threshold)
{
dstData[idx] = 0;
}
else
{
switch(flag)
{
case 'R':
dstData[idx] = rData[idx]-(gData[idx]+bData[idx])/2;
break;
case 'G':
dstData[idx] = gData[idx]-(rData[idx]+bData[idx])/2;
break;
case 'B':
dstData[idx] = bData[idx]-(rData[idx]+gData[idx])/2;
break;
case 'Y':
dstData[idx] = rData[idx]+gData[idx]-2*(abs(rData[idx]-gData[idx])+bData[idx]);
break;
default:
break;
}

if(dstData[idx]<0)
dstData[idx]=0;
}
}
}
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
void GetIntensityMap(IplImage *R,IplImage *G,IplImage *B,IplImage *IntensityImage,CvScalar weight)
{
if(R->nChannels!=1 || G->nChannels!=1 || B->nChannels!=1)
{
printf("\nWhen do GetIntensityMap(..),the input images' Channel is not all equal to ONE!");
return ;
}
else
{
int tempHeight = R->height;
int tempWidth = R->width;
int tempStep = R->widthStep/sizeof(unsigned char);

unsigned char* tempRData = (unsigned char*)R->imageData;
unsigned char* tempGData = (unsigned char*)G->imageData;
unsigned char* tempBData = (unsigned char*)B->imageData;

for(int i=0;i<tempHeight;i++)
{
for(int j=0;j<tempWidth;j++)
{
int idx = i*tempStep+j;
IntensityImage->imageData[idx] = (char)((double)tempRData[idx]*weight.val[0]
+ (double)tempGData[idx]*weight.val[1]
+ (double)tempGData[idx]*weight.val[2]);
}
}
}
}
*/

/*
// --------------------------------------------------------------------------------------
//The GaborFilteredImg provides the outputs of the Gabor filter bank
//-----------------------------------------------------------------------------------------
void GaborFilteredImg(CvMat *FilteredImg_real, CvMat *FilteredImg_imag, CvMat *img, int side, double Ul, double Uh, 
int scale, int orientation, int flag)
{
int h, w, xs, ys, border, r1, r2, r3, r4, hei, wid, s, n;
CvMat *IMG, *IMG_imag, *Gr, *Gi, *Tmp_1, *Tmp_2, *F_1, *F_2, *G_real, *G_imag, *F_real, *F_imag;
double temp;

border = side;
hei = img->height;
wid = img->width;

// FFT2 
xs = (int) pow(2.0, ceil(log2((double)(img->height+2.0*border))));
ys = (int) pow(2.0, ceil(log2((double)(img->width+2.0*border))));

IMG=cvCreateMat(xs,ys,CV_32FC1);
//CreateMatrix(&IMG, xs, ys);

r1 = img->width+border;
r2 = img->width+2*border;
for (h=0;h<border;h++) {
for (w=0;w<border;w++)
{
temp=cvGetReal2D(img,border-1-h,border-1-w);
cvSetReal2D(IMG,h,w,temp);
}
//IMG->data[h][w] = img->data[border-1-h][border-1-w];
for (w=border;w<r1;w++)
{
temp=cvGetReal2D(img,border-1-h,w-border);
cvSetReal2D(IMG,h,w,temp);
}
//IMG->data[h][w] = img->data[border-1-h][w-border];
for (w=r1;w<r2;w++)
{
temp=cvGetReal2D(img,border-1-h,img->width-w+border-1);
cvSetReal2D(IMG,h,w,temp);
}
//IMG->data[h][w] = img->data[border-1-h][2*img->width-w+border-1];
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
//IMG->data[h][w] = img->data[h-border][border-1-w];
for (w=border;w<r2;w++)
{
temp=cvGetReal2D(img,h-border,w-border);
cvSetReal2D(IMG,h,w,temp);
}
//IMG->data[h][w] = img->data[h-border][w-border];
for (w=r2;w<r3;w++)
{
temp=cvGetReal2D(img,h-border,2*img->width-w+border-1);
cvSetReal2D(IMG,h,w,temp);
}
//IMG->data[h][w] = img->data[h-border][2*img->width-w+border-1];
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
//IMG->data[h][w] = img->data[2*img->height-h+border-1][border-1-w];
for (w=border;w<r3;w++)
{
temp=cvGetReal2D(img,2*img->height-h+border-1,w-border);
cvSetReal2D(IMG,h,w,temp);
}
//IMG->data[h][w] = img->data[2*img->height-h+border-1][w-border];
for (w=r3;w<r4;w++)
{
temp=cvGetReal2D(img,2*img->height-h+border-1,2*img->width-w+border-1);
cvSetReal2D(IMG,h,w,temp);
}
//IMG->data[h][w] = img->data[2*img->height-h+border-1][2*img->width-w+border-1];
}

F_real=cvCreateMat(xs,ys,CV_32FC1);
F_imag=cvCreateMat(xs,ys,CV_32FC1);
IMG_imag=cvCreateMat(xs,ys,CV_32FC1);


CreateMatrix(&F_real, xs, ys);
CreateMatrix(&F_imag, xs, ys);
CreateMatrix(&IMG_imag, xs, ys);

cvDFT()
Mat_FFT2(F_real, F_imag, IMG, IMG_imag);

// ----------- compute the Gabor filtered output ------------- 

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

//The returned filter is stored in 'Gr' (real part) and 'Gi' (imaginary part).
//--------------------------------------------------------------------------------------------------------
//void Gabor(CvMat *Gr, CvMat *Gi, int s, int n, double Ul, double Uh, int scale, int orientation, int flag)
//{
//	double base, a, u0, z, Uvar, Vvar, Xvar, Yvar, X, Y, G, t1, t2, m;
//	int x, y, side;
//	double temp;
//
//	base = Uh/Ul;
//	a = pow(base, 1.0/(double)(scale-1));
//
//	u0 = Uh/pow(a, (double) scale-s);
//
//	Uvar = (a-1.0)*u0/((a+1.0)*sqrt(2.0*log(2.0)));
//
//	z = -2.0*log(2.0)*(Uvar*Uvar)/u0;
//	Vvar = tan(PI/(2*orientation))*(u0+z)/sqrt(2.0*log(2.0)-z*z/(Uvar*Uvar));
//
//	Xvar = 1.0/(2.0*PI*Uvar);
//	Yvar = 1.0/(2.0*PI*Vvar);
//
//	t1 = cos(PI/orientation*(n-1.0));
//	t2 = sin(PI/orientation*(n-1.0));
//
//	side = (int) (Gr->height-1)/2;
//
//	for (x=0;x<2*side+1;x++) {
//		for (y=0;y<2*side+1;y++) {
//			X = (double) (x-side)*t1+ (double) (y-side)*t2;
//			Y = (double) -(x-side)*t2+ (double) (y-side)*t1;
//			G = 1.0/(2.0*PI*Xvar*Yvar)*pow(a, (double) scale-s)*exp(-0.5*((X*X)/(Xvar*Xvar)+(Y*Y)/(Yvar*Yvar)));
//			cvSetReal2D(Gr,x,y,G*cos(2.0*PI*u0*X));
//			cvSetReal2D(Gi,x,y,G*sin(2.0*PI*u0*X));
//			Gr->data[x][y] = G*cos(2.0*PI*u0*X);
//			Gi->data[x][y] = G*sin(2.0*PI*u0*X);
//		}
//	}
//
//	//if flag = 1, then remove the DC from the filter 
//		if (flag == 1) {
//			m = 0;
//			for (x=0;x<2*side+1;x++)
//				for (y=0;y<2*side+1;y++)
//					m += cvGetReal2D(Gr,x,y);
//			m += Gr->data[x][y];
//
//			m /= pow((double) 2.0*side+1, 2.0);
//
//			for (x=0;x<2*side+1;x++)
//				for (y=0;y<2*side+1;y++)
//				{
//					temp=cvGetReal2D(Gr,x,y);
//					temp -= m;
//					cvSetReal2D(Gr,x,y,temp);
//				}
//				Gr->data[x][y] -= m;
//		}	
//}

//void CreateImagePyramid(IplImage *img,CvMat *pyrMat[])
//{
//	int i;
//	IplImage *pyrImage[9];
//	pyrImage[0]=cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,3);
//	cvCopy(img,pyrImage[0]);
//
//	CvSize pyrSize=cvGetSize(img);
//	for(i=1;i<=8;i++)
//	{
//		pyrSize.height /= 2;
//		pyrSize.width  /= 2;
//		pyrImage[i]=cvCreateImage(pyrSize,IPL_DEPTH_8U,3);
//		cvPyrDown(pyrImage[i-1],pyrImage[i],CV_GAUSSIAN_5x5);
//	}
//
//	for(i=0;i<=8;i++)
//	{
//		cvGetMat(pyrImage[i],pyrMat[i]);
//		cvReleaseImage(&pyrImage[i]);
//	}	
//}
//void ColorNorm(CvMat *rMat, CvMat *gMat,CvMat *bMat,CvMat *dstMat,uchar flag,CvMat *mask)
//{
//	int tempHeight = rMat->rows;
//	int tempWidth = rMat->cols;
//
//	int m,n;
//	float tempData=0,r=0,g=0,b=0;
//
//	for(m=0;m<tempHeight;m++)
//	{
//		for(n=0;n<tempWidth;n++)
//		{	
//			tempData=0;
//			//			if(cvmGet(mask,m,n))
//			if((int)cvGetReal2D(mask,m,n))
//			{
//				//		r=(float)cvmGet(rMat,m,n);
//				//		g=(float)cvmGet(gMat,m,n);
//				//		b=(float)cvmGet(bMat,m,n);
//
//				r=(float)cvGetReal2D(rMat,m,n);
//				g=(float)cvGetReal2D(gMat,m,n);
//				b=(float)cvGetReal2D(bMat,m,n);
//
//				switch(flag)
//				{
//				case 'R':
//					tempData = r-(g+b)/2;
//					break;
//				case 'G':
//					tempData = g-(r+b)/2;
//					break;
//				case 'B':
//					tempData = b-(r+g)/2;
//					break;
//				case 'Y':
//					tempData = r+g-2*(fabs(r-g)+b);
//					break;
//				default:
//					break;
//				}
//				if(tempData<0)
//					tempData=0;
//			}
//			//cvmSet(dstMat,m,n,tempData);
//			cvSetReal2D(dstMat,m,n,tempData);
//		}			
//	}
//}

///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////