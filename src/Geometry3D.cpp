#include "tp_math_utils/Geometry3D.h"

#include "glm/gtx/norm.hpp"
#include "glm/gtx/normal.hpp"

namespace tp_math_utils
{

namespace
{
struct Face_lt
{
  int indexes[3];
  glm::vec3 normal;
};

//##################################################################################################
void calculateFaces(const Geometry3D& geometry, std::vector<Face_lt>& faces)
{
  if(geometry.indexes.empty())
    return;

  for(const auto& indexes : geometry.indexes)
  {
    auto calcVMax = [&](size_t sub)
    {
      return (sub>indexes.indexes.size())?0:indexes.indexes.size()-sub;
    };

    if(indexes.type == geometry.triangleFan)
    {
      const auto& f = indexes.indexes.front();

      size_t vMax = calcVMax(1);
      for(size_t v=1; v<vMax; v++)
      {
        Face_lt face;
        face.indexes[0] = f;
        face.indexes[1] = indexes.indexes.at(v);
        face.indexes[2] = indexes.indexes.at(v+1);
        faces.push_back(face);
      }
    }
    else if(indexes.type == geometry.triangleStrip)
    {
      size_t vMax = calcVMax(2);
      for(size_t v=0; v<vMax; v++)
      {
        Face_lt face;
        if(v&1)
        {
          face.indexes[0] = indexes.indexes.at(v);
          face.indexes[1] = indexes.indexes.at(v+2);
          face.indexes[2] = indexes.indexes.at(v+1);
        }
        else
        {
          face.indexes[0] = indexes.indexes.at(v);
          face.indexes[1] = indexes.indexes.at(v+1);
          face.indexes[2] = indexes.indexes.at(v+2);
        }
        faces.push_back(face);
      }
    }
    else if(indexes.type == geometry.triangles)
    {
      size_t vMax = calcVMax(2);
      for(size_t v=0; v<vMax; v+=3)
      {
        Face_lt face;
        face.indexes[0] = indexes.indexes.at(v);
        face.indexes[1] = indexes.indexes.at(v+1);
        face.indexes[2] = indexes.indexes.at(v+2);
        faces.push_back(face);
      }
    }
  }

  for(auto& face : faces)
    face.normal = glm::triangleNormal(geometry.verts.at(size_t(face.indexes[0])).vert,
                                      geometry.verts.at(size_t(face.indexes[1])).vert,
                                      geometry.verts.at(size_t(face.indexes[2])).vert);
}
}

//##################################################################################################
void Geometry3D::calculateVertexNormals()
{
  for(auto& vert : verts)
    vert.normal = {0.0f, 0.0f, 0.0f};

  std::vector<Face_lt> faces;
  calculateFaces(*this, faces);

  for(const auto& face : faces)
    for(size_t i=0; i<3; i++)
      verts[size_t(face.indexes[i])].normal += face.normal;

  for(auto& vert : verts)
  {
    if(glm::length2(vert.normal)>0.001f)
      vert.normal = glm::normalize(vert.normal);
    else
      vert.normal = {0.0f, 0.0f, 1.0f};
  }
}

//##################################################################################################
void Geometry3D::calculateFaceNormals()
{
  std::vector<Face_lt> faces;
  calculateFaces(*this, faces);

  std::vector<Vertex3D> newVerts;
  indexes.clear();
  Indexes3D& newIndexes = indexes.emplace_back();
  newIndexes.type = triangles;
  newVerts.reserve(faces.size()*3);
  newIndexes.indexes.resize(faces.size()*3);
  for(size_t i=0; i<newIndexes.indexes.size(); i++)
    newIndexes.indexes[i] = int(i);
  for(const auto& face : faces)
  {
    for(size_t i=0; i<3; i++)
    {
      auto& v = newVerts.emplace_back();
      v = verts[size_t(face.indexes[i])];
      v.normal = face.normal;
    }
  }

  verts = std::move(newVerts);
}

}
