#include "tp_math_utils/Plane.h"

namespace tp_math_utils
{

//##################################################################################################
Plane::Plane():
  m_pointAndNormalValid(true),
  m_threePointsValid(true)
{
  m_pointAndNormal[0] = glm::vec3(0.0f, 0.0f, 0.0f);
  m_pointAndNormal[1] = glm::vec3(0.0f, 0.0f, 1.0f);

  m_threePoints[0] = glm::vec3(0.0f, 0.0f, 0.0f);
  m_threePoints[1] = glm::vec3(1.0f, 0.0f, 0.0f);
  m_threePoints[2] = glm::vec3(0.0f, 1.0f, 0.0f);
}

//##################################################################################################
Plane::Plane(const glm::vec3& point, const glm::vec3& normal):
  m_pointAndNormalValid(true),
  m_threePointsValid(false)
{
  m_pointAndNormal[0] = point;
  m_pointAndNormal[1] = normal;

  m_threePoints[0] = glm::vec3(0.0f, 0.0f, 0.0f);
  m_threePoints[1] = glm::vec3(0.0f, 0.0f, 0.0f);
  m_threePoints[2] = glm::vec3(0.0f, 0.0f, 0.0f);
}

//##################################################################################################
Plane::Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3):
  m_pointAndNormalValid(false),
  m_threePointsValid(true)
{
  m_pointAndNormal[0] = glm::vec3(0.0f, 0.0f, 0.0f);
  m_pointAndNormal[1] = glm::vec3(0.0f, 0.0f, 0.0f);

  m_threePoints[0] = p1;
  m_threePoints[1] = p2;
  m_threePoints[2] = p3;
}

//##################################################################################################
const glm::vec3* Plane::pointAndNormal()const
{
  if(!m_pointAndNormalValid)
  {
    m_pointAndNormal[0] = m_threePoints[0];

    const glm::vec3& a = m_threePoints[0];
    const glm::vec3& b = m_threePoints[1];
    const glm::vec3& c = m_threePoints[2];
    m_pointAndNormal[1] = glm::normalize(glm::cross(c - a, b - a));

    m_pointAndNormalValid = true;
  }

  return m_pointAndNormal;
}

//##################################################################################################
const glm::vec3* Plane::threePoints()const
{
  if(!m_threePointsValid)
  {
    glm::vec3 normal = m_pointAndNormal[1];
    normal.x = std::fabs(normal.x);
    normal.y = std::fabs(normal.y);
    normal.z = std::fabs(normal.z);

    //This is used to give us a cross product
    glm::vec3 cardinal;

    //Find the shortest component
    if(normal.x<normal.y)
    {
      if(normal.x<normal.z)
        cardinal = glm::vec3(1.0f, 0.0f, 0.0f);
      else
        cardinal = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    else
    {
      if(normal.y<normal.z)
        cardinal = glm::vec3(0.0f, 1.0f, 0.0f);
      else
        cardinal = glm::vec3(0.0f, 0.0f, 1.0f);
    }

    m_threePoints[0] = m_pointAndNormal[0];
    m_threePoints[1] = glm::cross(m_pointAndNormal[1], cardinal);
    m_threePoints[2] = glm::cross(m_pointAndNormal[1], m_threePoints[1]);
    m_threePoints[1] = glm::cross(m_pointAndNormal[1], m_threePoints[2]);
    m_threePoints[1] += m_threePoints[0];
    m_threePoints[2] += m_threePoints[0];

    m_threePointsValid = true;
  }

  return m_threePoints;
}

}
