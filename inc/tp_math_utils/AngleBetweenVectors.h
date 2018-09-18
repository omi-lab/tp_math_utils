#ifndef tp_math_utils_AngleBetweenVectors_h
#define tp_math_utils_AngleBetweenVectors_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{
struct DRay;

//##################################################################################################
float angleBetweenVectors(const glm::vec3& a, const glm::vec3& b);

//##################################################################################################
double angleBetweenVectors(const glm::dvec3& a, const glm::dvec3& b);

//##################################################################################################
float angleBetweenVectorsSigned(const glm::vec3& a, const glm::vec3& b, const glm::vec3& normal);

//##################################################################################################
double angleToMidPointBetweenLines(const DRay& S1, const DRay& S2, glm::dvec3& midPoint);
}

#endif
