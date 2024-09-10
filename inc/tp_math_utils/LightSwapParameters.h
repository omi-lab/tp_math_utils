#ifndef tp_math_utils_LightSwapParameters_h
#define tp_math_utils_LightSwapParameters_h

#include "tp_math_utils/Globals.h"
#include "tp_math_utils/Light.h"
#include "tp_math_utils/LightParameters.h"

#include "json.hpp"

#include <unordered_set>

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_EXPORT LightSwapParameters
{
  glm::vec3 ambientUse       {0.0f, 0.0f, 0.0f};
  glm::vec3 ambientScale     {1.0f, 1.0f, 1.0f};
  glm::vec3 ambientBias      {0.0f, 0.0f, 0.0f};

  glm::vec3 diffuseUse       {0.0f, 0.0f, 0.0f};
  glm::vec3 diffuseScale     {1.0f, 1.0f, 1.0f};
  glm::vec3 diffuseBias      {0.0f, 0.0f, 0.0f};

  glm::vec3 specularUse      {0.0f, 0.0f, 0.0f};
  glm::vec3 specularScale    {1.0f, 1.0f, 1.0f};
  glm::vec3 specularBias     {0.0f, 0.0f, 0.0f};

  glm::vec3 offsetScaleUse   {0.0f, 0.0f, 0.0f};
  glm::vec3 offsetScaleScale {1.0f, 1.0f, 1.0f};
  glm::vec3 offsetScaleBias  {0.0f, 0.0f, 0.0f};

  float diffuseScaleUse      {0.0f};
  float diffuseScaleScale    {1.0f};
  float diffuseScaleBias     {0.0f};
  float diffuseScaleDefault  {0.0f};

  float spotLightBlendUse    {0.0f};
  float spotLightBlendScale  {1.0f};
  float spotLightBlendBias   {0.0f};

  float fovUse               {0.0f};
  float fovScale             {1.0f};
  float fovBias              {0.0f};

  //################################################################################################
  tp_math_utils::Light lightWithSwappedParameters(const tp_math_utils::Light& light,
                                                        const LightParameters& userParams) const;

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
