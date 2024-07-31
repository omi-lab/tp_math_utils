#include "tp_math_utils/LightParameters.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/JSONUtils.h"
#include "tp_utils/DebugUtils.h"

namespace tp_math_utils
{

//##################################################################################################
void LightParameters::saveState(nlohmann::json& j) const
{
  j["color"]          = tp_math_utils::vec3ToJSON(color);

  j["offsetScale"]    = offsetScale;
  j["diffuseScale"]   = diffuseScale;
  j["spotLightBlend"] = spotLightBlend;
  j["fov"]            = fov;
}

//##################################################################################################
void LightParameters::loadState(const nlohmann::json& j)
{
  color          = TPJSONVec3(j, "color"  ,   color);

  offsetScale    = TPJSONFloat(j, "offsetScale"   , offsetScale);
  diffuseScale   = TPJSONFloat(j, "diffuseScale"  , diffuseScale);
  spotLightBlend = TPJSONFloat(j, "spotLightBlend", spotLightBlend);
  fov            = TPJSONFloat(j, "fov"           , fov);
}

}
