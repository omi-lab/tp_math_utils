#ifndef tp_math_utils_SubdivideGeometry3D_h
#define tp_math_utils_SubdivideGeometry3D_h

#include "tp_math_utils/Geometry3D.h"

#include "tp_utils/DebugUtils.h"
#include "tp_utils/TimeUtils.h"
#include "tp_utils/Parallel.h"

#include "glm/gtx/norm.hpp"

#include <deque>
#include <list>

namespace tp_math_utils
{

//##################################################################################################
template<typename TriangleVisible, typename EdgeLength>
class SubdivideGeometry3D
{
  enum class VIdx : size_t {}; //!< Index into the vertex array.    (m_geometry->verts[VIdx], m_positionLookup[VIdx])
  enum class PIdx : size_t {}; //!< Index into the positions array. (m_positions[PIdx])
  enum class TIdx : size_t {}; //!< Index into the triangles array. (m_triangles[TIdx])
  enum class MIdx : size_t {}; //!< Index into the mesh array.      (m_geometry->indexes[MIdx])

  struct Edge_lt;

  struct Triangle_lt
  {
    MIdx iM{0};          //!< Index of the mesh that this triangle is part of.
    VIdx iVs[3];         //!< Indexes of the 3 corner verts that make up this triangle.
    bool visible{false}; //!< Can the camera see this triangle.
    bool exclude{false}; //!< Set true once the triangle has been split.

    float lengths[3];
    Edge_lt* edges[3];
  };

  struct Edge_lt
  {
    std::vector<TIdx> iTs; //!< Indexes of triangles that share this edge.
    PIdx iPs[2];
    float length;

    bool contains(PIdx iP)
    {
      return (iPs[0] == iP) || (iPs[1] == iP);
    }
  };

public:
  //################################################################################################
  SubdivideGeometry3D(Geometry3D* geometry,
                      TriangleVisible triangleVisible,
                      EdgeLength edgeLength,
                      float maxLength):
    m_geometry(geometry),
    m_triangleVisible(triangleVisible),
    m_edgeLength(edgeLength),
    m_maxLength(maxLength)
  {
    geometry->convertToTriangles();

    // Add all of the verts
    // We don't know how many verts we are actually going to generate so lets allocate a bit of
    // extra space and hope for the best.
    m_positionLookup.reserve(m_geometry->verts.size()*2);
    m_positions.reserve(m_geometry->verts.size()*2);
    for(size_t i=0; i<m_geometry->verts.size(); i++)
    {
      const auto& v = m_geometry->verts.at(i);

      // Try to find an existing position that is similar or the same as this.
      bool foundExisting=false;
      for(size_t j=0; j<m_positions.size(); j++)
      {
        if(glm::distance2(m_positions.at(j), v.vert) < 0.000001f)
        {
          m_positionLookup.push_back(PIdx(j));
          foundExisting = true;
          break;
        }
      }

      if(foundExisting)
        continue;

      m_positionLookup.push_back(PIdx(m_positions.size()));
      m_positions.push_back(v.vert);
    }

    // Add triangles and edges
    {
      size_t faceCount=0;
      for(const auto& mesh : geometry->indexes)
        faceCount += mesh.indexes.size() / 3;

      // Again guess how many triangles and edges we may need.
      m_triangles.reserve(faceCount*2);

      for(size_t iM=0; iM<geometry->indexes.size(); iM++)
      {
        const auto& mesh = geometry->indexes.at(iM);

        for(size_t c=0; (c+2)<mesh.indexes.size(); c+=3)
        {
          Triangle_lt& triangle = m_triangles.emplace_back();
          triangle.iM = MIdx(iM);

          VIdx i0 = VIdx(mesh.indexes.at(c));
          VIdx i1 = VIdx(mesh.indexes.at(c+1));
          VIdx i2 = VIdx(mesh.indexes.at(c+2));

          const glm::vec3& v0 = vertex(i0).vert;
          const glm::vec3& v1 = vertex(i1).vert;
          const glm::vec3& v2 = vertex(i2).vert;

          triangle.iVs[0] = i0;
          triangle.iVs[1] = i1;
          triangle.iVs[2] = i2;

          triangle.visible = m_triangleVisible(v0, v1, v2);

          std::unordered_set<Edge_lt*> dirtyEdges;
          linkLastTriangle(dirtyEdges);
        }
      }
    }

    removeDeadEdges();
  }

