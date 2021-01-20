#include "tp_math_utils/Geometry3D.h"

#include "tp_utils/DebugUtils.h"

#include "nanoflann.hpp"

#include "glm/gtx/norm.hpp"
#include "glm/gtx/normal.hpp"

#include <array>

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
      face.normal = glm::triangleNormal(
            geometry.verts.at(size_t(face.indexes[0])).vert,
          geometry.verts.at(size_t(face.indexes[1])).vert,
          geometry.verts.at(size_t(face.indexes[2])).vert);

  return faces;
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
    "CalculateAdaptiveNormals",
    "CalculateTangentsAndBitangents"
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
  case NormalCalculationMode::CalculateTangentsAndBitangents: return "CalculateTangentsAndBitangents";
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
  if(mode == "CalculateTangentsAndBitangents") return NormalCalculationMode::CalculateTangentsAndBitangents;
  return NormalCalculationMode::None;
}

//##################################################################################################
void Vertex3D::calculateSimpleTangentAndBitangent()
{
  float f=1.0;
  for(const auto& v : {glm::vec3(1.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f),glm::vec3(0.0f,0.0f,1.0f)})
  {
    if(auto a=std::fabs(glm::dot(normal, v)); a<f)
    {
      f=a;
      tangent=v;
    }
  }

  makeTangentAndBitangentOrthogonal();
}

//##################################################################################################
void Vertex3D::makeTangentAndBitangentOrthogonal()
{
  bitangent = glm::cross(normal, tangent);
  tangent   = glm::cross(bitangent, normal);
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
std::string Geometry3D::stats() const
{
  size_t indexCount{0};
  size_t triangleCount{0};
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

  return
      std::string("Verts: ") + std::to_string(verts.size()) +
      std::string(" indexes: ") + std::to_string(indexCount) +
      std::string(" triangles: ") + std::to_string(triangleCount);
}

//##################################################################################################
void Geometry3D::convertToTriangles()
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

  verts = std::move(newVerts);
}

//##################################################################################################
void Geometry3D::calculateNormals(NormalCalculationMode mode)
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
    calculateAdaptiveNormals();
    break;

  case NormalCalculationMode::CalculateTangentsAndBitangents:
    calculateTangentsAndBitangents();
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
      if(*count)
      {
        //vert->normal/=float(*count);
        vert->normal = glm::normalize(vert->normal);
      }
      else
        vert->normal = {0.0f, 0.0f, 1.0f};

      vert->calculateSimpleTangentAndBitangent();
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
      v.calculateSimpleTangentAndBitangent();
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
    case 3: return pts[idx].color.x;
    case 4: return pts[idx].color.y;
    case 5: return pts[idx].color.z;
    case 6: return pts[idx].texture.x;
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

  typedef nanoflann::KDTreeSingleIndexDynamicAdaptor<nanoflann::L2_Simple_Adaptor<float, VertCloud>, VertCloud, 8> KDTree;

  VertCloud cloud;
  cloud.pts.reserve(verts.size());
  KDTree tree(8, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );

  for(size_t i=0; i<verts.size(); i++)
  {
    const auto& vert = verts.at(i);

    bool found=false;

    {
      float query[8] = {vert.vert.x, vert.vert.y, vert.vert.z, vert.color.x, vert.color.y, vert.color.z, vert.texture.x, vert.texture.y};

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
    for(auto& i : ii.indexes)
      i = int(idxLookup[size_t(i)]);
}

//##################################################################################################
void Geometry3D::calculateAdaptiveNormals()
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

  const float minDot=0.9f;

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
        newVert.calculateSimpleTangentAndBitangent();
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

  //calculateTangentsAndBitangents();
}

//##################################################################################################
void Geometry3D::calculateTangentsAndBitangents()
{
  for(auto& vert : verts)
  {
    vert.tangent = {0.0f,0.0f,0.0f};
    vert.bitangent = {0.0f,0.0f,0.0f};
  }

  for(const auto& face : calculateFaces(*this, false))
  {
    size_t i0 = face.indexes[0];
    size_t i1 = face.indexes[1];
    size_t i2 = face.indexes[2];

    auto& vert0 = verts.at(i0);
    auto& vert1 = verts.at(i1);
    auto& vert2 = verts.at(i2);

    const glm::vec3& v0 = vert0.vert;
    const glm::vec3& v1 = vert1.vert;
    const glm::vec3& v2 = vert2.vert;

    const glm::vec2& uv0 = vert0.texture;
    const glm::vec2& uv1 = vert1.texture;
    const glm::vec2& uv2 = vert2.texture;

    glm::vec3 deltaPos1 = v1-v0;
    glm::vec3 deltaPos2 = v2-v0;

    glm::vec2 deltaUV1 = uv1-uv0;
    glm::vec2 deltaUV2 = uv2-uv0;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

    if(glm::length2(deltaUV1)<0.0000001f ||
       glm::length2(deltaUV2)<0.0000001f ||
       glm::isinf(r) ||
       glm::isnan(r))
    {
      deltaUV1 = {1,0};
      deltaUV2 = {0,1};
      r = 1.0f;
    }

    glm::vec3 tangent   = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

    if(glm::length2(tangent)<0.0000001f || glm::length2(bitangent)<0.0000001f)
    {

    }
    else
    {
      vert0.tangent += tangent;
      vert1.tangent += tangent;
      vert2.tangent += tangent;

      vert0.bitangent += bitangent;
      vert1.bitangent += bitangent;
      vert2.bitangent += bitangent;
    }
  }

  for(auto& vert : verts)
  {
    if(glm::length2(vert.tangent)<0.0000001f || glm::length2(vert.bitangent)<0.0000001f)
    {
      // If the tangents are to small calculate them from the normal.
      vert.calculateSimpleTangentAndBitangent();
    }
    else
    {
      // Otherwise normalize them and make sure that they are orthogonal.
      vert.tangent = glm::normalize(vert.tangent);
      vert.bitangent = glm::normalize(vert.bitangent);
      vert.makeTangentAndBitangentOrthogonal();
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
    vert.tangent = r * vert.tangent;
    vert.bitangent = r * vert.bitangent;
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

//##################################################################################################
tp_utils::StringID Geometry3D::getName() const
{
  return (!comments.empty())?tp_utils::StringID(comments.front()):material.name;
}

}
