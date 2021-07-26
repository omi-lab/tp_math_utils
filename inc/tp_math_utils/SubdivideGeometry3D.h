#ifndef tp_math_utils_SubdivideGeometry3D_h
#define tp_math_utils_SubdivideGeometry3D_h

#include "tp_math_utils/Geometry3D.h"

#include "tp_utils/DebugUtils.h"

#include "glm/gtx/norm.hpp"

#include <set>

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

  struct PairHash
  {
      template <class T1, class T2>
      std::size_t operator() (const std::pair<T1, T2> &pair) const {
          return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
      }
  };

  struct Triangle_lt
  {
    MIdx iM{0};          //!< Index of the mesh that this triangle is part of.
    VIdx iVs[3];         //!< Indexes of the 3 corner verts that make up this triangle.
    bool visible{false}; //!< Can the camera see this triangle.
    bool exclude{false}; //!< Set true once the triangle has been split.
  };

  struct Edge_lt
  {
    PIdx iPs[2];
    std::vector<TIdx> iTs; //!< Indexes of triangles that share this edge.
    float length;
    bool visible{false};   //!< Can the camera see any of the triangles attached to this edge.

    bool contains(PIdx iP)
    {
      return (iPs[0] == iP) || (iPs[1] == iP);
    }
  };

  struct EdgeLessThan
  {
    bool operator()(const Edge_lt* a, const Edge_lt* b) const
    {
      return a->length < b->length;
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
      //m_edges.reserve(faceCount*3*2);
      m_edgeLookup.reserve(faceCount*3*2);

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

          linkLastTriangle();
        }
      }
    }

    popDeadEdges();
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

    std::unique_ptr<Edge_lt> edge{takeLastEdge()};

    struct ExistingNewVerts
    {
      VIdx iVs[2];
      VIdx iVNew;
    };

    std::vector<ExistingNewVerts> midVerts;

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
              splitTriangle(t, midVert.iVNew);
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
            newVert.color   = (v0.color   + v1.color  ) / 2.0f;
            newVert.texture = (v0.texture + v1.texture) / 2.0f;
            newVert.normal  = glm::normalize(v0.normal  + v1.normal);
          }

          splitTriangle(t, midVert.iVNew);
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

    popDeadEdges();

    return count;
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
    return m_edges.empty()?0.0f:(*m_edges.rbegin())->length;
  }

private:

  //################################################################################################
  void splitTriangle(const Triangle_lt t, VIdx iVNew)
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
      linkLastTriangle();
    }

    {
      Triangle_lt& newTriangle = m_triangles.emplace_back();
      newTriangle.iM = t.iM;

      newTriangle.iVs[0] = iVNew;
      newTriangle.iVs[1] = t.iVs[1];
      newTriangle.iVs[2] = t.iVs[2];

      newTriangle.visible = t.visible?m_triangleVisible(pNew, p1, p2):false;
      linkLastTriangle();
    }
  }

  //################################################################################################
  float areaOfTraingle(const Triangle_lt& triangle)
  {
    glm::vec3 v0 = position(triangle.iVs[0]);
    glm::vec3 v1 = position(triangle.iVs[1]);
    glm::vec3 v2 = position(triangle.iVs[2]);

    glm::vec3 e1 = v1-v0;
    glm::vec3 e2 = v2-v0;
    glm::vec3 e3 = glm::cross(e1, e2);

    return 0.5f * glm::length(e3);
  }

  //################################################################################################
  bool isTraingleValid(const Triangle_lt& triangle)
  {
    PIdx i0 = positionIndex(triangle.iVs[0]);
    PIdx i1 = positionIndex(triangle.iVs[1]);
    PIdx i2 = positionIndex(triangle.iVs[2]);

    if(i0==i1 || i0==i2 || i1==i2)
      return false;

    return true;
  }

  //################################################################################################
  void linkLastTriangle()
  {
    TIdx iT = TIdx(m_triangles.size()-1);
    const Triangle_lt& t = m_triangles.back();

    if(isTraingleValid(t))
    {
      insertEdge(positionIndex(t.iVs[0]), positionIndex(t.iVs[1]), iT);
      insertEdge(positionIndex(t.iVs[1]), positionIndex(t.iVs[2]), iT);
      insertEdge(positionIndex(t.iVs[2]), positionIndex(t.iVs[0]), iT);
    }
    else
    {
      m_triangles.pop_back();
    }
  }

  //################################################################################################
  void insertEdge(PIdx iP0, PIdx iP1, TIdx iT)
  {
    if(iP1<iP0)
      std::swap(iP0, iP1);

    auto updateVisible = [&](Edge_lt* e)
    {
      e->visible = false;
      for(TIdx iT : e->iTs)
      {
        const auto& t = triangle(iT);
        if(t.visible && !t.exclude)
        {
          e->visible=true;
          return;
        }
      }
    };

    if(auto i = m_edgeLookup.find({iP0, iP1}); i!=m_edgeLookup.end())
    {
      i->second->iTs.push_back(iT);
      updateVisible(i->second);
      return;
    }

    float length = m_edgeLength(position(iP0), position(iP1));
    if(length >= m_maxLength)
    {
      auto e = new Edge_lt();
      e->iPs[0] = iP0;
      e->iPs[1] = iP1;
      e->iTs.push_back(iT);
      e->length = length;
      updateVisible(e);

      //{
      //  auto i = std::lower_bound(m_edges.begin(), m_edges.end(), length, [](const Edge_lt* a, float b){return a->length < b;});
      //  m_edges.insert(i, e);
      //  m_edgeLookup[{iP0, iP1}] = e;
      //}
      m_edges.insert(e);
      m_edgeLookup[{iP0, iP1}] = e;
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
    return m_positions.at(positionIndex(idx));
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
  //! If an edge is not connected to any visible triangles we don't need to care about it.
  /*!
  This pops dead edges until the list is empty or a valid edge if found.
  */
  void popDeadEdges()
  {
    while(!m_edges.empty() && !(*m_edges.rbegin())->visible)
      takeLastEdge();
  }

  //################################################################################################
  Edge_lt* takeLastEdge()
  {
    auto i = m_edges.rbegin();
    Edge_lt* e = *i;
    m_edges.erase(std::next(i).base());
    m_edgeLookup.erase({e->iPs[0], e->iPs[1]});
    return e;
  }


  Geometry3D* m_geometry;
  TriangleVisible m_triangleVisible;
  EdgeLength m_edgeLength;
  float m_maxLength;

  std::vector<glm::vec3>   m_positions;
  std::vector<PIdx>        m_positionLookup;
  std::vector<Triangle_lt> m_triangles;

  std::multiset<Edge_lt*, EdgeLessThan>                         m_edges;
  std::unordered_map<std::pair<PIdx, PIdx>, Edge_lt*, PairHash> m_edgeLookup;
};

}

#endif
