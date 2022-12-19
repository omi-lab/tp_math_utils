#ifndef tp_math_utils_MarchingCubes_h
#define tp_math_utils_MarchingCubes_h

#include "tp_math_utils/Geometry3D.h"

#include "tp_utils/Progress.h"

namespace tp_math_utils
{

//##################################################################################################
bool marchingCubes(const std::vector<bool>& cubeData,
                   const glm::vec<3, size_t>& size,
                   std::vector<Geometry3D>& geometry,
                   tp_utils::Progress* progress);

//##################################################################################################
/*!

\param cubeData stored as 1 bit per grid cell.
\param size of the grid.
\param geometry output.
\param progress output.
\return True for success.
*/
bool marchingCubes(const void* cubeData,
                   const glm::vec<3, size_t>& size,
                   std::vector<Geometry3D>& geometry,
                   tp_utils::Progress* progress);

}

#endif
