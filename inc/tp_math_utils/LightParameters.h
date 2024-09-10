#ifndef tp_math_utils_LightParameters_h
#define tp_math_utils_LightParameters_h

#include "tp_math_utils/Globals.h"

#include "json.hpp"

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_EXPORT LightParameters
{
  glm::vec3 color           {0.5f, 0.5f, 0.5f};
  float offsetScale          {0.5f};
  float diffuseScale         {0.5f};
  float spotLightBlend       {0.5f};
  float fov                  {0.5f};

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
