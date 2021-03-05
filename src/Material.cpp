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
  j["sss"]              = tp_math_utils::vec3ToJSON(sss);
  j["emission"]         = tp_math_utils::vec3ToJSON(emission);

  j["albedoScale"]      = albedoScale;
  j["sssScale"]         = sssScale;
  j["emissionScale"]    = emissionScale;
  j["heightScale"]      = heightScale;
  j["heightMidlevel"]   = heightMidlevel;

  j["alpha"]            = alpha;
  j["roughness"]        = roughness;
  j["metalness"]        = metalness;
  j["transmission"]     = transmission;
  j["ior"]              = ior;

  j["sssRadius"]        = tp_math_utils::vec3ToJSON(sssRadius);

  j["useAmbient"]       = useAmbient;
  j["useDiffuse"]       = useDiffuse;
  j["useNdotL"]         = useNdotL;
  j["useAttenuation"]   = useAttenuation;
  j["useShadow"]        = useShadow;
  j["useLightMask"]     = useLightMask;
  j["useReflection"]    = useReflection;

  j["tileTextures"]     = tileTextures;

  viewTypedTextures([&](const auto& type, const auto& value, const auto&)
  {
    j[type] = value.keyString();
  });

  return j;
}

//##################################################################################################
void Material::loadState(const nlohmann::json& j)
{
  name = TPJSONString(j, "name");

  albedo           = tp_math_utils::vec3FromJSON(TPJSON(j, "albedo"  ), albedo  );
  sss              = tp_math_utils::vec3FromJSON(TPJSON(j, "sss"     ), sss     );
  emission         = tp_math_utils::vec3FromJSON(TPJSON(j, "emission"), emission);

  albedoScale      = TPJSONFloat(j, "albedoScale"   , albedoScale   );
  sssScale         = TPJSONFloat(j, "sssScale"      , sssScale      );
  emissionScale    = TPJSONFloat(j, "emissionScale" , emissionScale );
  heightScale      = TPJSONFloat(j, "heightScale"   , heightScale   );
  heightMidlevel   = TPJSONFloat(j, "heightMidlevel", heightMidlevel);

  alpha            = TPJSONFloat(j, "alpha"       , alpha       );
  roughness        = TPJSONFloat(j, "roughness"   , roughness   );
  metalness        = TPJSONFloat(j, "metalness"   , metalness   );
  transmission     = TPJSONFloat(j, "transmission", transmission);
  ior              = TPJSONFloat(j, "ior"         , ior         );

  sssRadius        = tp_math_utils::vec3FromJSON(TPJSON(j, "sssRadius"), sssRadius);

  useAmbient       = TPJSONFloat(j, "useAmbient"    , useAmbient    );
  useDiffuse       = TPJSONFloat(j, "useDiffuse"    , useDiffuse    );
  useNdotL         = TPJSONFloat(j, "useNdotL"      , useNdotL      );
  useAttenuation   = TPJSONFloat(j, "useAttenuation", useAttenuation);
  useShadow        = TPJSONFloat(j, "useShadow"     , useShadow     );
  useLightMask     = TPJSONFloat(j, "useLightMask"  , useLightMask  );
  useReflection    = TPJSONFloat(j, "useReflection" , useReflection );

  tileTextures     = TPJSONBool(j, "tileTextures", false);

  updateTypedTextures([&](const auto& type, auto& value, const auto&)
  {
    value = TPJSONString(j, type);
  });
}

}
