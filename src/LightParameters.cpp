#include "tp_math_utils/LightParameters.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/JSONUtils.h"
#include "tp_utils/DebugUtils.h"

namespace tp_math_utils
{

//##################################################################################################
void LightParameters::saveState(nlohmann::json& j) const
{
  j["ambient"]        = tp_math_utils::vec3ToJSON(ambient);
  j["diffuse"]        = tp_math_utils::vec3ToJSON(diffuse);
  j["specular"]       = tp_math_utils::vec3ToJSON(specular);

  j["offsetScale"]    = offsetScale;
  j["diffuseScale"]   = diffuseScale;
  j["spotLightBlend"] = spotLightBlend;
  j["fov"]            = fov;
}

//##################################################################################################
void LightParameters::loadState(const nlohmann::json& j)
{
  ambient        = TPJSONVec3(j, "ambient"  ,   ambient);
  diffuse        = TPJSONVec3(j, "diffuse"  ,   diffuse);
  specular       = TPJSONVec3(j, "specular" ,   specular);

  offsetScale    = TPJSONFloat(j, "offsetScale"   , offsetScale);
  diffuseScale   = TPJSONFloat(j, "diffuseScale"  , diffuseScale);
  spotLightBlend = TPJSONFloat(j, "spotLightBlend", spotLightBlend);
  fov            = TPJSONFloat(j, "fov"           , fov);
}

}
