#include "tp_math_utils/AngleBetweenVectors.h"
#include "tp_math_utils/MidPointBetweenLines.h"
#include "tp_math_utils/Ray.h"

#include "tp_utils/Globals.h"

namespace tp_math_utils
{

//http://geomalgorithms.com/a07-_distance.html

//##################################################################################################
float angleBetweenVectors(const glm::vec3& a, const glm::vec3& b)
{
  glm::vec3 v1 = glm::normalize(a);
  glm::vec3 v2 = glm::normalize(b);

  float dot = tpBound(-1.0f, glm::dot(v1, v2), 1.0f);

  if(std::isnan(dot))
    return 0.0f;

  return std::acos(dot);
}

//##################################################################################################
double angleBetweenVectors(const glm::dvec3& a, const glm::dvec3& b)
{
  glm::dvec3 v1 = glm::normalize(a);
  glm::dvec3 v2 = glm::normalize(b);

  double dot = tpBound(-1.0, glm::dot(v1, v2), 1.0);

  if(std::isnan(dot))
    return 0.0;

  return std::acos(dot);
}

//##################################################################################################
float angleBetweenVectorsSigned(const glm::vec3& a, const glm::vec3& b, const glm::vec3& normal)
{
  glm::vec3 n = glm::normalize(normal);

  float x1 = a.x;
  float x2 = b.x;
  float xn = n.x;

  float y1 = a.y;
  float y2 = b.y;
  float yn = n.y;

  float z1 = a.z;
  float z2 = b.z;
  float zn = n.z;

  float dot = glm::dot(a, b);
  float det = x1*y2*zn + x2*yn*z1 + xn*y1*z2 - z1*y2*xn - z2*yn*x1 - zn*y1*x2;
  return std::atan2(det, dot);
}

//##################################################################################################
// Copyright 2001 softSurfer, 2012 Dan Sunday
// This code may be freely used, distributed and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
double angleToMidPointBetweenLines(const tp_math_utils::DRay& S1, const tp_math_utils::DRay& S2, glm::dvec3& midPoint)
{
  midPointBetweenLines(S1, S2, midPoint);
  return tp_math_utils::angleBetweenVectors(S1.p1, midPoint);
}

}
