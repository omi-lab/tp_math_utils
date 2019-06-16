#ifndef tp_math_utils_Intersection_h
#define tp_math_utils_Intersection_h

#include "tp_math_utils/Plane.h"
#include "tp_math_utils/Ray.h"

namespace tp_math_utils
{

//##################################################################################################
bool rayPlaneIntersection(const Ray& ray, const Plane& plane, glm::vec3& intersection)
{
  const glm::vec3* p = plane.threePoints();

  // Returns in (fX, fY) the location on the plane (P1,P2,P3) of the intersection with the ray (R1, R2)
  // First compute the axes
  glm::vec3 V1 = p[1] - p[0];
  glm::vec3 V2 = p[2] - p[0];
  glm::vec3 V3 = glm::cross(V1, V2);

  const glm::vec3& r1 = ray.p0;
  const glm::vec3& r2 = ray.p1;

  // Project ray points r1 and r2 onto the axes of the plane. (This is equivalent to a rotation.)
  glm::vec3 vRotRay1(glm::dot(V1, r1-p[0]), glm::dot(V2, r1-p[0]), glm::dot(V3, r1-p[0]));
  glm::vec3 vRotRay2(glm::dot(V1, r2-p[0]), glm::dot(V2, r2-p[0]), glm::dot(V3, r2-p[0]));

  // Return now if ray will never intersect plane (they're parallel)
  if(std::fabs(vRotRay1.z - vRotRay2.z)<0.0001f)
    return false;

  // Find 2D plane coordinates (fX, fY) that the ray interesects with
  float fPercent = vRotRay1.z / (vRotRay2.z-vRotRay1.z);
//  glm::vec3 vIntersect2d = vRotRay1 + (vRotRay1-vRotRay2) * fPercent;
//  //fX = vIntersect2d.x;
//  //fY = vIntersect2d.y;

  // Note that to find the 3D point on the world-space ray use this
  intersection = r1 + (r1-r2) * fPercent;
  return true;
}


}

#endif
