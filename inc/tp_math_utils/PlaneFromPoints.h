#ifndef tp_math_utils_PlaneFromPoints_h
#define tp_math_utils_PlaneFromPoints_h

#include "tp_math_utils/Plane.h"

#include <vector>

namespace tp_math_utils
{

//##################################################################################################
Plane TP_MATH_UTILS_SHARED_EXPORT planeFromPoints(const std::vector<glm::vec3>& points);

}

#endif
