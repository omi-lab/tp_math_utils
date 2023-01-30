#ifndef tp_math_utils_Cone_h
#define tp_math_utils_Cone_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{

//##################################################################################################
/*!
The cone is defined as a line segment with a radius at each end. Setting the radius to the same
value at both ends defines a cylinder.
*/
struct TP_MATH_UTILS_EXPORT Cone
{
  Cone(const glm::vec3& p0_={0.0f, 0.0f, 0.0f}, float r0_=0.0f, const glm::vec3& p1_={0.0f, 0.0f, 0.0f}, float r1_=0.0f):
    p0(p0_),
    p1(p1_),
    r0(r0_),
    r1(r1_)
  {
  }

  glm::vec3 p0;
  glm::vec3 p1;

  float r0;
  float r1;
};

//##################################################################################################
/*!
The cone is defined as a line segment with a radius at each end. Setting the radius to the same
value at both ends defines a cylinder.
*/
struct TP_MATH_UTILS_EXPORT DCone
{
  DCone(const glm::dvec3& p0_={0.0, 0.0, 0.0}, double r0_=0.0, const glm::dvec3& p1_={0.0, 0.0, 0.0}, double r1_=0.0):
    p0(p0_),
    p1(p1_),
    r0(r0_),
    r1(r1_)
  {
  }

  glm::dvec3 p0;
  glm::dvec3 p1;

  double r0;
  double r1;
};

}

#endif
