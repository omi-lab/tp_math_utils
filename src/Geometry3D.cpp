#include "tp_math_utils/Geometry3D.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/FileUtils.h"
#include "tp_utils/DebugUtils.h"

#include "nanoflann.hpp"

#include "glm/gtx/normal.hpp" // IWYU pragma: keep
#include "glm/gtx/norm.hpp" // IWYU pragma: keep

#include <array>
#include <sstream>

namespace tp_math_utils
{

namespace
{
struct Face_lt
{
  std::array<int, 3> indexes{};
  glm::vec3 normal{};
};

//##################################################################################################
std::vector<Face_lt> calculateFaces(const Geometry3D& geometry, bool calculateNormals)
{
  std::vector<Face_lt> faces;

  size_t count=0;
  for(const auto& indexes : geometry.indexes)
  {
    if(indexes.indexes.size()<3)
      continue;

    if(indexes.type == geometry.triangleFan)
      count+=indexes.indexes.size()-2;
    else if(indexes.type == geometry.triangleStrip)
      count+=indexes.indexes.size()-2;
    else if(indexes.type == geometry.triangles)
      count+=indexes.indexes.size()/3;
  }

  faces.reserve(count);

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
        Face_lt& face = faces.emplace_back();
        face.indexes[0] = f;
        face.indexes[1] = indexes.indexes.at(v);
        face.indexes[2] = indexes.indexes.at(v+1);
      }
    }
    else if(indexes.type == geometry.triangleStrip)
    {
      size_t vMax = calcVMax(2);
      for(size_t v=0; v<vMax; v++)
      {
        Face_lt& face = faces.emplace_back();
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
      }
    }
    else if(indexes.type == geometry.triangles)
    {
      size_t vMax = calcVMax(2);
      for(size_t v=0; v<vMax; v+=3)
      {
        Face_lt& face = faces.emplace_back();
        face.indexes[0] = indexes.indexes.at(v);
        face.indexes[1] = indexes.indexes.at(v+1);
        face.indexes[2] = indexes.indexes.at(v+2);
      }
    }
  }

  if(calculateNormals)
    for(auto& face : faces)
      face.normal = glm::triangleNormal(geometry.verts.at(size_t(face.indexes[0])).vert,
          geometry.verts.at(size_t(face.indexes[1])).vert,
          geometry.verts.at(size_t(face.indexes[2])).vert);

  return faces;
}

