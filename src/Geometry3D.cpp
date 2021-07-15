#include "tp_math_utils/Geometry3D.h"
#include "tp_math_utils/Intersection.h"
#include "tp_math_utils/Plane.h"
#include "tp_math_utils/Ray.h"
#include "tp_math_utils/JSONUtils.h"

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
float getLongestEdge(const std::vector<glm::vec3>& points, size_t& longestEdgeIndex)
{
  longestEdgeIndex = 0;
  float longestEdgeLength2 = 0;
  for (size_t pointIdx = 0; pointIdx < points.size(); ++pointIdx)
  {
    float currentEdgeLength2 = glm::distance2(points[pointIdx], points[(pointIdx + 1) % 3]);
    if (currentEdgeLength2 > longestEdgeLength2)
    {
       longestEdgeLength2 = currentEdgeLength2;
       longestEdgeIndex = pointIdx;
    }
  }
  return sqrt(longestEdgeLength2);
}

//##################################################################################################
void subdivideFaceToTarget(const std::array<int, 3>& faceIndexes,
                        std::vector<Vertex3D>& meshVertices,
                        std::vector<int>& meshIndexes,
                        const glm::mat4& modelMatrix,
                        float targetEdgeLength,
                        unsigned int maxSubdivisions)
{
  std::function<void(const std::array<int, 3>&, unsigned int)> subdivideFaceInTwo =
      [&](const std::array<int, 3>& faceIndexes, unsigned int subdivisionCount)
  {
    size_t longestEdgeIdx = 0;
    // Compute the longest edge of the triangle in model space.
    float longestEdgeLength = getLongestEdge({
                                               glm::vec3(modelMatrix * glm::vec4(meshVertices[faceIndexes[0]].vert, 1.0f)),
                                               glm::vec3(modelMatrix * glm::vec4(meshVertices[faceIndexes[1]].vert, 1.0f)),
                                               glm::vec3(modelMatrix * glm::vec4(meshVertices[faceIndexes[2]].vert, 1.0f))},
                                             longestEdgeIdx);

    if (longestEdgeLength <= targetEdgeLength || subdivisionCount >= maxSubdivisions)
    {
      // Stop subdividing, add the indexes of the provided triangle.
      for (size_t i = 0; i < faceIndexes.size(); ++i)
        meshIndexes.emplace_back(faceIndexes[i]);
      return;
    }

    // Subdivide in 2 triangles by creating a vertex at the middle of the longest edge.
    int newVertexIndex = static_cast<int>(meshVertices.size());
    Vertex3D newVertex = Vertex3D::interpolate(0.5f, meshVertices[faceIndexes[longestEdgeIdx]], meshVertices[faceIndexes[(longestEdgeIdx + 1) % 3]]);
    meshVertices.emplace_back(std::move(newVertex));

    // Subdivide the 2 resulting triangles.
    {
      ++subdivisionCount;
      int index0 = faceIndexes[longestEdgeIdx];
      int index1 = newVertexIndex;
      int index2 = faceIndexes[(longestEdgeIdx + 2) % 3];
      subdivideFaceInTwo({index0, index1, index2}, subdivisionCount);
      index0 = newVertexIndex;
      index1 = faceIndexes[(longestEdgeIdx + 1) % 3];
      index2 = faceIndexes[(longestEdgeIdx + 2) % 3];
      subdivideFaceInTwo({index0, index1, index2}, subdivisionCount);
    }
  };

  // Recursively divide the face in 2.
  subdivideFaceInTwo(faceIndexes, 0);
}