  //################################################################################################
  //! Divide the longest edge.
  /*!
  \return The number of extra triangles generated.
  */
  size_t divideOnce()
  {
    if(m_edges.empty())
      return 0;

    size_t count=0;

    std::unique_ptr<Edge_lt> edge = tpTakeLast(m_edges);

    struct ExistingNewVerts
    {
      VIdx iVs[2];
      VIdx iVNew;
    };

    std::vector<ExistingNewVerts> midVerts;
    std::unordered_set<Edge_lt*> dirtyEdges;
    {
      // Calculate the midpoint
      glm::vec3 p0 = position(edge->iPs[0]);
      glm::vec3 p1 = position(edge->iPs[1]);

      PIdx iPNew = PIdx(m_positions.size());
      m_positions.push_back((p0 + p1) / 2.0f);

      for(TIdx iT : edge->iTs)
      {
        Triangle_lt& t = triangle(iT);
        if(t.exclude)
          continue;

        t.exclude = true;

        if(t.edges[0])dirtyEdges.insert(t.edges[0]);
        if(t.edges[1])dirtyEdges.insert(t.edges[1]);
        if(t.edges[2])dirtyEdges.insert(t.edges[2]);

        for(size_t a=0; a<3; a++)
        {
          VIdx iV0 = t.iVs[0];
          VIdx iV1 = t.iVs[1];
          PIdx iP0 = positionIndex(iV0);
          PIdx iP1 = positionIndex(iV1);
          if(edge->contains(iP0) && edge->contains(iP1))
          {
            if(iV1<iV0)
              std::swap(iV0, iV1);

            bool found=false;
            for(const auto& midVert : midVerts)
            {
              if(midVert.iVs[0] == iV0 && midVert.iVs[1] == iV1)
              {
                found = true;
                splitTriangle(t, midVert.iVNew, dirtyEdges);
                count++;
                break;
              }
            }

            if(found)
              break;

            auto& midVert = midVerts.emplace_back();
            midVert.iVs[0] = iV0;
            midVert.iVs[1] = iV1;
            midVert.iVNew  = VIdx(m_geometry->verts.size());

            {
              m_positionLookup.push_back(iPNew);
              auto& newVert = m_geometry->verts.emplace_back();
              newVert.vert    = position(iPNew);
              const auto& v0 = vertex(iV0);
              const auto& v1 = vertex(iV1);
              newVert.texture = (v0.texture + v1.texture) / 2.0f;
              newVert.normal  = glm::normalize(v0.normal  + v1.normal);
            }

            splitTriangle(t, midVert.iVNew, dirtyEdges);
            count++;
            break;
          }

          {
            VIdx iV  = t.iVs[0];
            t.iVs[0] = t.iVs[1];
            t.iVs[1] = t.iVs[2];
            t.iVs[2] = iV;
          }
        }
      }
    }

    removeDeadEdges(dirtyEdges);

    return count;
  }

  //################################################################################################
  void reindex(float lengthF)
  {
    m_maxLength = lengthF;
    m_edges.clear();

    auto triangleExcluded = [&](const Triangle_lt& t)
    {
      return t.exclude;
    };

    m_triangles.erase(std::remove_if(m_triangles.begin(), m_triangles.end(), triangleExcluded), m_triangles.end());

    struct Positions
    {
      PIdx iPs[3];
    };

    for(size_t iT=0; iT<m_triangles.size(); iT++)
    {
      Triangle_lt& t = m_triangles.at(iT);

       PIdx iPs[3];
       iPs[0] = positionIndex(t.iVs[0]);
       iPs[1] = positionIndex(t.iVs[1]);
       iPs[2] = positionIndex(t.iVs[2]);

      t.edges[0] = insertEdge(iPs[0], iPs[1], TIdx(iT), t.lengths[0]);
      t.edges[1] = insertEdge(iPs[1], iPs[2], TIdx(iT), t.lengths[1]);
      t.edges[2] = insertEdge(iPs[2], iPs[0], TIdx(iT), t.lengths[2]);
    }

    removeDeadEdges();
  }

  //################################################################################################
  //! Write the results back to the geometry.
  void finalize()
  {
    for(auto& mesh : m_geometry->indexes)
      mesh.indexes.clear();

    {
      std::unordered_map<size_t, size_t> meshSize;
      for(const auto& triangle : m_triangles)
        if(!triangle.exclude)
          meshSize[size_t(triangle.iM)] += 3;

      for(size_t m=0; m<m_geometry->indexes.size(); m++)
        m_geometry->indexes.at(m).indexes.reserve(meshSize[m]);
    }

    for(const auto& triangle : m_triangles)
    {
      if(triangle.exclude)
        continue;

      auto& mesh = m_geometry->indexes.at(size_t(triangle.iM));

      for(size_t i=0; i<3; i++)
        mesh.indexes.push_back(int(triangle.iVs[i]));
    }
  }

