#include "tp_math_utils/MatrixComposition.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace tp_math_utils
{

//##################################################################################################
void decomposeMatrix(glm::mat4 m, glm::vec3& translation, glm::vec3& scale, glm::mat4& rotation)
{
  float* f = glm::value_ptr(m);
  translation.x = f[12];
  translation.y = f[13];
  translation.z = f[14];

  f[12] = 0.0f;
  f[13] = 0.0f;
  f[14] = 0.0f;

  scale.x = glm::length(glm::vec3{f[0], f[1], f[2]});
  scale.y = glm::length(glm::vec3{f[4], f[5], f[6]});
  scale.z = glm::length(glm::vec3{f[8], f[9], f[10]});

  f[0]/=scale.x;
  f[1]/=scale.x;
  f[2]/=scale.x;

  f[4]/=scale.y;
  f[5]/=scale.y;
  f[6]/=scale.y;

  f[8]/=scale.z;
  f[9]/=scale.z;
  f[10]/=scale.z;

  rotation = m;
}

//##################################################################################################
void decomposeMatrix(glm::mat4 m, glm::vec3& translation, glm::vec3& scale, glm::vec3& rotation)
{
  glm::mat4 r;
  decomposeMatrix(m, translation, scale, r);

  float* f = glm::value_ptr(r);

  rotation.x = std::atan2(f[6], f[10]);
  rotation.y = std::atan2(-f[2], std::sqrt((f[6]*f[6])+(f[10]*f[10])));
  rotation.z = std::atan2(f[1], f[0]);
}

//##################################################################################################
void composeMatrix(glm::mat4& m, const glm::vec3 translation, const glm::vec3 scale, const glm::mat4 rotation)
{
  float* f = glm::value_ptr(m);

  m = rotation;

  f[0]*=scale.x;
  f[1]*=scale.x;
  f[2]*=scale.x;

  f[4]*=scale.y;
  f[5]*=scale.y;
  f[6]*=scale.y;

  f[8]*=scale.z;
  f[9]*=scale.z;
  f[10]*=scale.z;

  f[12] = translation.x;
  f[13] = translation.y;
  f[14] = translation.z;
}

//##################################################################################################
void composeMatrix(glm::mat4& m, const glm::vec3& translation, const glm::vec3& scale, const glm::vec3& rotation)
{
  m = glm::translate(glm::mat4{1.0f}, translation);
  m = glm::scale(m, scale);
  m = glm::rotate(m, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
  m = glm::rotate(m, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
  m = glm::rotate(m, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
}

}
