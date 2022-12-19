#include "tp_math_utils/MarchingCubes.h"
#include "tp_math_utils/external/MC33/MC33.h"
#include "tp_math_utils/external/MC33/Grid3DBool.h"
#include "tp_math_utils/external/MC33/Grid3DBits.h"

namespace tp_math_utils
{

//##################################################################################################
bool marchingCubes(Grid3DBase* grid,
                   std::vector<Geometry3D>& geometry,
                   tp_utils::Progress* progress)
{
  MC33 mc;
  mc.setGrid3D(grid);

  progress->addMessage("Populated grid.");
  progress->setProgress(0.1f);

  float isovalue = 0.5f;

  auto& mesh = geometry.emplace_back();
  mc.calculateIsosurface(mesh, isovalue);

  progress->addMessage("Calculated surface.");
  progress->setProgress(1.0f);

  return true;
}

//##################################################################################################
bool marchingCubes(const std::vector<bool>& cubeData,
                   const glm::vec<3, size_t>& size,
                   std::vector<Geometry3D>& geometry,
                   tp_utils::Progress* progress)
{
  Grid3DBool grid(size, &cubeData);
  return marchingCubes(&grid, geometry, progress);
}

//##################################################################################################
bool marchingCubes(const void* cubeData,
                   const glm::vec<3, size_t>& size,
                   std::vector<Geometry3D>& geometry,
                   tp_utils::Progress* progress)
{
  Grid3DBits grid(size, cubeData);
  return marchingCubes(&grid, geometry, progress);
}

}