//##################################################################################################
/*! If maxSubdivisions and targetEdgeLength are not 0,
 *  faces inside frustum will be further subdivide to reach target size.
*/
void divideFaceWithFrustum(const std::array<int, 3>& faceIndexes,
                           std::vector<Vertex3D>& meshVertices,
                           std::vector<int>& meshIndexes,
                           const glm::mat4& modelMatrix,
                           const std::vector<tp_math_utils::Plane>& frustumPlanes,
                           const std::function<bool(const glm::vec3&, glm::vec4*)>& isPointInCamera,
                           float targetEdgeLength = 0,
                           unsigned int maxSubdivisions = 0)
{
  bool subdivideInsideFrustum = (maxSubdivisions != 0 && targetEdgeLength > 0);

  std::function<void(const std::array<int, 3>&, size_t)> divideFaceWithPlane =
      [&](const std::array<int, 3>& faceIndexes, size_t planeIdx)
  {
    // Transform the face vertices from model space to world space.
    std::array<glm::vec4, 3> faceVerticesInWorldSpace;
    for (size_t vertexIdx = 0; vertexIdx < faceIndexes.size(); ++vertexIdx)
      faceVerticesInWorldSpace[vertexIdx] = modelMatrix * glm::vec4(meshVertices[faceIndexes[vertexIdx]].vert, 1.0f);

    {
      bool stopDividing = (planeIdx >= frustumPlanes.size()) ? true : false;
      bool faceIsInCamera = false;
      if (!stopDividing || subdivideInsideFrustum)
      {
        // Test if face is in the camera frustum
      if (isPointInCamera(faceVerticesInWorldSpace[0], nullptr)
          && isPointInCamera(faceVerticesInWorldSpace[1], nullptr)
          && isPointInCamera(faceVerticesInWorldSpace[2], nullptr))
      {
        faceIsInCamera = true;
        // Opti: no need to test intersection with remaining planes.
        stopDividing = true;
      }
      }

      if (stopDividing)
      {
        if (subdivideInsideFrustum && faceIsInCamera)
        {
          subdivideFaceToTarget(faceIndexes, meshVertices, meshIndexes, modelMatrix, targetEdgeLength, maxSubdivisions);
        }
        else
        {
          // We divided along all planes: add the indexes of the provided triangle.
          for (size_t i = 0; i < faceIndexes.size(); ++i)
            meshIndexes.emplace_back(faceIndexes[i]);
        }
        return;
      }
    }

    const auto& frustumPlane = frustumPlanes[planeIdx];
    ++planeIdx;

    // Test intersection between plane and each edge of the triangle.
    Vertex3D intersectionVertices[2]; // in model space
    glm::vec3 intersectionPoints[2]; // in world space
    size_t nonIntersectedEdgeIdx = 2;
    int intersectionCount = 0;
    for (size_t edgeIdx = 0; edgeIdx < 3; ++edgeIdx)
    {
      // Edge vertices in world space.
      const glm::vec3& edgeVertex0 = glm::vec3(faceVerticesInWorldSpace[edgeIdx]);
      const glm::vec3& edgeVertex1 = glm::vec3(faceVerticesInWorldSpace[(edgeIdx + 1) % 3]);
      // Find intersection in world space with the edge line (this returns false if edge is parallel to the plane or on the plane).
      const tp_math_utils::Ray triangleEdge(edgeVertex0, edgeVertex1);
      if (tp_math_utils::rayPlaneIntersection(triangleEdge, frustumPlane, intersectionPoints[intersectionCount]))
      {
        float edgeLength = glm::distance(edgeVertex0, edgeVertex1);
        float distanceToEdgeVertex0 = glm::distance(intersectionPoints[intersectionCount], edgeVertex0);
        // If the intersection is already a triangle vertex or outside the edge, no need to subdivide.
        if (distanceToEdgeVertex0 >= edgeLength
            || glm::distance(intersectionPoints[intersectionCount], edgeVertex1) >= edgeLength)
        {
          nonIntersectedEdgeIdx = edgeIdx;
          continue;
        }

        float t = distanceToEdgeVertex0 / edgeLength;
        // Create a new vertex at the intersection point (in model space).
        intersectionVertices[intersectionCount] = Vertex3D::interpolate(t, meshVertices[faceIndexes[edgeIdx]], meshVertices[faceIndexes[(edgeIdx + 1) % 3]]);
        ++intersectionCount;
      }
      else
        nonIntersectedEdgeIdx = edgeIdx;

      // There can either be 2 intersection points or 0 (if triangle is entirely on one side or the other of the plane).
      if (intersectionCount > 1 || ((edgeIdx > 0) && (intersectionCount == 0)))
        break;
    }

    // 1st possibility: no intersection.
    if (intersectionCount == 0)
    {
      // Recurse on the same triangle with the next plane.
      divideFaceWithPlane(faceIndexes, planeIdx);
      return;
    }

    // 2nd possibility: intersection edge is entirely outside the frustum.
    {
      glm::vec4 offsetPoint0;
      glm::vec4 offsetPoint1;
      bool point0InCamera = isPointInCamera(intersectionPoints[0], &offsetPoint0);
      bool point1InCamera = isPointInCamera(intersectionPoints[1], &offsetPoint1);
      // Detect case where both intersections are outside and on the same side of the frustum.
      if (!point0InCamera && !point1InCamera)
      {
        glm::vec4 mult = offsetPoint0 * offsetPoint1;
        if (mult.x >= 0 && mult.y >= 0 && mult.z >= 0 && mult.w >= 0)
        {
          // No need to subdivide, recurse on the same triangle with the next plane.
          divideFaceWithPlane(faceIndexes, planeIdx);
          return;
        }
      }

      // Detect case where one intersection is on the frustum edge and the rest is outside.
      size_t pointInFrustumIdx = 2;
      if (point0InCamera)
      {
        if (!point1InCamera) pointInFrustumIdx = 0;
      }
      else
      {
        if (point1InCamera) pointInFrustumIdx = 1;
      }
      if (pointInFrustumIdx == 0 || pointInFrustumIdx == 1)
      {
        glm::vec3 offsetPoint = intersectionPoints[pointInFrustumIdx] +
            0.01f * (intersectionPoints[(pointInFrustumIdx + 1) % 2] - intersectionPoints[pointInFrustumIdx]);
        if (!isPointInCamera(offsetPoint, nullptr))
        {
          // No need to subdivide, recurse on the same triangle with the next plane.
          divideFaceWithPlane(faceIndexes, planeIdx);
          return;
        }
      }
    }

    // 3rd possibility: intersection edge is inside the frustum (partially at least): subdivide in 3 triangles.
    {
      // Add the new vertices.
      int firstIntersectionIndex = static_cast<int>(meshVertices.size());
      meshVertices.emplace_back(std::move(intersectionVertices[0]));
      meshVertices.emplace_back(std::move(intersectionVertices[1]));

      // Define the indexes for the 3 triangles resulting of the subdivision.
      // 1st triangle: using the non-intersected edge + 1st intersection point
      int index0 = faceIndexes[nonIntersectedEdgeIdx];
      int index1 = faceIndexes[(nonIntersectedEdgeIdx + 1) % 3];
      int index2 = firstIntersectionIndex;
      // Recurse on this new triangle with the next plane.
      divideFaceWithPlane({index0, index1, index2}, planeIdx);

      // 2nd triangle: using 1st intersection point, 2nd intersection point, 1st vertex of non-intersected edge (or opposite vertex to non-intersected edge).
      index0 = firstIntersectionIndex;
      index1 = firstIntersectionIndex + 1;
      index2 = (nonIntersectedEdgeIdx != 1) ? faceIndexes[nonIntersectedEdgeIdx] : faceIndexes[(nonIntersectedEdgeIdx + 2) % 3];
      // Recurse on this new triangle with the next plane.
      divideFaceWithPlane({index0, index1, index2}, planeIdx);

      // 3rd triangle: using 2nd intersection point, 1st intersection point, opposite vertex to non-intersected edge (or 2nd vertex of non-intersected edge).
      index0 = firstIntersectionIndex + 1;
      index1 = firstIntersectionIndex;
      index2 = (nonIntersectedEdgeIdx != 1) ? faceIndexes[(nonIntersectedEdgeIdx + 2) % 3] : faceIndexes[(nonIntersectedEdgeIdx + 1) % 3];
      // Recurse on this new triangle with the next plane.
      divideFaceWithPlane({index0, index1, index2}, planeIdx);
    }
  };

  // Divide along the first plane (then recursively along other planes).
  divideFaceWithPlane(faceIndexes, 0);
}

