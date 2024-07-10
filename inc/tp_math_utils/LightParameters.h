#ifndef tp_math_utils_LightParameters_h
#define tp_math_utils_LightParameters_h

#include "tp_math_utils/Globals.h"

#include "json.hpp"

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_EXPORT LightParameters
{
  glm::vec3 ambient          {0.0f, 0.0f, 0.0f};
  glm::vec3 diffuse          {0.0f, 0.0f, 0.0f};
  glm::vec3 specular         {0.0f, 0.0f, 0.0f};

  float offsetScale          {0.0f};
  float diffuseScale         {0.0f};

  float spotLightBlend       {0.0f};
  float fov                  {0.0f};

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
