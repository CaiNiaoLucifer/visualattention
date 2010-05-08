
#ifndef _VISUAL_ATTENTION_CONST_
#define _VISUAL_ATTENTION_CONST_

#include <cv.h>
#include <cxcore.h>

//The type of normalizing feature map
enum{
	VAM_NAVIE_SUM = 0,
	VAM_LINEAR_COMBINATIOIN = 1,
	VAM_NON_LINEAR_AMPLIFICATION = 2,
	VAM_ITERATIVE_LOCALIZED_INTERACTIONS = 3
}VAM_Type_Norm_Feat_Map;

static CvSize GlobalImageSize;

#define Abs(x) x>0 ? x : -x
#define Min(a,b) a<b ? a : b
#define Max(a,b) a>b ? a : b
#define PI 3.14159265
#define TOTALSCALE 9

#define m_sizeOfGaborFilter  9
#define GABOR_UL   0.04
#define GABOR_UH   0.5
#define GABOR_ORI 4
#define GABOR_SCALE  4

#endif