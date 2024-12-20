#include "tp_math_utils/LightSwapParameters.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/JSONUtils.h"
#include <optional>

namespace tp_math_utils
{

//##################################################################################################
void LightSwapInput::saveState(nlohmann::json& j) const
{
  j["color"]          = tp_math_utils::vec3ToJSON(color);

  j["diffuseScale"]   = diffuseScale;

  j["spotLightBlend"] = spotLightBlend;
  j["fov"]            = fov;

  j["offsetScale"]    = offsetScale;
}

//##################################################################################################
void LightSwapInput::loadState(const nlohmann::json& j)
{
  color          = TPJSONVec3(j, "color"          ,   color       );

  diffuseScale   = TPJSONFloat(j, "diffuseScale"  , diffuseScale  );

  spotLightBlend = TPJSONFloat(j, "spotLightBlend", spotLightBlend);
  fov            = TPJSONFloat(j, "fov"           , fov           );

  offsetScale    = TPJSONFloat(j, "offsetScale"   , offsetScale   );
}

//################################################################################################
tp_math_utils::Light LightSwapParameters::lightWithSwappedParameters(const tp_math_utils::Light& light,
                                                                     const LightSwapInput& userParams) const
{
  tp_math_utils::Light swapped = light;

  swapped.ambient  = ambient .applyClamped(light.ambient , userParams.color);
  swapped.diffuse  = diffuse .applyClamped(light.diffuse , userParams.color);
  swapped.specular = specular.applyClamped(light.specular, userParams.color);

  swapped.diffuseScale   = diffuseScale  .apply(light.diffuseScale  , userParams.diffuseScale  );

  swapped.spotLightBlend = spotLightBlend.apply(light.spotLightBlend, userParams.spotLightBlend);
  swapped.fov            = fov           .apply(light.fov           , userParams.fov           );

  swapped.offsetScale    = offsetScale   .apply(light.offsetScale   , userParams.offsetScale   );

  return swapped;
}

//##################################################################################################
void LightSwapParameters::saveState(nlohmann::json& j) const
{
  ambient.saveState(j["ambient"]);
  diffuse.saveState(j["diffuse"]);
  specular.saveState(j["specular"]);

  diffuseScale.saveState(j["diffuseScale"]);

  spotLightBlend.saveState(j["spotLightBlend"]);
  fov.saveState(j["fov"]);

  offsetScale.saveState(j["offsetScale"]);
}

//##################################################################################################
void LightSwapParameters::loadState(const nlohmann::json& j)
{
  ambient.loadState(j, "ambient");
  diffuse.loadState(j, "diffuse");
  specular.loadState(j, "specular");

  diffuseScale.loadState(j, "diffuseScale");

  spotLightBlend.loadState(j, "spotLightBlend");
  fov.loadState(j, "fov");

  offsetScale.loadState(j, "offsetScale");
}

}
