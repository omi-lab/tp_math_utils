#ifndef tp_math_utils_Geometry3D_h
#define tp_math_utils_Geometry3D_h

#include "tp_math_utils/Material.h"

namespace tp_math_utils
{

class Plane;

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

  //################################################################################################
  //! Lerp: (1-u)*v0 + u*v1
  static Vertex3D interpolate(float u, const Vertex3D& v0, const Vertex3D& v1);

  //################################################################################################
  static Vertex3D interpolate(float u, float v, const Vertex3D& v0, const Vertex3D& v1, const Vertex3D& v2);
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
  // Create subdivisions in the mesh to split the parts outside of the given frustum.
  void subdivideAroundFrustum(const glm::mat4& modelMatrix,
                              const std::array<tp_math_utils::Plane, 6>& frustumPlanes,
                              const std::function<bool(const glm::vec4&)>& isPointInCamera);

  //################################################################################################
  tp_utils::StringID getName() const;
};

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT Geometry
{
  std::vector<glm::vec2> geometry;
  glm::mat4 transform{1.0f};
  tp_math_utils::Material material;

  //################################################################################################
  nlohmann::json saveState() const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
