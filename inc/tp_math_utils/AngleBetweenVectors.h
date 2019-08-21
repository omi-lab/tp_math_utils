#ifndef tp_math_utils_AngleBetweenVectors_h
#define tp_math_utils_AngleBetweenVectors_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{
struct DRay;

//##################################################################################################
float TP_MATH_UTILS_SHARED_EXPORT angleBetweenVectors(const glm::vec3& a, const glm::vec3& b);

//##################################################################################################
double TP_MATH_UTILS_SHARED_EXPORT angleBetweenVectors(const glm::dvec3& a, const glm::dvec3& b);

//##################################################################################################
float TP_MATH_UTILS_SHARED_EXPORT angleBetweenVectorsSigned(const glm::vec3& a, const glm::vec3& b, const glm::vec3& normal);

//##################################################################################################
double TP_MATH_UTILS_SHARED_EXPORT angleToMidPointBetweenLines(const DRay& S1, const DRay& S2, glm::dvec3& midPoint);
}

#endif
