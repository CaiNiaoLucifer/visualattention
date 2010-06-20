
#ifndef _VISUAL_ATTENTION_CONST_
#define _VISUAL_ATTENTION_CONST_

#include <cv.h>
#include <cxcore.h>

//The type of normalizing feature map
enum VAM_Type_Norm_Feat_Map{
	VAM_NAVIE_SUM = 0,
	VAM_LINEAR_COMBINATIOIN = 1,
	VAM_NON_LINEAR_AMPLIFICATION = 2,
	VAM_ITERATIVE = 3
};

#endif