//################################################################################################
void accumulateTangentForTriangle(Vertex3DList const& verts, Indexes3D const& part, size_t i1, size_t i2, size_t i3, std::vector<glm::vec3>& tangent)
{
  auto idx1 = size_t(part.indexes.at(i1));
  auto idx2 = size_t(part.indexes.at(i2));
  auto idx3 = size_t(part.indexes.at(i3));
  if(idx1<verts.size() && idx2<verts.size() && idx3<verts.size())
  {
    const auto& v1 = verts.at(idx1);
    const auto& v2 = verts.at(idx2);
    const auto& v3 = verts.at(idx3);
    auto posdiff21 = v2.vert-v1.vert;
    auto posdiff31 = v3.vert-v1.vert;
    auto posdiff32 = v3.vert-v2.vert;
    auto texdiff21 = v2.texture-v1.texture;
    auto texdiff31 = v3.texture-v1.texture;
    auto texdiff32 = v3.texture-v2.texture;
    glm::vec3 t = {0,0,0};
    const float smallVal = 1.e-4f;
    const float verySmallVal = 1.e-6f;
    if(verySmallVal > glm::abs(texdiff21.y) && smallVal < glm::abs(texdiff21.x))
      // use points 1 and 2
      t = (texdiff31.x < 0.f) ? -posdiff21 : posdiff21;
    else if(verySmallVal > glm::abs(texdiff31.y) && smallVal < glm::abs(texdiff31.x))
      // use points 1 and 3
      t = (texdiff31.x < 0.f) ? -posdiff31 : posdiff31;
    else if(verySmallVal > glm::abs(texdiff32.y) && smallVal < glm::abs(texdiff32.x))
      // use points 2 and 3
      t = (texdiff32.x < 0.f) ? -posdiff32 : posdiff32;
    else if(smallVal < glm::abs(texdiff32.y) || smallVal < glm::abs(texdiff31.y) || smallVal < glm::abs(texdiff21.y))
    {
      if(glm::abs(texdiff32.y) > glm::abs(texdiff31.y) && glm::abs(texdiff32.y) > glm::abs(texdiff21.y))
      {
        // interpolate relative to point 1
        float alpha = texdiff31.y/texdiff32.y;
        float ud = alpha*texdiff21.x + (1.f-alpha)*texdiff31.x;
        if(smallVal < glm::abs(ud))
        {
          t = alpha*posdiff21 + (1.f-alpha)*posdiff31;
          if (ud < 0.f) t = -t;
        }
      }
      else if(glm::abs(texdiff31.y) > glm::abs(texdiff21.y))
      {
        // interpolate relative to point 2
        float alpha = texdiff21.y/texdiff31.y;
        float ud = alpha*texdiff32.x - (1.f-alpha)*texdiff21.x;
        if(smallVal < glm::abs(ud))
        {
          t = alpha*posdiff32 - (1.f-alpha)*posdiff21;
          if (ud < 0.f) t = -t;
        }
      }
      else
      {
        // interpolate relative to point 3
        float alpha = -texdiff32.y/texdiff21.y;
        float ud = -alpha*texdiff31.x - (1.f-alpha)*texdiff32.x;
        if(smallVal < glm::abs(ud))
        {
          t = -alpha*posdiff31 - (1.f-alpha)*posdiff32;
          if (ud < 0.f) t = -t;
        }
      }
#if 0
      // check results are the same
      glm::vec3 t21, t31, t32;
      {
        // 1
        float alpha = texdiff31.y/texdiff32.y;
        float ud = alpha*texdiff21.x + (1.f-alpha)*texdiff31.x;
        t32 = alpha*posdiff21 + (1.f-alpha)*posdiff31;
        if (ud < 0.f) t32 = -t32;
      }
      {
        // 2
        float alpha = texdiff21.y/texdiff31.y;
        float ud = alpha*texdiff32.x - (1.f-alpha)*texdiff21.x;
        t21 = alpha*posdiff32 - (1.f-alpha)*posdiff21;
        if (ud < 0.f) t21 = -t21;
      }
      {
        // 3
        float alpha = -texdiff32.y/texdiff21.y;
        float ud = -alpha*texdiff31.x - (1.f-alpha)*texdiff32.x;
        t31 = -alpha*posdiff31 - (1.f-alpha)*posdiff32;
        if (ud < 0.f) t31 = -t31;
      }

      t32 = glm::normalize(t32);
      t21 = glm::normalize(t21);
      t31 = glm::normalize(t31);
      tpDebug() << "t32= (" << t32.x << " " << t32.y << " " << t32.z << ")";
      tpDebug() << "t21= (" << t21.x << " " << t21.y << " " << t21.z << ")";
      tpDebug() << "t31= (" << t31.x << " " << t31.y << " " << t31.z << ")";
#endif
    }

    if(glm::length2(t) > 1.e-10f)
    {
      t = glm::normalize(t);
      tangent[idx1] += t;
      tangent[idx2] += t;
      tangent[idx3] += t;
#if 0
      if(glm::abs(glm::dot(verts.at(idx1).normal, t)) > 0.99f)
         tpDebug() << "Here1";
      if(glm::abs(glm::dot(verts.at(idx2).normal, t)) > 0.99f)
         tpDebug() << "Here2";
      if(glm::abs(glm::dot(verts.at(idx3).normal, t)) > 0.99f)
         tpDebug() << "Here3";
#endif
    }
#if 0
    else
    {
      int y=1;
    }
#endif
  }
}

//################################################################################################
void accumulateTangents(const Geometry3D& geometry,
                        Indexes3D const& part,
                        std::vector<glm::vec3>& tangent)
{
  if(part.type == geometry.triangleFan)
    for(size_t n=2; n<part.indexes.size(); ++n)
      accumulateTangentForTriangle(geometry.verts, part, 0, n-1, n, tangent);
  else if(part.type == geometry.triangleStrip)
    for(size_t n=2; n<part.indexes.size(); ++n)
      accumulateTangentForTriangle(geometry.verts, part, n-2, n-1, n, tangent);
  else if(part.type == geometry.triangles)
    // process each triplet of consecutive vertices
    for(size_t n=0; n<part.indexes.size(); n+=3)
      accumulateTangentForTriangle(geometry.verts, part, n, n+1, n+2, tangent);
}

}
    
