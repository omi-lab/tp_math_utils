#include "tp_math_utils/DistanceToPlane.h"

#include "glm/gtx/norm.hpp"

namespace tp_math_utils
{
// Copyright 2001 softSurfer, 2012 Dan Sunday
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.

//##################################################################################################
float distanceToPlane(const glm::vec3& point, const Plane& plane, glm::vec3& pointOnPlane)
{
  return std::sqrt(squaredDistanceToPlane(point, plane, pointOnPlane));
}

//##################################################################################################
float squaredDistanceToPlane(const glm::vec3& point, const Plane& plane, glm::vec3& pointOnPlane)
{
  glm::vec3 PLV0 = plane.pointAndNormal()[0];
  glm::vec3 PLn  = plane.pointAndNormal()[1];

  float sn = -glm::dot(PLn, (point - PLV0));
  float sd = glm::dot(PLn, PLn);
  float sb = sn / sd;

  pointOnPlane = point + sb * PLn;
  return glm::distance2(point, pointOnPlane);
}

//##################################################################################################
double summedSquaredDistanceToPlane(const std::vector<glm::vec3>& points, const Plane& plane)
{
  glm::vec3 pointOnPlane{0.0f, 0.0f, 0.0f};
  double sum=0.0;
  for(const glm::vec3& point : points)
    sum += double(squaredDistanceToPlane(point, plane, pointOnPlane));
  return sum;
}

}
