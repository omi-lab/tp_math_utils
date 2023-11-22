#include "tp_math_utils/Geometry3D.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/FileUtils.h"

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
