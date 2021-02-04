#ifndef tp_math_utils_Geometry3D_h
#define tp_math_utils_Geometry3D_h

#include "tp_math_utils/Material.h"

namespace tp_math_utils
{

#define TP_TRIANGLES      0x0004 //!< GL_TRIANGLE_FAN
#define TP_TRIANGLE_STRIP 0x0005 //!< GL_TRIANGLE_STRIP
#define TP_TRIANGLE_FAN   0x0006 //!< GL_TRIANGLES

//##################################################################################################
enum class NormalCalculationMode
{
  None,
  CalculateFaceNormals,
  CalculateVertexNormals,
  CalculateAdaptiveNormals
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

  int triangleFan  {TP_TRIANGLE_FAN  };
  int triangleStrip{TP_TRIANGLE_STRIP};
  int triangles    {TP_TRIANGLES     };

  Material material;

  //##################################################################################################
  void add(const Geometry3D& other);

  //################################################################################################
  std::string stats() const;

  //################################################################################################
  void convertToTriangles();

  //################################################################################################
  void calculateNormals(NormalCalculationMode mode, float minDot=0.9f);

  //################################################################################################
  void calculateVertexNormals();

  //################################################################################################
  void calculateFaceNormals();

  //################################################################################################
  void calculateAdaptiveNormals(float minDot=0.9f);

  //################################################################################################
  void combineSimilarVerts();

  //################################################################################################
  void transform(const glm::mat4& m);

  //################################################################################################
  //! Duplicate and reverse geometry to render back faces.
  void addBackFaces();

  //################################################################################################
  tp_utils::StringID getName() const;
};

}

#endif
