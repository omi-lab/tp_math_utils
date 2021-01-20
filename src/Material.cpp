#include "tp_math_utils/Material.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/JSONUtils.h"

namespace tp_math_utils
{

//##################################################################################################
nlohmann::json Material::saveState() const
{
  nlohmann::json j;

  j["name"] = name.keyString();

  j["albedo"]           = tp_math_utils::vec3ToJSON(albedo);
  j["specular"]         = tp_math_utils::vec3ToJSON(specular);

  j["alpha"]            = alpha;

  j["roughness"]        = roughness;
  j["metalness"]        = metalness;

  j["albedoScale"]      = albedoScale;
  j["specularScale"]    = specularScale;

  j["useAmbient"]       = useAmbient;
  j["useDiffuse"]       = useDiffuse;
  j["useNdotL"]         = useNdotL;
  j["useAttenuation"]   = useAttenuation;
  j["useShadow"]        = useShadow;
  j["useLightMask"]     = useLightMask;
  j["useReflection"]    = useReflection;

  j["tileTextures"]     = tileTextures;

  j["albedoTexture"]    = albedoTexture.keyString();
  j["specularTexture"]  = specularTexture.keyString();
  j["alphaTexture"]     = alphaTexture.keyString();
  j["normalsTexture"]   = normalsTexture.keyString();

  j["roughnessTexture"] = roughnessTexture.keyString();
  j["metalnessTexture"] = metalnessTexture.keyString();
  j["aoTexture"]        = aoTexture.keyString();

  return j;
}

//##################################################################################################
void Material::loadState(const nlohmann::json& j)
{
  name = TPJSONString(j, "name");

  albedo          = tp_math_utils::vec3FromJSON(TPJSON(j, "albedo"));
  specular        = tp_math_utils::vec3FromJSON(TPJSON(j, "specular"));

  alpha           = TPJSONFloat(j, "alpha", alpha);
  roughness       = TPJSONFloat(j, "roughness", roughness);
  metalness       = TPJSONFloat(j, "metalness", metalness);

  albedoScale     = TPJSONFloat(j, "albedoScale", 1.0f);
  specularScale   = TPJSONFloat(j, "specularScale", 1.0f);

  useAmbient      = TPJSONFloat(j, "useAmbient"    , 1.0f);
  useDiffuse      = TPJSONFloat(j, "useDiffuse"    , 1.0f);
  useNdotL        = TPJSONFloat(j, "useNdotL"      , 1.0f);
  useAttenuation  = TPJSONFloat(j, "useAttenuation", 1.0f);
  useShadow       = TPJSONFloat(j, "useShadow"     , 1.0f);
  useLightMask    = TPJSONFloat(j, "useLightMask"  , 1.0f);
  useReflection   = TPJSONFloat(j, "useReflection" , 1.0f);

  tileTextures    = TPJSONBool(j, "tileTextures", false);

  albedoTexture   = TPJSONString(j, "albedoTexture");
  specularTexture = TPJSONString(j, "specularTexture");
  alphaTexture    = TPJSONString(j, "alphaTexture");
  normalsTexture  = TPJSONString(j, "normalsTexture");
}

}
