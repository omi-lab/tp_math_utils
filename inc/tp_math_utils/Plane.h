#ifndef tp_math_utils_Plane_h
#define tp_math_utils_Plane_h

#include "tp_math_utils/Globals.h"

#include <glm/glm.hpp>

namespace tp_math_utils
{

//##################################################################################################
class TP_MATH_UTILS_SHARED_EXPORT Plane
{
public:
  //################################################################################################
  //! Define a plane facing up the y axis
  Plane();

  //################################################################################################
  //! Define a plane with an origin point and a normal
  Plane(const glm::vec3& point, const glm::vec3& normal);

  //################################################################################################
  //! Define a plane using three points on the surface of the plane
  Plane(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);


  //################################################################################################
  //! Return the plane as an array with a point and a normal
  /*!
  This will retuns the plane described as an origin point and a normal, it will calculate these
  values if they have not yet been set.

  \return An array with two elements in it
  */
  const glm::vec3* pointAndNormal()const;

  //################################################################################################
  //! Define a plane using three points on the surface of the plane
  const glm::vec3* threePoints()const;

private:
  mutable glm::vec3 m_pointAndNormal[2];
  mutable bool m_pointAndNormalValid;

  mutable glm::vec3 m_threePoints[3];
  mutable bool m_threePointsValid;
};

}

#endif
