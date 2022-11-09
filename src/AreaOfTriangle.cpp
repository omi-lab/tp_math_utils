#include "tp_math_utils/AreaOfTriangle.h"

namespace tp_math_utils
{

//##################################################################################################
float areaOfTriangle(const glm::vec3& a,
                     const glm::vec3& b,
                     const glm::vec3& c)
{
  glm::vec3 e1 = b-a;
  glm::vec3 e2 = c-a;
  glm::vec3 e3 = glm::cross(e1, e2);
  return glm::length(e3) * 0.5f;
}

//##################################################################################################
float areaOfTriangle(float a,
                     float b,
                     float c)
{
  float s = (a+b+c)/2.0f;
  return std::sqrt(s*(s-a)*(s-b)*(s-c));
}

//##################################################################################################
float areaOfTriangle(const glm::vec2& a,
                     const glm::vec2& b,
                     const glm::vec2& c)
{
  return areaOfTriangle(glm::distance(a, b), glm::distance(b, c), glm::distance(c, a));
}

}