  //################################################################################################
  float longestEdge() const
  {
    return m_edges.empty()?0.0f:m_edges.back()->length;
  }

private:

  //################################################################################################
  void splitTriangle(const Triangle_lt t, VIdx iVNew, std::unordered_set<Edge_lt*>& dirtyEdges)
  {
    // We know that the edge that needs cutting is between t.verts[0] and t.verts[1] because the
    // triangle is rotated until this is true by divideOnce.

    PIdx iPNew = positionIndex(iVNew);
    PIdx iP0   = positionIndex(t.iVs[0]);
    PIdx iP1   = positionIndex(t.iVs[1]);
    PIdx iP2   = positionIndex(t.iVs[2]);

    const auto& pNew = position(iPNew);
    const auto& p0   = position(iP0  );
    const auto& p1   = position(iP1  );
    const auto& p2   = position(iP2  );

    {
      Triangle_lt& newTriangle = m_triangles.emplace_back();
      newTriangle.iM = t.iM;

      newTriangle.iVs[0] = t.iVs[0];
      newTriangle.iVs[1] = iVNew;
      newTriangle.iVs[2] = t.iVs[2];

      newTriangle.visible = t.visible?m_triangleVisible(p0, pNew, p2):false;
      linkLastTriangle(dirtyEdges);
    }

    {
      Triangle_lt& newTriangle = m_triangles.emplace_back();
      newTriangle.iM = t.iM;

      newTriangle.iVs[0] = iVNew;
      newTriangle.iVs[1] = t.iVs[1];
      newTriangle.iVs[2] = t.iVs[2];

      newTriangle.visible = t.visible?m_triangleVisible(pNew, p1, p2):false;
      linkLastTriangle(dirtyEdges);
    }
  }

  //################################################################################################
  double triangleArea2(const Triangle_lt& triangle)
  {
    glm::dvec3 v0 = position(triangle.iVs[0]);
    glm::dvec3 v1 = position(triangle.iVs[1]);
    glm::dvec3 v2 = position(triangle.iVs[2]);

    glm::dvec3 e1 = v1-v0;
    glm::dvec3 e2 = v2-v0;
    glm::dvec3 e3 = glm::cross(e1, e2);

    return glm::length2(e3);
  }

  //################################################################################################
  bool pointsAreColinear(const Triangle_lt& triangle)
  {
    return triangleArea2(triangle) < 0.00000000000000001;
  }

  //################################################################################################
  bool isTraingleValid(const Triangle_lt& triangle)
  {
    PIdx i0 = positionIndex(triangle.iVs[0]);
    PIdx i1 = positionIndex(triangle.iVs[1]);
    PIdx i2 = positionIndex(triangle.iVs[2]);

    if(i0==i1 || i0==i2 || i1==i2)
      return false;

    if(pointsAreColinear(triangle))
      return false;

    return true;
  }

  //################################################################################################
  void linkLastTriangle(std::unordered_set<Edge_lt*>& dirtyEdges)
  {
    TIdx iT = TIdx(m_triangles.size()-1);
    Triangle_lt& t = m_triangles.back();

    if(isTraingleValid(t))
    {
      if(t.visible && triangleArea2(t)<0.000000000000001)
        t.visible = false;

      t.edges[0] = insertEdge(positionIndex(t.iVs[0]), positionIndex(t.iVs[1]), iT);
      t.edges[1] = insertEdge(positionIndex(t.iVs[1]), positionIndex(t.iVs[2]), iT);
      t.edges[2] = insertEdge(positionIndex(t.iVs[2]), positionIndex(t.iVs[0]), iT);

      if(t.edges[0])dirtyEdges.insert(t.edges[0]);
      if(t.edges[1])dirtyEdges.insert(t.edges[1]);
      if(t.edges[2])dirtyEdges.insert(t.edges[2]);

      t.lengths[0] = m_edgeLength(position(t.iVs[0]), position(t.iVs[1]));
      t.lengths[1] = m_edgeLength(position(t.iVs[1]), position(t.iVs[2]));
      t.lengths[2] = m_edgeLength(position(t.iVs[2]), position(t.iVs[0]));

    }
    else
    {
      m_triangles.pop_back();
    }
  }

