#ifndef tp_math_utils_ClosestPointsOnLines_h
#define tp_math_utils_ClosestPointsOnLines_h

#include <glm/glm.hpp>

namespace tp_math_utils
{
struct DRay;

//##################################################################################################
void closestPointsOnLines(const DRay& S1, const DRay& S2, glm::dvec3& P1, glm::dvec3& P2);

}

#endif
