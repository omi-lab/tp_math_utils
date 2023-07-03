#ifndef tp_math_utils_Intersection_h
#define tp_math_utils_Intersection_h

#include "tp_math_utils/Plane.h"
#include "tp_math_utils/Ray.h"

namespace tp_math_utils
{

//##################################################################################################
/*!
Return false if ray is parallel to the plane (including ray belongs to the plane),
true otherwise with intersection parameter filled.
*/

#ifndef ALEX_BLINOV_DEBUG_OFF // some new imlementation (old one is left below for reference) remove it later!

template<typename FLOAT_TYPE>
bool rayPlaneIntersectionImpl(const Ray& ray, const Plane& plane, glm::vec<3,FLOAT_TYPE>& intersection)
{
  const auto& Po = plane.pointAndNormal()[0];
  const auto& Pn = plane.pointAndNormal()[1];
  const auto& Ro = ray.p0;
  const auto  Rd = ray.p1-ray.p0;

  auto denom = glm::dot(Po-Ro,Pn);
  if(std::fabs(denom) < 1e-10f)
  {
    intersection  = Po;
    return true;
  }
  auto Pp = Ro + (denom/glm::dot(Pn,Pn))*Pn;

  auto nom = glm::dot(Pp-Ro,Pp-Ro);
  if( std::fabs(denom) < 1e-10f)
  {
    intersection = Pp;
    return true;
  }

  denom = glm::dot(Rd,Pp-Ro);
  if(std::fabs(denom) < 1e-10f)
    return false;

  intersection = Ro + (nom/denom)*Rd;
  return true;
}

//##################################################################################################
bool rayPlaneIntersection(const Ray& ray, const Plane& plane, glm::vec3& intersection)
{
  return rayPlaneIntersectionImpl(ray, plane, intersection);
}

//##################################################################################################
bool rayPlaneIntersection(const Ray& ray, const Plane& plane, glm::dvec3& intersection)
{
  return rayPlaneIntersectionImpl(ray, plane, intersection);
}

#else //here is old implementation

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

  // Return now if ray is parallel to the plane (i.e. they will never intersect or ray belongs to the plane).
  if(std::fabs(vRotRay1.z - vRotRay2.z)<0.0001f)
    return false;

  // Find 2D plane coordinates (fX, fY) that the ray intersects with.
  float fPercent = vRotRay1.z / (vRotRay2.z-vRotRay1.z);
//  glm::vec3 vIntersect2d = vRotRay1 + (vRotRay1-vRotRay2) * fPercent;
//  //fX = vIntersect2d.x;
//  //fY = vIntersect2d.y;

  // Find the 3D world-space coordinates of the intersection point.
  intersection = r1 + (r1-r2) * fPercent;
  return true;
}

//##################################################################################################
/*!
Return false if ray is parallel to the plane (including ray belongs to the plane),
true otherwise with intersection parameter filled.
*/
bool rayPlaneIntersection(const Ray& ray, const Plane& plane, glm::dvec3& intersection)
{
  const glm::vec3* pp = plane.threePoints();
  const glm::dvec3 p[3] = {pp[0], pp[1], pp[2]};

  // Returns in (fX, fY) the location on the plane (P1,P2,P3) of the intersection with the ray (R1, R2)
  // First compute the axes
  glm::dvec3 V1 = p[1] - p[0];
  glm::dvec3 V2 = p[2] - p[0];
  glm::dvec3 V3 = glm::cross(V1, V2);

  const glm::dvec3 r1 = ray.p0;
  const glm::dvec3 r2 = ray.p1;

  // Project ray points r1 and r2 onto the axes of the plane. (This is equivalent to a rotation.)
  glm::dvec3 vRotRay1(glm::dot(V1, r1-p[0]), glm::dot(V2, r1-p[0]), glm::dot(V3, r1-p[0]));
  glm::dvec3 vRotRay2(glm::dot(V1, r2-p[0]), glm::dot(V2, r2-p[0]), glm::dot(V3, r2-p[0]));

  // Return now if ray will never intersect plane (they're parallel)
  if(std::fabs(vRotRay1.z - vRotRay2.z)<0.0001)
    return false;

  // Find 2D plane coordinates (fX, fY) that the ray intersects with.
  double fPercent = vRotRay1.z / (vRotRay2.z-vRotRay1.z);
//  glm::vec3 vIntersect2d = vRotRay1 + (vRotRay1-vRotRay2) * fPercent;
//  //fX = vIntersect2d.x;
//  //fY = vIntersect2d.y;

  // Find the 3D world-space coordinates of the intersection point.
  intersection = r1 + (r1-r2) * fPercent;
  return true;
}

#endif
}

#endif
