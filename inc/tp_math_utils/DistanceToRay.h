#ifndef tp_math_utils_DistanceToRay_h
#define tp_math_utils_DistanceToRay_h

#include "tp_math_utils/Ray.h"

namespace tp_math_utils
{

//##################################################################################################
float TP_MATH_UTILS_EXPORT distanceToRay(const glm::vec3& point, const Ray& ray, glm::vec3& pointOnRay);

//##################################################################################################
float TP_MATH_UTILS_EXPORT distanceToLineSegment(const glm::vec3& point, const Ray& lineSegment, glm::vec3& pointOnRay);

//##################################################################################################
glm::vec3 TP_MATH_UTILS_EXPORT closestPointOnLineSegment(const glm::vec3& point, const Ray& lineSegment);

}

#endif