//##################################################################################################
std::vector<std::string> normalCalculationModes()
{
  return
  {
    "None",
    "CalculateFaceNormals",
    "CalculateVertexNormals",
    "CalculateAdaptiveNormals"
  };
}

//##################################################################################################
std::string normalCalculationModeToString(NormalCalculationMode mode)
{
  switch(mode)
  {
    case NormalCalculationMode::None:                           return "None";
    case NormalCalculationMode::CalculateFaceNormals:           return "CalculateFaceNormals";
    case NormalCalculationMode::CalculateVertexNormals:         return "CalculateVertexNormals";
    case NormalCalculationMode::CalculateAdaptiveNormals:       return "CalculateAdaptiveNormals";
  }
  return "None";
}

//##################################################################################################
NormalCalculationMode normalCalculationModeFromString(const std::string& mode)
{
  if(mode == "None")                           return NormalCalculationMode::None;
  if(mode == "CalculateFaceNormals")           return NormalCalculationMode::CalculateFaceNormals;
  if(mode == "CalculateVertexNormals")         return NormalCalculationMode::CalculateVertexNormals;
  if(mode == "CalculateAdaptiveNormals")       return NormalCalculationMode::CalculateAdaptiveNormals;
  return NormalCalculationMode::None;
}

//##################################################################################################
Vertex3D Vertex3D::interpolate(float u, const Vertex3D& v0, const Vertex3D& v1)
{
  float v = 1.0f - u;
  tp_math_utils::Vertex3D result;

  result.vert    = (v*v0.vert   ) + (u*v1.vert   );
  result.texture = (v*v0.texture) + (u*v1.texture);
  result.normal  = (v*v0.normal ) + (u*v1.normal );

  return result;
}

//##################################################################################################
Vertex3D Vertex3D::interpolate(float u, float v, const Vertex3D& v0, const Vertex3D& v1, const Vertex3D& v2)
{
  float w = 1.0f - (u+v);
  tp_math_utils::Vertex3D result;

  result.vert    = (w*v0.vert   ) + (u*v1.vert   ) + (v*v2.vert   );
  result.texture = (w*v0.texture) + (u*v1.texture) + (v*v2.texture);
  result.normal  = (w*v0.normal ) + (u*v1.normal ) + (v*v2.normal );

  return result;
}

//##################################################################################################
void Geometry3D::add(const Geometry3D& other)
{
  auto offset = verts.size();
  verts.reserve(offset+other.verts.size());
  for(const auto& vert : other.verts)
    verts.push_back(vert);

  indexes.reserve(indexes.size()+other.indexes.size());
  for(const auto& index : other.indexes)
    for(auto& i : indexes.emplace_back(index).indexes)
      i+=int(offset);
}

//##################################################################################################
void Geometry3D::clear()
{
  comments.clear();
  verts.clear();
  indexes.clear();
}

//##################################################################################################
std::string Geometry3D::stats(const std::vector<Geometry3D>& geometry)
{
  size_t vertCount{0};
  size_t indexCount{0};
  size_t triangleCount{0};

  for(const auto& mesh : geometry)
    mesh.stats(vertCount, indexCount, triangleCount);

  return statsString(vertCount, indexCount, triangleCount);
}

//##################################################################################################
std::string Geometry3D::statsString(size_t vertCount, size_t indexCount, size_t triangleCount)
{
  return
      std::string("Verts: ") + std::to_string(vertCount) +
      std::string(" indexes: ") + std::to_string(indexCount) +
      std::string(" triangles: ") + std::to_string(triangleCount);
}

//##################################################################################################
void Geometry3D::stats(size_t& vertCount, size_t& indexCount, size_t& triangleCount) const
{
  vertCount += verts.size();

  for(const auto& index : indexes)
  {
    indexCount += index.indexes.size();

    if(index.type == triangleFan)
      triangleCount+=index.indexes.size()-2;
    else if(index.type == triangleStrip)
      triangleCount+=index.indexes.size()-2;
    else if(index.type == triangles)
      triangleCount+=index.indexes.size()/3;
  }
}

//##################################################################################################
std::string Geometry3D::stats() const
{
  size_t vertCount{0};
  size_t indexCount{0};
  size_t triangleCount{0};

  stats(vertCount, indexCount, triangleCount);

  return statsString(vertCount, indexCount, triangleCount);
}

