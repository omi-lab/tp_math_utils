#ifndef tp_math_utils_Intersection_h
#define tp_math_utils_Intersection_h

#include "tp_math_utils/Globals.h"

#include <glm/glm.hpp>

namespace tp_math_utils
{
class Plane;
struct Ray;

//##################################################################################################
bool TP_MATH_UTILS_SHARED_EXPORT rayPlaneIntersection(const Ray& ray, const Plane& plane, glm::vec3& intersection);

}

#endif
