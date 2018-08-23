#ifndef tp_math_utils_StandardDeviation_h
#define tp_math_utils_StandardDeviation_h

#include "tp_math_utils/Globals.h"

#include <vector>

namespace tp_math_utils
{

//##################################################################################################
double calculateSD(const std::vector<float>& data);

//##################################################################################################
double calculateSD(const std::vector<float>& data, double& mean);

//##################################################################################################
void sortedDifferences(std::vector<float>& data);

}

#endif