//##################################################################################################
void Geometry3D::convertToTriangles()
{
  std::vector<Face_lt> faces = calculateFaces(*this, false);

  indexes.clear();
  Indexes3D& newIndexes = indexes.emplace_back();
  newIndexes.type = triangles;
  newIndexes.indexes.reserve(faces.size()*3);

  for(const auto& face : faces)
    for(const auto& i : face.indexes)
      newIndexes.indexes.push_back(i);
}

//##################################################################################################
void Geometry3D::breakApartTriangles()
{
  std::vector<Face_lt> faces = calculateFaces(*this, false);

  indexes.clear();
  Indexes3D& newIndexes = indexes.emplace_back();
  newIndexes.type = triangles;
  newIndexes.indexes.resize(faces.size()*3);
  for(size_t i=0; i<newIndexes.indexes.size(); i++)
    newIndexes.indexes[i] = int(i);

  std::vector<Vertex3D> newVerts;
  newVerts.reserve(faces.size()*3);
  for(const auto& face : faces)
  {
    for(const auto& i : face.indexes)
    {
      auto& v = newVerts.emplace_back();
      v = verts[size_t(i)];
    }
  }

  verts.swap(newVerts);
}

//##################################################################################################
void Geometry3D::calculateNormals(NormalCalculationMode mode, float minDot)
{
  switch(mode)
  {
    case NormalCalculationMode::None:
    break;

    case NormalCalculationMode::CalculateFaceNormals:
    calculateFaceNormals();
    break;

    case NormalCalculationMode::CalculateVertexNormals:
    calculateVertexNormals();
    break;

    case NormalCalculationMode::CalculateAdaptiveNormals:
    calculateAdaptiveNormals(minDot);
    break;
  }
}

//##################################################################################################
void Geometry3D::calculateVertexNormals()
{
  const size_t vMax = verts.size();

  std::vector<int> normalCounts(vMax, 0);

  for(auto& vert : verts)
    vert.normal = {0.0f, 0.0f, 0.0f};

  std::vector<Face_lt> faces = calculateFaces(*this, true);
  {
    auto const* face = faces.data();
    auto faceMax = face + faces.size();
    for(; face<faceMax; face++)
    {
      if(std::isnan(face->normal.x) || std::isnan(face->normal.y) || std::isnan(face->normal.z) ||
         std::isinf(face->normal.x) || std::isinf(face->normal.y) || std::isinf(face->normal.z))
        continue;

      for(const auto& i : face->indexes)
      {
        auto ii = size_t(i);
        normalCounts[ii]++;
        verts[ii].normal += face->normal;
      }
    }
  }

  {
    auto vert  = verts.data();
    auto vertMax = vert+vMax;
    auto count = normalCounts.data();
    for(; vert<vertMax; vert++, count++)
    {
      if(*count && glm::length2(vert->normal)>0.000001f)
        vert->normal = glm::normalize(vert->normal);
      else
        vert->normal = {0.0f, 0.0f, 1.0f};
    }
  }
}

//##################################################################################################
void Geometry3D::calculateFaceNormals()
{
  std::vector<Face_lt> faces = calculateFaces(*this, true);

  indexes.clear();
  Indexes3D& newIndexes = indexes.emplace_back();
  newIndexes.type = triangles;
  newIndexes.indexes.resize(faces.size()*3);
  for(size_t i=0; i<newIndexes.indexes.size(); i++)
    newIndexes.indexes[i] = int(i);

  std::vector<Vertex3D> newVerts;
  newVerts.reserve(faces.size()*3);
  for(const auto& face : faces)
  {
    for(const auto& i : face.indexes)
    {
      auto& v = newVerts.emplace_back();
      v = verts[size_t(i)];
      v.normal = face.normal;
    }
  }

  verts = std::move(newVerts);
}

namespace
{
struct VertCloud
{
  std::vector<Vertex3D>  pts;
  inline size_t kdtree_get_point_count() const { return pts.size(); }
  inline float kdtree_get_pt(const size_t idx, const size_t dim) const
  {
    switch(dim)
    {
      case 0: return pts[idx].vert.x;
      case 1: return pts[idx].vert.y;
      case 2: return pts[idx].vert.z;
      case 3: return pts[idx].texture.x;
    }

    return pts[idx].texture.y;
  }

  template <class BBOX>
  bool kdtree_get_bbox(BBOX&) const { return false; }
};
}

