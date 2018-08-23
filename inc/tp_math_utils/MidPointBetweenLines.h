#ifndef tp_math_utils_MidPointBetweenLines_h
#define tp_math_utils_MidPointBetweenLines_h

#include <glm/glm.hpp>

namespace tp_math_utils
{
struct DRay;

//##################################################################################################
void midPointBetweenLines(const DRay& S1, const DRay& S2, glm::dvec3& midPoint);

}

#endif
