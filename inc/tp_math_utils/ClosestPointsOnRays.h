#ifndef tp_math_utils_ClosestPointsOnRays_h
#define tp_math_utils_ClosestPointsOnRays_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{
struct DRay;

//##################################################################################################
void closestPointsOnRays(const DRay& S1, const DRay& S2, glm::dvec3& P1, glm::dvec3& P2);

}

#endif
