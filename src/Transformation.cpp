#include "tp_math_utils/Transformation.h"

#include "glm/gtx/rotate_vector.hpp"

namespace tp_math_utils
{

//##################################################################################################
Transformation::Transformation():
  m_upVector(1.0f, 0.0f, 0.0f),
  m_scale(1.0f, 1.0f, 1.0f),
  m_matrixValid(false),
  m_matrix()
{

}

//##################################################################################################
void Transformation::setPosition(const glm::vec3& position)
{
  m_plane = tp_math_utils::Plane(position, m_plane.pointAndNormal()[1]);
  m_matrixValid = false;
}

//##################################################################################################
const glm::vec3& Transformation::position() const
{
  return m_plane.pointAndNormal()[0];
}

//##################################################################################################
void Transformation::setScale(const glm::vec3& scale)
{
  m_scale = scale;
  m_matrixValid = false;
}

//##################################################################################################
const glm::vec3& Transformation::scale() const
{
  return m_scale;
}

//##################################################################################################
void Transformation::setRotation(const glm::vec3& normal, const glm::vec3& upVector)
{
  m_plane = tp_math_utils::Plane(m_plane.pointAndNormal()[0], normal);
  m_upVector = upVector;
  m_matrixValid = false;
}

//##################################################################################################
const glm::vec3& Transformation::normal() const
{
  return m_plane.pointAndNormal()[1];
}

//##################################################################################################
const glm::vec3& Transformation::upVector() const
{
  return m_upVector;
}

//##################################################################################################
const glm::mat4& Transformation::matrix() const
{
  if(!m_matrixValid)
  {
    m_matrixValid = true;

    m_matrix[0] = glm::vec4(m_upVector,                                          0.0f);
    m_matrix[1] = glm::vec4(glm::cross(m_plane.pointAndNormal()[1], m_upVector), 0.0f);
    m_matrix[2] = glm::vec4(m_plane.pointAndNormal()[1],                         0.0f);
    m_matrix[3] = glm::vec4(0.0f, 0.0f, 0.0f,                                    1.0f);

    m_matrix = glm::scale(glm::mat4(), m_scale) * m_matrix;
    m_matrix = glm::translate(glm::mat4(), m_plane.pointAndNormal()[0]) * m_matrix;
  }

  return m_matrix;
}

//##################################################################################################
const Plane& Transformation::plane() const
{
  return m_plane;
}

}
