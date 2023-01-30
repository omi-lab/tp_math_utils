#ifndef tp_math_utils_AngleBetweenVectors_h
#define tp_math_utils_AngleBetweenVectors_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{
struct DRay;

//##################################################################################################
[[nodiscard]] float TP_MATH_UTILS_EXPORT angleBetweenVectors(const glm::vec3& a, const glm::vec3& b);

//##################################################################################################
[[nodiscard]] float TP_MATH_UTILS_EXPORT angleBetweenUnitVectors(const glm::vec3& v1, const glm::vec3& v2);

//##################################################################################################
[[nodiscard]] double TP_MATH_UTILS_EXPORT angleBetweenVectors(const glm::dvec3& a, const glm::dvec3& b);

//##################################################################################################
[[nodiscard]] float TP_MATH_UTILS_EXPORT angleBetweenVectorsSigned(const glm::vec3& a, const glm::vec3& b, const glm::vec3& normal);

//##################################################################################################
[[nodiscard]] double TP_MATH_UTILS_EXPORT angleToMidPointBetweenLines(const DRay& S1, const DRay& S2, glm::dvec3& midPoint);
}

#endif
