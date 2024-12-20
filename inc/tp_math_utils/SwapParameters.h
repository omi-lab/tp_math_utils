#ifndef tp_math_utils_SwapParameters_h
#define tp_math_utils_SwapParameters_h

#include "tp_math_utils/Globals.h"

#include "json.hpp" // IWYU pragma: keep

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_EXPORT Vec3SwapParameters
{
  glm::vec3 use  {0.0f, 0.0f, 0.0f};
  glm::vec3 scale{1.0f, 1.0f, 1.0f};
  glm::vec3 bias {0.0f, 0.0f, 0.0f};

  //################################################################################################
  glm::vec3 apply(const glm::vec3& originalValue, const glm::vec3& selectedValue) const;

  //################################################################################################
  glm::vec3 apply(const glm::vec3& originalValue, float selectedValue) const;

  //################################################################################################
  //! Apply the value and clamp the output between 0 and 1.
  glm::vec3 applyClamped(const glm::vec3& originalValue, const glm::vec3& selectedValue) const;

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);

  //################################################################################################
  void loadState(const nlohmann::json& j, const std::string& prefix);
};

//##################################################################################################
struct TP_MATH_UTILS_EXPORT FloatSwapParameters
{
  float use  {0.0f};
  float scale{1.0f};
  float bias {0.0f};

  //################################################################################################
  float apply(float originalValue, float selectedValue) const;

  //################################################################################################
  float applyClamped(float originalValue, float selectedValue) const;

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);

  //################################################################################################
  void loadState(const nlohmann::json& j, const std::string& prefix);
};

}

#endif