//##################################################################################################
void Geometry3D::combineSimilarVerts()
{
  size_t insertPos=0;
  std::vector<size_t> idxLookup;
  idxLookup.resize(verts.size());

  typedef nanoflann::KDTreeSingleIndexDynamicAdaptor<nanoflann::L2_Simple_Adaptor<float, VertCloud>, VertCloud, 5> KDTree;

  VertCloud cloud;
  cloud.pts.reserve(verts.size());
  KDTree tree(5, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );

  for(size_t i=0; i<verts.size(); i++)
  {
    const auto& vert = verts.at(i);

    bool found=false;

    {
      float query[5] = {vert.vert.x, vert.vert.y, vert.vert.z, vert.texture.x, vert.texture.y};

      size_t index;
      float dist2;
      nanoflann::KNNResultSet<float> resultSet(1);
      resultSet.init(&index, &dist2);
      if(tree.findNeighbors(resultSet, query, nanoflann::SearchParams(10)))
      {
        if(dist2 < 0.0000001f)
        {
          found = true;
          idxLookup[i] = index;
        }
      }
    }

    if(!found)
    {
      cloud.pts.push_back(vert);
      tree.addPoints(insertPos, insertPos);
      idxLookup[i] = insertPos;
      insertPos++;
    }
  }

  verts.swap(cloud.pts);

  for(auto& ii : indexes)
  {
    for(auto& i : ii.indexes)
    {
      auto j = size_t(i);
      i = int(idxLookup[j]);
    }
  }
}

//##################################################################################################
void Geometry3D::calculateAdaptiveNormals(float minDot)
{
  combineSimilarVerts();

  const size_t vMax = verts.size();

  for(auto& vert : verts)
    vert.normal = {0.0f, 0.0f, 0.0f};

  struct VertCluster_lt
  {
    glm::vec3 normal{0,0,0};
    int newVertIndex{0};
  };

  struct VertDetails_lt
  {
    std::vector<VertCluster_lt> clusters;
  };

  std::vector<VertDetails_lt> clusters(vMax);

  std::vector<Face_lt> faces = calculateFaces(*this, true);
  std::vector<std::array<int, 3>> faceClusters(faces.size());

  size_t newVertsCount=0;
  for(size_t f=0; f<faces.size(); f++)
  {
    const auto& face = faces.at(f);

    for(size_t i=0; i<3; i++)
    {
      auto& vertClusters = clusters.at(face.indexes.at(i));

      bool done=false;
      for(size_t c=0; c<vertClusters.clusters.size(); c++)
      {
        auto& cluster = vertClusters.clusters.at(c);
        if(glm::dot(glm::normalize(cluster.normal), face.normal)>minDot)
        {
          faceClusters.at(f).at(i) = int(c);
          cluster.normal += face.normal;
          done=true;
          break;
        }
      }

      if(!done)
      {
        newVertsCount++;
        faceClusters.at(f).at(i) = int(vertClusters.clusters.size());
        auto& cluster = vertClusters.clusters.emplace_back();
        cluster.normal = face.normal;
      }
    }
  }

  {
    std::vector<Vertex3D> newVerts;
    newVerts.reserve(newVertsCount);
    for(size_t c=0; c<clusters.size(); c++)
    {
      auto& vertClusters = clusters.at(c);
      for(auto& cluster : vertClusters.clusters)
      {
        cluster.newVertIndex = int(newVerts.size());
        auto& newVert = newVerts.emplace_back();
        newVert = verts.at(c);
        newVert.normal = glm::normalize(cluster.normal);
      }
    }

    verts = std::move(newVerts);
  }

  indexes.clear();
  Indexes3D& newIndexes = indexes.emplace_back();
  newIndexes.type = triangles;
  newIndexes.indexes.reserve(faces.size()*3);

  for(size_t f=0; f<faceClusters.size(); f++)
  {
    const auto& face = faces.at(f);
    const auto& faceCluster = faceClusters.at(f);

    for(size_t i=0; i<3; i++)
    {
      const auto& vertClusters = clusters.at(face.indexes.at(i));
      newIndexes.indexes.push_back(vertClusters.clusters.at(faceCluster.at(i)).newVertIndex);
    }
  }
}

//##################################################################################################
void Geometry3D::transform(const glm::mat4& m)
{
  glm::mat3 r(m);
  for(auto& vert : verts)
  {
    vert.vert = tpProj(m, vert.vert);
    vert.normal = r * vert.normal;
  }
}

