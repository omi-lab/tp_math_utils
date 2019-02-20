#ifndef tp_math_utils_Geometry3D_h
#define tp_math_utils_Geometry3D_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT Vertex3D
{
  glm::vec3 vert{0,0,0};
  glm::vec4 color{1,0,0,1};
  glm::vec2 texture{0,0};
  glm::vec3 normal{0,1,0};
};

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT Indexes3D
{
  int type{0};
  std::vector<int> indexes;
};

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT Geometry3D
{
  std::vector<std::string> comments;
  std::vector<Vertex3D> verts;
  std::vector<Indexes3D> indexes;

  //GL_TRIANGLE_FAN, GL_TRIANGLE_STRIP, GL_TRIANGLES
  int triangleFan  {1};
  int triangleStrip{2};
  int triangles    {3};

  //################################################################################################
  void calculateVertexNormals();

  //################################################################################################
  void calculateFaceNormals();
};

}

#endif
