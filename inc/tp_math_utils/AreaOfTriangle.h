#ifndef tp_math_utils_AreaOfTriangle_h
#define tp_math_utils_AreaOfTriangle_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{

//##################################################################################################
float TP_MATH_UTILS_EXPORT areaOfTriangle(const glm::vec3& a,
                                                 const glm::vec3& b,
                                                 const glm::vec3& c);

//##################################################################################################
//! Area of triangle from side lengths, Heron's Formula.
float TP_MATH_UTILS_EXPORT areaOfTriangle(float a,
                                                 float b,
                                                 float c);

//##################################################################################################
float TP_MATH_UTILS_EXPORT areaOfTriangle(const glm::vec2& a,
                                                 const glm::vec2& b,
                                                 const glm::vec2& c);

}

#endif
