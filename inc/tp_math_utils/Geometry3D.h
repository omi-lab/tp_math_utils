#ifndef tp_math_utils_Geometry3D_h
#define tp_math_utils_Geometry3D_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{

//##################################################################################################
enum class NormalCalculationMode
{
  None,
  CalculateFaceNormals,
  CalculateVertexNormals,
  CalculateTangentsAndBitangents
};

//##################################################################################################
std::vector<std::string> normalCalculationModes();

//##################################################################################################
std::string normalCalculationModeToString(NormalCalculationMode mode);

//##################################################################################################
NormalCalculationMode normalCalculationModeFromString(const std::string& mode);

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT Vertex3D
{
  glm::vec3 vert{0,0,0};
  glm::vec4 color{1,0,0,1};
  glm::vec2 texture{0,0};
  glm::vec3 normal{0,0,1};

  // Used to calculate lighting from normal and normal map.
  glm::vec3 tangent{1,0,0};
  glm::vec3 bitangent{0,1,0};

  //################################################################################################
  void calculateSimpleTangentAndBitangent();

  //################################################################################################
  void makeTangentAndBitangentOrthogonal();
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

  int triangleFan  {1}; //!< GL_TRIANGLE_FAN
  int triangleStrip{2}; //!< GL_TRIANGLE_STRIP
  int triangles    {3}; //!< GL_TRIANGLES

  //################################################################################################
  void calculateNormals(NormalCalculationMode mode);

  //################################################################################################
  void calculateVertexNormals();

  //################################################################################################
  void calculateFaceNormals();

  //################################################################################################
  void calculateTangentsAndBitangents();
};

}

#endif