//##################################################################################################
void Geometry3D::addBackFaces()
{
  std::vector<Face_lt> faces = calculateFaces(*this, false);

  size_t size = verts.size();
  verts.resize(size*2);
  for(size_t i=0; i<size; i++)
  {
    auto& dst = verts.at(i+size);
    dst = verts.at(i);
    dst.normal = -dst.normal;
  }

  auto& newTriangles = indexes.emplace_back();
  newTriangles.type = triangles;
  newTriangles.indexes.reserve(faces.size()*3);
  for(const auto& face : faces)
  {
    newTriangles.indexes.push_back(face.indexes[2] + int(size));
    newTriangles.indexes.push_back(face.indexes[1] + int(size));
    newTriangles.indexes.push_back(face.indexes[0] + int(size));
  }
}

//################################################################################################
void Geometry3D::buildTangentVectors(std::vector<glm::vec3>& tangent) const
{
  // start with non-zero in case no valid tangents are found - there will still be a valid result
  tangent.assign(verts.size(), {1.e-6f,0,0});
  for(const auto& part : indexes)
    accumulateTangents(*this, part, tangent);
    
  // normalize each tangent vector to unit length
  for(auto& t : tangent)
    t = glm::normalize(t);
#if 0
  // when tangent and normal are nearly parallel we have to select a different tangent
  for(size_t idx=0; idx<tangent.size(); ++idx)
    if(glm::abs(glm::dot(verts.at(idx).normal, tangent.at(idx))) > 0.999f)
    {
      glm::vec3 t1 = glm::cross(glm::vec3(1,0,0), verts.at(idx).normal);
      glm::vec3 t2 = glm::cross(glm::vec3(0,1,0), verts.at(idx).normal);
      tangent[idx] = glm::normalize((glm::dot(t1, t1)>glm::dot(t2,t2))?t1:t2);
    }
#endif
}

//##################################################################################################
tp_utils::StringID Geometry3D::getName() const
{
  if(!comments.empty() && comments[0] == "MESH_NAME")
    return comments[1];
  else
    return material.name;
}

//##################################################################################################
size_t Geometry3D::sizeInBytes(const std::vector<Geometry3D>& geometry)
{
  size_t size{0};
  for(const auto& mesh : geometry)
  {
    for(const auto& indexes : mesh.indexes)
      size += indexes.indexes.size() * sizeof(int);

    size += mesh.verts.size() * sizeof(Vertex3D);

    for(const auto& comment : mesh.comments)
      size += comment.size();
  }
  return size;
}

//################################################################################################
bool Geometry3D::printDataToFile(const std::vector<Geometry3D>& geometry, std::string const& filename)
{
  try
  {
    std::stringstream ss;

    for(auto const& mesh : geometry)
    {
      for(auto const& c: mesh.comments)
        ss << "Comment: " << c << "\n";

      ss << "Geometry\n";
      for(const auto& indexes : mesh.indexes)
      {
        ss << "Index:";
        for(const auto i : indexes.indexes)
          ss << " " << i;

        ss << "\n";
      }

      ss << "Vertex count " << mesh.verts.size() << "\n";
      for(const auto& v : mesh.verts)
      {
        ss << "  Vertex:\n";
        ss << "    vert: " << v.vert[0] << " " << v.vert[1] << " " << v.vert[2] << "\n";
        ss << "    texture: " << v.texture[0] << " " << v.texture[1] << "\n";
        ss << "    normal: " << v.normal[0] << " " << v.normal[1] << " " << v.normal[2] << "\n";
      }

      ss << "Material\n";

      {
        nlohmann::json j;
        mesh.material.saveState(j);
        ss << j.dump(2);
      }
    }

    return tp_utils::writeTextFile(filename, ss.str());
  }
  catch(...)
  {
    return false;
  }
}

//##################################################################################################
nlohmann::json Geometry::saveState() const
{
  nlohmann::json j;
  j["geometry"] = vec2VectorToJSON(geometry);
  j["transform"] = mat4ToJSON(transform);
  material.saveState(j["material"]);
  return j;
}

//##################################################################################################
void Geometry::loadState(const nlohmann::json& j)
{
  geometry = vec2VectorFromJSON(TPJSON(j, "geometry"));
  transform = mat4FromJSON(TPJSON(j, "transform"));
  material.loadState(TPJSON(j, "material"));
}

}