//##################################################################################################
Vertex3D Vertex3D::interpolate(float u, const Vertex3D& v0, const Vertex3D& v1)
{
  float v = 1.0f - u;
  tp_math_utils::Vertex3D result;

  result.vert    = (v*v0.vert   ) + (u*v1.vert   );
  result.color   = (v*v0.color  ) + (u*v1.color  );
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
  result.color   = (w*v0.color  ) + (u*v1.color  ) + (v*v2.color  );
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
      if(*count)
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
void Geometry3D::subdivideToTargetSize(const glm::mat4& modelMatrix,
                                       float targetEdgeLength,
                                       unsigned int maxSubdivisions)
{
  // Calculate the faces of the mesh.
  std::vector<Face_lt> faces = calculateFaces(*this, false);

  // Initialise.
  indexes.clear();
  Indexes3D& newIndexes = indexes.emplace_back();
  newIndexes.type = triangles;
  newIndexes.indexes.reserve(faces.size() * 3);

  // For each triangle of the mesh,
  for (const auto& face : faces)
    subdivideFaceToTarget(face.indexes, verts, newIndexes.indexes, modelMatrix, targetEdgeLength, maxSubdivisions);
}

//##################################################################################################
void Geometry3D::divideAroundFrustum(const glm::mat4& modelMatrix,
                                        const std::vector<tp_math_utils::Plane>& frustumPlanes,
                                        const std::function<bool(const glm::vec3&, glm::vec4*)>& isPointInCamera)
{
  // Calculate the faces of the mesh.
  std::vector<Face_lt> faces = calculateFaces(*this, false);

  // Initialise.
  indexes.clear();
  Indexes3D& newIndexes = indexes.emplace_back();
  newIndexes.type = triangles;
  newIndexes.indexes.reserve(faces.size() * 3);

  // For each triangle of the mesh,
  for (const auto& face : faces)
    divideFaceWithFrustum(face.indexes, verts, newIndexes.indexes, modelMatrix,
                          frustumPlanes, isPointInCamera, 0, 0);
}

//##################################################################################################
void Geometry3D::subdivideToTargetSizeInsideFrustum(const glm::mat4& modelMatrix,
                                                    const std::vector<tp_math_utils::Plane>& frustumPlanes,
                                                    const std::function<bool(const glm::vec3&, glm::vec4*)>& isPointInCamera,
                                                    float targetEdgeLength,
                                                    unsigned int maxSubdivisions)
{
  // Calculate the faces of the mesh.
  std::vector<Face_lt> faces = calculateFaces(*this, false);

  // Initialise.
  indexes.clear();
  Indexes3D& newIndexes = indexes.emplace_back();
  newIndexes.type = triangles;
  newIndexes.indexes.reserve(faces.size() * 3);

  // For each triangle of the mesh,
  for (const auto& face : faces)divideFaceWithFrustum(face.indexes, verts, newIndexes.indexes, modelMatrix,
                                                      frustumPlanes, isPointInCamera, targetEdgeLength, maxSubdivisions);
}

//##################################################################################################
tp_utils::StringID Geometry3D::getName() const
{
  return (!comments.empty())?tp_utils::StringID(comments.front()):material.name;
}

//##################################################################################################
nlohmann::json Geometry::saveState() const
{
  nlohmann::json j;
  j["geometry"] = vec2VectorToJSON(geometry);
  j["transform"] = mat4ToJSON(transform);
  j["material"] = material.saveState();
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
