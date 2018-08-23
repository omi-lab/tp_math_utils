#ifndef tp_math_utils_DistanceToPlane_h
#define tp_math_utils_DistanceToPlane_h

#include "tp_math_utils/Plane.h"

#include <vector>

namespace tp_math_utils
{

//##################################################################################################
float distanceToPlane(const glm::vec3& point, const Plane& plane, glm::vec3& pointOnPlane);

//##################################################################################################
float squaredDistanceToPlane(const glm::vec3& point, const Plane& plane, glm::vec3& pointOnPlane);

//##################################################################################################
double summedSquaredDistanceToPlane(const std::vector<glm::vec3>& points, const Plane& plane);

}

#endif
