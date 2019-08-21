#include "tp_math_utils/PlaneFromPoints.h"

namespace tp_math_utils
{

//##################################################################################################
//http://www.ilikebigbits.com/blog/2017/9/24/fitting-a-plane-to-noisy-points-in-3d
// Fit a plane to a collection of points.
// Fast, and accurate to within a few degrees.
// Returns None if the points do not span a plane.
Plane planeFromPoints(const std::vector<glm::vec3>& points)
{
  if(points.size()<3)
    return Plane();

  glm::dvec3 sum{0.0, 0.0, 0.0};
  double n = double(points.size());

  for(const glm::vec3& p : points)
    sum += p;

  glm::dvec3 centroid = sum * (1.0 / n);

  // Calculate full 3x3 covariance matrix, excluding symmetries:
  double xx = 0.0; double xy = 0.0; double xz = 0.0;
  double yy = 0.0; double yz = 0.0; double zz = 0.0;

  for(const glm::vec3& p : points)
  {
    glm::dvec3 r = glm::dvec3(p) - centroid;
    xx += r.x * r.x;
    xy += r.x * r.y;
    xz += r.x * r.z;
    yy += r.y * r.y;
    yz += r.y * r.z;
    zz += r.z * r.z;
  }

  xx /= n;
  xy /= n;
  xz /= n;
  yy /= n;
  yz /= n;
  zz /= n;

  glm::dvec3 weighted_dir{0.0, 0.0, 0.0};

  {
      double det_x = yy*zz - yz*yz;
      glm::dvec3 axis_dir{det_x, xz*yz - xy*zz, xy*yz - xz*yy};
      double weight = det_x * det_x;

      if(glm::dot(weighted_dir, axis_dir) < 0.0)
        weight = -weight;

      weighted_dir += axis_dir * weight;
  }

  {
      double det_y = xx*zz - xz*xz;
      glm::dvec3 axis_dir{xz*yz - xy*zz, det_y, xy*xz - yz*xx};
      double weight = det_y * det_y;

      if(glm::dot(weighted_dir, axis_dir) < 0.0)
        weight = -weight;

      weighted_dir += axis_dir * weight;
  }

  {
      double det_z = xx*yy - xy*xy;
      glm::dvec3 axis_dir{xy*yz - xz*yy, xy*xz - yz*xx, det_z};
      double weight = det_z * det_z;

      if(glm::dot(weighted_dir, axis_dir) < 0.0)
       weight = -weight;

      weighted_dir += axis_dir * weight;
  }

  glm::dvec3 normal = glm::normalize(weighted_dir);

  if(std::isinf(normal.x) || std::isinf(normal.y) ||std::isinf(normal.z) ||
     std::isnan(normal.x) || std::isnan(normal.y) ||std::isnan(normal.z))
    return Plane();

  return Plane(centroid, normal);
}

}
