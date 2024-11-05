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

}

#endif
