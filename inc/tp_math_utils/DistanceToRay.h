#ifndef tp_math_utils_DistanceToRay_h
#define tp_math_utils_DistanceToRay_h

#include "tp_math_utils/Ray.h"

namespace tp_math_utils
{

//##################################################################################################
float distanceToRay(const glm::vec3& point, const Ray& ray, glm::vec3& pointOnRay);

}

#endif
