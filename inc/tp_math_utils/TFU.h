#ifndef tp_math_utils_Transformation_h
#define tp_math_utils_Transformation_h

#include "tp_math_utils/Globals.h"

#include "glm/gtc/matrix_transform.hpp"

namespace tp_math_utils
{

//##################################################################################################
glm::mat4 tfuToViewMatrix(const glm::vec3& translation,
                          const glm::vec3& forward,
                          const glm::vec3& up,
                          const glm::vec3& scale={1.0f, 1.0f, 1.0f})
{
  auto f = glm::normalize(forward);
  auto u = glm::normalize(up);

  auto a = glm::cross(f, u);
  u = glm::cross(f, a);

  auto t = translation * scale;

  return glm::lookAt(t, t+f, -u);
}

}

#endif
