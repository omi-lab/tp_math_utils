#include "tp_math_utils/Sphere.h"

#include "glm/gtx/compatibility.hpp"

namespace tp_math_utils
{

//##################################################################################################
//Geometry3D Sphere::icosahedralClass1(float radius,
//                                     size_t division,
//                                     int triangleFan,
//                                     int triangleStrip,
//                                     int triangles)
//{
//  std::vector<glm::vec3> verts;
//  return indexAndScale(radius, triangleFan, triangleStrip, triangles, verts);
//}

//##################################################################################################
Geometry3D Sphere::octahedralClass1(float radius,
                                    size_t division,
                                    int triangleFan,
                                    int triangleStrip,
                                    int triangles)
{
  std::vector<glm::vec3> verts;

  divideClass1(division, { 0,-1, 0}, { 1, 0, 0}, { 0, 0, 1}, verts);
  divideClass1(division, {-1, 0, 0}, { 0,-1, 0}, { 0, 0, 1}, verts);
  divideClass1(division, { 0, 1, 0}, {-1, 0, 0}, { 0, 0, 1}, verts);
  divideClass1(division, { 1, 0, 0}, { 0, 1, 0}, { 0, 0, 1}, verts);

  divideClass1(division, { 1, 0, 0}, { 0,-1, 0}, { 0, 0,-1}, verts);
  divideClass1(division, { 0,-1, 0}, {-1, 0, 0}, { 0, 0,-1}, verts);
  divideClass1(division, {-1, 0, 0}, { 0, 1, 0}, { 0, 0,-1}, verts);
  divideClass1(division, { 0, 1, 0}, { 1, 0, 0}, { 0, 0,-1}, verts);

  return indexAndScale(radius, triangleFan, triangleStrip, triangles, verts);
}

//##################################################################################################
Geometry3D Sphere::tetrahedralClass1(float radius,
                                     size_t division,
                                     int triangleFan,
                                     int triangleStrip,
                                     int triangles)
{
  std::vector<glm::vec3> verts;

  divideClass1(division, {-1,-1,-1}, { 1,-1, 1}, {-1, 1, 1}, verts);
  divideClass1(division, {-1,-1,-1}, { 1, 1,-1}, { 1,-1, 1}, verts);
  divideClass1(division, {-1,-1,-1}, {-1, 1, 1}, { 1, 1,-1}, verts);
  divideClass1(division, { 1,-1, 1}, { 1, 1,-1}, {-1, 1, 1}, verts);

  return indexAndScale(radius, triangleFan, triangleStrip, triangles, verts);
}

//##################################################################################################
void Sphere::divideClass1(size_t division,
                          const glm::vec3& a,
                          const glm::vec3& b,
                          const glm::vec3& c,
                          std::vector<glm::vec3>& verts)
{
  if(division==0)
    return;

  double stepI = 1.0 / double(division);

  std::vector<glm::vec3> prev;
  prev.push_back(a);
  for(size_t i=0; i<division; i++)
  {
    glm::vec3 bb = glm::lerp(a, b, float(stepI*double(i+1)));
    glm::vec3 cc = glm::lerp(a, c, float(stepI*double(i+1)));

    double stepJ = 1.0 / double(i+1);

    std::vector<glm::vec3> next;
    next.push_back(bb);

    for(size_t j=0; j<i; j++)
      next.push_back(glm::lerp(bb, cc, float(stepJ*double(j+1))));

    next.push_back(cc);

    for(size_t c=0; c<prev.size(); c++)
    {
      verts.push_back(prev.at(c));
      verts.push_back(next.at(c));
      verts.push_back(next.at(c+1));
    }

    for(size_t c=1; c<prev.size(); c++)
    {
      verts.push_back(prev.at(c-1));
      verts.push_back(next.at(c));
      verts.push_back(prev.at(c));
    }

    prev.swap(next);
  }
}

//##################################################################################################
Geometry3D Sphere::indexAndScale(float radius,
                                 int triangleFan,
                                 int triangleStrip,
                                 int triangles,
                                 const std::vector<glm::vec3>& verts)
{
  Geometry3D geometry;

  geometry.triangleFan   = triangleFan;
  geometry.triangleStrip = triangleStrip;
  geometry.triangles     = triangles;

  auto& indexes = geometry.indexes.emplace_back().indexes;
  geometry.indexes.back().type = geometry.triangles;

  float e=0.000001f;
  for(const auto& vert : verts)
  {
    int index=-1;

    for(size_t i=0; i<geometry.verts.size(); i++)
    {
      const auto& v = geometry.verts.at(i).vert;
      if(std::fabs(v.x-vert.x)<e &&
         std::fabs(v.y-vert.y)<e &&
         std::fabs(v.z-vert.z)<e)
      {
        index = int(i);
        break;
      }
    }

    if(index==-1)
    {
      index = int(geometry.verts.size());
      geometry.verts.emplace_back().vert = vert;
    }

    indexes.push_back(index);
  }

  for(auto& vert : geometry.verts)
    vert.vert = glm::normalize(vert.vert) * radius;

  return geometry;
}

}

