#include "tp_math_utils/UnwrapUVs.h"
#include "tp_math_utils/external/xatlas.h"

#include "tp_utils/DebugUtils.h"

namespace tp_math_utils
{

namespace
{

//##################################################################################################
struct ProgressStats_lt
{
  tp_utils::Progress* progress;
};

//##################################################################################################
// Must be thread safe
bool progressCallback(xatlas::ProgressCategory category, int progress, void* userData)
{
  ProgressStats_lt* progressStats = static_cast<ProgressStats_lt*>(userData);

  TP_UNUSED(category);
  TP_UNUSED(progress);
  TP_UNUSED(progressStats);

  return true;
}
}

//##################################################################################################
bool unwrapUVs(std::vector<Geometry3D>& geometry, tp_utils::Progress* progress)
{
  if(geometry.empty())
  {
    progress->setProgress(1.0f);
    return true;
  }

  ProgressStats_lt progressStats;
  progressStats.progress = progress;

  //xatlas::SetPrint(Print, s_verbose);

  xatlas::Atlas* atlas = xatlas::Create();
  TP_CLEANUP([&]{xatlas::Destroy(atlas);});

  xatlas::SetProgressCallback(atlas, progressCallback, &progressStats);


  //-- Add meshes ----------------------------------------------------------------------------------
  std::vector<Geometry3D*> geometryLookup;
  geometryLookup.reserve(geometry.size());
  {
    progressStats.progress = progress->addChildStep("Add meshes for UV unwrap.", 0.1f);

    // Add meshes to atlas.
    uint32_t totalVertices = 0;
    uint32_t totalFaces = 0;

    std::vector<std::unique_ptr<std::vector<uint32_t>>> indexesCopy;

    for(auto& mesh : geometry)
    {
      if(mesh.verts.empty())
        continue;

      mesh.convertToTriangles();

      if(mesh.indexes.empty())
        continue;

      const auto& indexes = mesh.indexes.front().indexes;

      if(indexes.empty())
        continue;

      geometryLookup.push_back(&mesh);

      auto& copy = indexesCopy.emplace_back(std::make_unique<std::vector<uint32_t>>());
      copy->reserve(indexes.size());
      for(const auto& i : indexes)
        copy->push_back(uint32_t(i));

      xatlas::MeshDecl meshDecl;
      meshDecl.vertexCount = uint32_t(mesh.verts.size());

      meshDecl.vertexPositionData = &mesh.verts.data()->vert;
      meshDecl.vertexPositionStride = sizeof(Vertex3D);

      meshDecl.vertexNormalData = &mesh.verts.data()->normal;
      meshDecl.vertexNormalStride = sizeof(Vertex3D);

      meshDecl.indexCount = uint32_t(indexes.size());
      meshDecl.indexData = copy->data();
      meshDecl.indexFormat = xatlas::IndexFormat::UInt32;

      xatlas::AddMeshError error = xatlas::AddMesh(atlas, meshDecl, uint32_t(geometry.size()));
      if (error != xatlas::AddMeshError::Success)
      {
        std::string errorStr = xatlas::StringForEnum(error);
        progressStats.progress->addError("Error adding mesh: " + errorStr);
        return false;
      }
    }

    // Only called here so geometry totals are printed after the AddMesh progress indicator.
    xatlas::AddMeshJoin(atlas);
    progress->addMessage("Total vertices " + std::to_string(totalVertices));
    progress->addMessage("Total faces " + std::to_string(totalFaces));

    progressStats.progress->setProgress(1.0f);
  }


  //-- Generate atlas ------------------------------------------------------------------------------
  {
    progressStats.progress = progress->addChildStep("Generating atlas.", 0.9f);
    xatlas::Generate(atlas);
    progressStats.progress->setProgress(1.0f);
  }


  //-- Copy UV coords out --------------------------------------------------------------------------
  {
    progressStats.progress = progress->addChildStep("Extract UV coords.", 1.0f);

    if(size_t(atlas->meshCount) != geometryLookup.size())
    {
      progressStats.progress->addError("Geometry size missmatch.");
      return false;
    }

    for (size_t i=0; i<size_t(atlas->meshCount); i++)
    {
      const xatlas::Mesh& mesh = atlas->meshes[i];
      Geometry3D* outMesh = geometryLookup.at(i);

      std::vector<Vertex3D> newVerts;
      newVerts.reserve(size_t(mesh.vertexCount));
      for (uint32_t v = 0; v < mesh.vertexCount; v++)
      {
        const xatlas::Vertex& vertex = mesh.vertexArray[v];
        const Vertex3D& srcVert = outMesh->verts.at(size_t(vertex.xref));

        Vertex3D& newVert = newVerts.emplace_back(srcVert);
        newVert.texture.x = vertex.uv[0];
        newVert.texture.y = vertex.uv[1];
      }

      auto& indexes = outMesh->indexes.front().indexes;
      for(size_t j=0; j<size_t(mesh.indexCount); j++)
        indexes[j] = int(mesh.indexArray[j]);

      outMesh->verts.swap(newVerts);
    }

    progressStats.progress->setProgress(1.0f);
  }

  return true;
}

}
