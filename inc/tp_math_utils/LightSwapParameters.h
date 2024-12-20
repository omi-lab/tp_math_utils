#ifndef tp_math_utils_LightSwapParameters_h
#define tp_math_utils_LightSwapParameters_h

#include "tp_math_utils/Globals.h"
#include "tp_math_utils/Light.h"
#include "tp_math_utils/SwapParameters.h"

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_EXPORT LightSwapInput
{
  glm::vec3 color     {0.5f, 0.5f, 0.5f};

  float diffuseScale  {0.5f};

  float spotLightBlend{0.5f};
  float fov           {0.5f};

  float offsetScale   {0.5f};

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

//##################################################################################################
struct TP_MATH_UTILS_EXPORT LightSwapParameters
{
  Vec3SwapParameters ambient;
  Vec3SwapParameters diffuse;
  Vec3SwapParameters specular;

  FloatSwapParameters diffuseScale;

  FloatSwapParameters spotLightBlend;
  FloatSwapParameters fov;

  Vec3SwapParameters offsetScale;

  //################################################################################################
  tp_math_utils::Light lightWithSwappedParameters(const tp_math_utils::Light& light,
                                                  const LightSwapInput& lightSwapInput) const;

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
