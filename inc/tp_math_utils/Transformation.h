#ifndef tp_math_utils_Transformation_h
#define tp_math_utils_Transformation_h

#include "tp_math_utils/Plane.h"

namespace tp_math_utils
{

//##################################################################################################
class TP_MATH_UTILS_EXPORT Transformation
{
public:
  //################################################################################################
  //! Create a transformation for drawing on the flat
  Transformation();

  //################################################################################################
  //!
  void setPosition(const glm::vec3& position);

  //################################################################################################
  const glm::vec3& position() const;

  //################################################################################################
  void setScale(const glm::vec3& scale);

  //################################################################################################
  const glm::vec3& scale() const;

  //################################################################################################
  //! Define the surface that drawing happens on
  void setRotation(const glm::vec3& normal, const glm::vec3& upVector);


  //################################################################################################
  /*!
  \sa setRotation()
  \sa upVector()
  \returns The normal of the transformation
  */
  const glm::vec3& normal() const;

  //################################################################################################
  /*!
  \sa setRotation()
  \sa normal()
  \returns The up vector of the transformation
  */
  const glm::vec3& upVector() const;

  //################################################################################################
  //! Return the matrix generated from the other parameters
  const glm::mat4& matrix() const;

  //################################################################################################
  const Plane& plane() const;

private:
  Plane m_plane;
  glm::vec3 m_upVector;

  glm::vec3 m_scale;

  mutable bool m_matrixValid;
  mutable glm::mat4 m_matrix;
};
}

#endif
