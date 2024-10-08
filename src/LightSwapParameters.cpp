#include "tp_math_utils/LightSwapParameters.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/JSONUtils.h"
#include "tp_utils/DebugUtils.h"
#include <optional>

namespace tp_math_utils
{

//################################################################################################
tp_math_utils::Light LightSwapParameters::lightWithSwappedParameters(const tp_math_utils::Light& light,
                                                                              const LightParameters& userParams) const
{
  tp_math_utils::Light swapped = light;

  auto swapValue = [](float& v, float newValue, float use, float scale, float bias, std::optional<float> min = std::nullopt)
  {
    v = ((v*(1.0f-use)) + ( ( (newValue*scale) + bias) * use));
    if(min)
        v = std::max(v,*min);
  };

  auto swapVec3 = [&](glm::vec3& currentValue,const glm::vec3& userValue, const glm::vec3& use, const glm::vec3& scale, const glm::vec3& bias)
  {
    swapValue(currentValue.x, userValue.x, use.x, scale.x, bias.x);
    swapValue(currentValue.y, userValue.y, use.y, scale.y, bias.y);
    swapValue(currentValue.z, userValue.z, use.z, scale.z, bias.z);
  };

  swapVec3(swapped.ambient,  userParams.color,  ambientUse,  ambientScale,  ambientBias);
  swapVec3(swapped.diffuse,  userParams.color,  diffuseUse,  diffuseScale,  diffuseBias);
  swapVec3(swapped.specular, userParams.color, specularUse, specularScale, specularBias);

  const glm::vec3 userOffsetScale {userParams.offsetScale * 2.0f, userParams.offsetScale * 2.0f, userParams.offsetScale * 2.0f};
  swapVec3(swapped.offsetScale, userOffsetScale, offsetScaleUse, offsetScaleScale, offsetScaleBias);

  swapValue(swapped.diffuseScale,   userParams.diffuseScale,   diffuseScaleUse,   diffuseScaleScale,   diffuseScaleBias,  diffuseScaleMin);

  swapValue(swapped.spotLightBlend, userParams.spotLightBlend * 2.0f, spotLightBlendUse, spotLightBlendScale, spotLightBlendBias);
  swapValue(swapped.fov,            userParams.fov * 2.0f,            fovUse,            fovScale,            fovBias);

  return swapped;
}

//##################################################################################################
void LightSwapParameters::saveState(nlohmann::json& j) const
{
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
  j["diffuseScaleDefault"] = diffuseScaleDefault;
  j["diffuseScaleMin"]     = diffuseScaleMin;

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
  diffuseScaleDefault = TPJSONFloat(j, "diffuseScaleDefault");
  diffuseScaleMin     = TPJSONFloat(j, "diffuseScaleMin");

  spotLightBlendUse   = TPJSONFloat(j, "spotLightBlendUse");
  spotLightBlendScale = TPJSONFloat(j, "spotLightBlendScale");
  spotLightBlendBias  = TPJSONFloat(j, "spotLightBlendBias");

  fovUse              = TPJSONFloat(j, "fovUse");
  fovScale            = TPJSONFloat(j, "fovScale");
  fovBias             = TPJSONFloat(j, "fovBias");
}

}
