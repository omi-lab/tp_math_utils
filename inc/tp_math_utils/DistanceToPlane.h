#ifndef tp_math_utils_DistanceToPlane_h
#define tp_math_utils_DistanceToPlane_h

#include "tp_math_utils/Plane.h"

#include <vector>

namespace tp_math_utils
{

//##################################################################################################
float TP_MATH_UTILS_EXPORT distanceToPlane(const glm::vec3& point, const Plane& plane, glm::vec3& pointOnPlane);

//##################################################################################################
float TP_MATH_UTILS_EXPORT squaredDistanceToPlane(const glm::vec3& point, const Plane& plane, glm::vec3& pointOnPlane);

//##################################################################################################
double TP_MATH_UTILS_EXPORT summedSquaredDistanceToPlane(const std::vector<glm::vec3>& points, const Plane& plane);

}

#endif
