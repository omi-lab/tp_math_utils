#ifndef tp_math_utils_StandardDeviation_h
#define tp_math_utils_StandardDeviation_h

#include "tp_math_utils/Globals.h"

#include <vector>

namespace tp_math_utils
{

//##################################################################################################
double TP_MATH_UTILS_SHARED_EXPORT calculateSD(const std::vector<float>& data);

//##################################################################################################
double TP_MATH_UTILS_SHARED_EXPORT calculateSD(const std::vector<float>& data, double& mean);

//##################################################################################################
void TP_MATH_UTILS_SHARED_EXPORT sortedDifferences(std::vector<float>& data);

}

#endif



