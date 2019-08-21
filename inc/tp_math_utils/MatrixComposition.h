#ifndef tp_math_utils_MatrixComposition_h
#define tp_math_utils_MatrixComposition_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{
//##################################################################################################
void TP_MATH_UTILS_SHARED_EXPORT decomposeMatrix(glm::mat4 m, glm::vec3& translation, glm::vec3& scale, glm::mat4& rotation);

//##################################################################################################
void TP_MATH_UTILS_SHARED_EXPORT decomposeMatrix(glm::mat4 m, glm::vec3& translation, glm::vec3& scale, glm::vec3& rotation);

//##################################################################################################
void TP_MATH_UTILS_SHARED_EXPORT composeMatrix(glm::mat4& m, const glm::vec3 translation, const glm::vec3 scale, const glm::mat4 rotation);

//##################################################################################################
void TP_MATH_UTILS_SHARED_EXPORT composeMatrix(glm::mat4& m, const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation);

}

#endif
