#ifndef tp_math_utils_DistanceBetweenLines_h
#define tp_math_utils_DistanceBetweenLines_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{
struct Ray;

//##################################################################################################
float TP_MATH_UTILS_SHARED_EXPORT distanceBetweenLines(const tp_math_utils::Ray& l1, const tp_math_utils::Ray& l2);

}

#endif
