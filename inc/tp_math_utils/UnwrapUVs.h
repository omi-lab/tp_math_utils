#ifndef tp_math_utils_UnwrapUVs_h
#define tp_math_utils_UnwrapUVs_h

#include "tp_math_utils/Geometry3D.h"

#include "tp_utils/Progress.h"

namespace tp_math_utils
{

//##################################################################################################
bool unwrapUVs(std::vector<Geometry3D>& geometry, tp_utils::Progress* progress);

}

#endif
