#ifndef tp_math_utils_Sphere_h
#define tp_math_utils_Sphere_h

#include "tp_math_utils/Geometry3D.h"

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT Sphere
{
  //################################################################################################
  static Geometry3D icosahedralClass1(float radius,
                                      size_t division,
                                      int triangleFan,
                                      int triangleStrip,
                                      int triangles);

  //################################################################################################
  static Geometry3D octahedralClass1(float radius,
                                     size_t division,
                                     int triangleFan,
                                     int triangleStrip,
                                     int triangles);

  //################################################################################################
  static Geometry3D tetrahedralClass1(float radius,
                                      size_t division,
                                      int triangleFan,
                                      int triangleStrip,
                                      int triangles);

  //################################################################################################
  static void divideClass1(size_t division,
                           const glm::vec3& a,
                           const glm::vec3& b,
                           const glm::vec3& c,
                           std::vector<glm::vec3>& verts);

  //################################################################################################
  static Geometry3D indexAndScale(float radius,
                                  int triangleFan,
                                  int triangleStrip,
                                  int triangles,
                                  const std::vector<glm::vec3>& verts);
};

}

#endif