  //################################################################################################
  void debugCounts()
  {
    static int64_t lastT{0};
    if(tp_utils::currentTimeMS()>lastT)
    {
      lastT = tp_utils::currentTimeMS() + 4000;

      size_t visible=0;
      size_t notVisible=0;
      for(const auto& triangle : m_triangles)
      {
        if(triangle.exclude)
          continue;

        if(triangle.visible)
          visible++;
        else
          notVisible++;
      }

      tpWarning() << "Edges: " << m_edges.size() << " Visible: " << visible << " Not visible: " << notVisible;
    }
  }
  //################################################################################################
  Edge_lt* insertEdge(PIdx iP0, PIdx iP1, TIdx iT)
  {
    return insertEdge(iP0, iP1, iT, m_edgeLength(position(iP0), position(iP1)));
  }

  //################################################################################################
  Edge_lt* insertEdge(PIdx iP0, PIdx iP1, TIdx iT, float length)
  {
    if(length < m_maxLength)
      return nullptr;

    if(iP1<iP0)
      std::swap(iP0, iP1);

    Edge_lt key;
    key.length = length;
    auto i = std::lower_bound(m_edges.begin(), m_edges.end(), &key, [&](const auto& a, const auto& b)
    {
      return a->length < b->length;
    });

    for(; i!=m_edges.end() && (*i)->length<=length; ++i)
    {
      if((*i)->iPs[0] == iP0 && (*i)->iPs[1] == iP1)
      {
        (*i)->iTs.push_back(iT);
        return (*i).get();
      }
    }

    {
      Edge_lt* e = new Edge_lt();
      e->iPs[0] = iP0;
      e->iPs[1] = iP1;
      e->iTs.push_back(iT);
      e->length = length;

      m_edges.insert(i, std::unique_ptr<Edge_lt>(e));
      return e;
    }
  }

  //################################################################################################
  Vertex3D& vertex(VIdx idx)
  {
    return m_geometry->verts.at(size_t(idx));
  }

  //################################################################################################
  PIdx positionIndex(VIdx idx)
  {
    return m_positionLookup.at(size_t(idx));
  }

  //################################################################################################
  glm::vec3& position(VIdx idx)
  {
    return m_positions.at(size_t(positionIndex(idx)));
  }

  //################################################################################################
  glm::vec3& position(PIdx idx)
  {
    return m_positions.at(size_t(idx));
  }

  //################################################################################################
  Triangle_lt& triangle(TIdx idx)
  {
    return m_triangles.at(size_t(idx));
  }

  //################################################################################################
  void unlinkEdge(const Edge_lt* e)
  {
    for(TIdx iT : e->iTs)
    {
      auto& t = triangle(iT);
      if(t.edges[0] == e) t.edges[0]=nullptr;
      if(t.edges[1] == e) t.edges[1]=nullptr;
      if(t.edges[2] == e) t.edges[2]=nullptr;
    }
  }

  //################################################################################################
  bool edgeNotVisible(const Edge_lt* e)
  {
    for(TIdx iT : e->iTs)
    {
      const auto& t = triangle(iT);
      if(t.visible && !t.exclude)
        return false;
    }

    return true;
  }


  //################################################################################################
  //! If an edge is not connected to any visible triangles we don't need to care about it.
  void removeDeadEdges()
  {
    auto edgeNotVisible = [&](const std::unique_ptr<Edge_lt>& e)
    {
      bool r = this->edgeNotVisible(e.get());
      if(r)
        unlinkEdge(e.get());
      return r;
    };

    m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(), edgeNotVisible), m_edges.end());
  }

  //################################################################################################
  void removeDeadEdges(const std::unordered_set<Edge_lt*>& dirtyEdges)
  {
    std::unordered_set<Edge_lt*> deadEdges;
    for(auto dirtyEdge : dirtyEdges)
      if(edgeNotVisible(dirtyEdge))
        deadEdges.insert(dirtyEdge);

    for(auto e : deadEdges)
      unlinkEdge(e);

    if(deadEdges.size() == 1)
    {
      auto e = *deadEdges.begin();
      for(auto i=m_edges.rbegin(); i!=m_edges.rend(); ++i)
      {
        if(i->get() == e)
        {
          m_edges.erase(--(i.base()));
          break;
        }
      }
    }
    else
    {
      auto edgeNotVisible = [&](const std::unique_ptr<Edge_lt>& e)
      {
        return tpContains(deadEdges, e.get());
      };

      m_edges.erase(std::remove_if(m_edges.begin(), m_edges.end(), edgeNotVisible), m_edges.end());
    }
  }

  Geometry3D* m_geometry;
  TriangleVisible m_triangleVisible;
  EdgeLength m_edgeLength;
  float m_maxLength;

  std::vector<glm::vec3>   m_positions;
  std::vector<PIdx>        m_positionLookup;
  std::vector<Triangle_lt> m_triangles;

  std::vector<std::unique_ptr<Edge_lt>> m_edges;
};

}

#endif
