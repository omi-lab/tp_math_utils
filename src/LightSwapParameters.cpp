#include "tp_math_utils/LightSwapParameters.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/JSONUtils.h"
#include "tp_utils/DebugUtils.h"

namespace tp_math_utils
{

//################################################################################################
tp_math_utils::Light LightSwapParameters::lightWithSwappedParameters(const tp_math_utils::Light& light,
                                                                              const LightParameters& userParams) const
{
  tp_math_utils::Light swapped = light;

  auto swapValue = [](float& v, float newValue, float use, float scale, float bias)
  {
    v = std::clamp((v*(1.0f-use)) + ( ( (newValue*scale) + bias) * use), 0.0f, 1.0f);
  };

  auto swapVec3 = [&](glm::vec3& currentValue,const glm::vec3& userValue, const glm::vec3& use, const glm::vec3& scale, const glm::vec3& bias)
  {
    swapValue(currentValue.x, userValue.x, use.x, scale.x, bias.x);
    swapValue(currentValue.y, userValue.y, use.y, scale.y, bias.y);
    swapValue(currentValue.z, userValue.z, use.z, scale.z, bias.z);
  };

  swapVec3(swapped.ambient,  userParams.ambient,  ambientUse,  ambientScale,  ambientBias);
  swapVec3(swapped.diffuse,  userParams.diffuse,  diffuseUse,  diffuseScale,  diffuseBias);
  swapVec3(swapped.specular, userParams.specular, specularUse, specularScale, specularBias);

  const glm::vec3 userOffsetScale {userParams.offsetScale, 0.0f, 0.0f};
  swapVec3(swapped.offsetScale, userOffsetScale, offsetScaleUse, offsetScaleScale, offsetScaleBias);

  swapValue(swapped.diffuseScale,   userParams.diffuseScale,   diffuseScaleUse,   diffuseScaleScale,   diffuseScaleBias);
  swapValue(swapped.spotLightBlend, userParams.spotLightBlend, spotLightBlendUse, spotLightBlendScale, spotLightBlendBias);
  swapValue(swapped.fov,            userParams.fov,            fovUse,            fovScale,            fovBias);

  return swapped;
}

//##################################################################################################
void LightSwapParameters::saveState(nlohmann::json& j) const
{
  j["initialColor"] = tp_math_utils::vec3ToJSON(initialColor);

  j["ambientUse"]          = tp_math_utils::vec3ToJSON(ambientUse);
  j["ambientScale"]        = tp_math_utils::vec3ToJSON(ambientScale);
  j["ambientBias"]         = tp_math_utils::vec3ToJSON(ambientBias);

  j["diffuseUse"]          = tp_math_utils::vec3ToJSON(diffuseUse);
  j["diffuseScale"]        = tp_math_utils::vec3ToJSON(diffuseScale);
  j["diffuseBias"]         = tp_math_utils::vec3ToJSON(diffuseBias);

  j["specularUse"]         = tp_math_utils::vec3ToJSON(specularUse);
  j["specularScale"]       = tp_math_utils::vec3ToJSON(specularScale);
  j["specularBias"]        = tp_math_utils::vec3ToJSON(specularBias);

  j["offsetScaleUse"]      = tp_math_utils::vec3ToJSON(offsetScaleUse);
  j["offsetScaleScale"]    = tp_math_utils::vec3ToJSON(offsetScaleScale);
  j["offsetScaleBias"]     = tp_math_utils::vec3ToJSON(offsetScaleBias);

  j["diffuseScaleUse"]     = diffuseScaleUse;
  j["diffuseScaleScale"]   = diffuseScaleScale;
  j["diffuseScaleBias"]    = diffuseScaleBias;

  j["spotLightBlendUse"]   = spotLightBlendUse;
  j["spotLightBlendScale"] = spotLightBlendScale;
  j["spotLightBlendBias"]  = spotLightBlendBias;

  j["fovUse"]              = fovUse;
  j["fovScale"]            = fovScale;
  j["fovBias"]             = fovBias;

}

//##################################################################################################
void LightSwapParameters::loadState(const nlohmann::json& j)
{
  initialColor        = TPJSONVec3(j, "initialColor", initialColor);
  
  ambientUse          = TPJSONVec3(j, "ambientUse"  ,   ambientUse  );
  ambientScale        = TPJSONVec3(j, "ambientScale",   ambientScale);
  ambientBias         = TPJSONVec3(j, "ambientBias" ,   ambientBias );

  diffuseUse          = TPJSONVec3(j, "diffuseUse"  ,      diffuseUse     );
  diffuseScale        = TPJSONVec3(j, "diffuseScale",      diffuseScale   );
  diffuseBias         = TPJSONVec3(j, "diffuseBias" ,      diffuseBias    );

  specularUse         = TPJSONVec3(j, "specularUse"  , specularUse  );
  specularScale       = TPJSONVec3(j, "specularScale", specularScale);
  specularBias        = TPJSONVec3(j, "specularBias" , specularBias );

  offsetScaleUse      = TPJSONVec3(j, "offsetScaleUse"  ,   offsetScaleUse  );
  offsetScaleScale    = TPJSONVec3(j, "offsetScaleScale",   offsetScaleScale);
  offsetScaleBias     = TPJSONVec3(j, "offsetScaleBias" ,   offsetScaleBias );

  diffuseScaleUse     = TPJSONFloat(j, "diffuseScaleUse");
  diffuseScaleScale   = TPJSONFloat(j, "diffuseScaleScale");
  diffuseScaleBias    = TPJSONFloat(j, "diffuseScaleBias");

  spotLightBlendUse   = TPJSONFloat(j, "spotLightBlendUse");
  spotLightBlendScale = TPJSONFloat(j, "spotLightBlendScale");
  spotLightBlendBias  = TPJSONFloat(j, "spotLightBlendBias");

  fovUse              = TPJSONFloat(j, "fovUse");
  fovScale            = TPJSONFloat(j, "fovScale");
  fovBias             = TPJSONFloat(j, "fovBias");
}

}
