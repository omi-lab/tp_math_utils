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

  j["albedo"]                = tp_math_utils::vec3ToJSON(albedo);
  j["sss"]                   = tp_math_utils::vec3ToJSON(sss);
  j["emission"]              = tp_math_utils::vec3ToJSON(emission);
  j["velvet"]                = tp_math_utils::vec3ToJSON(velvet);

  j["albedoScale"]           = albedoScale;
  j["sssScale"]              = sssScale;
  j["emissionScale"]         = emissionScale;
  j["velvetScale"]           = velvetScale;
  j["heightScale"]           = heightScale;
  j["heightMidlevel"]        = heightMidlevel;

  j["alpha"]                 = alpha;
  j["roughness"]             = roughness;
  j["metalness"]             = metalness;
  j["specular"]              = specular;
  j["transmission"]          = transmission;
  j["transmissionRoughness"] = transmissionRoughness;
  j["ior"]                   = ior;
  j["sheen"]                 = sheen;
  j["sheenTint"]             = sheenTint;
  j["clearCoat"]             = clearCoat;
  j["clearCoatRoughness"]    = clearCoatRoughness;

  j["iridescentFactor"]      = iridescentFactor;
  j["iridescentOffset"]      = iridescentOffset;
  j["iridescentFrequency"]   = iridescentFrequency;

  j["sssRadius"]             = tp_math_utils::vec3ToJSON(sssRadius);

  j["albedoBrightness"]      = albedoBrightness;
  j["albedoContrast"]        = albedoContrast;
  j["albedoGamma"]           = albedoGamma;
  j["albedoHue"]             = albedoHue;
  j["albedoSaturation"]      = albedoSaturation;
  j["albedoValue"]           = albedoValue;
  j["albedoFactor"]          = albedoFactor;

  j["useAmbient"]            = useAmbient;
  j["useDiffuse"]            = useDiffuse;
  j["useNdotL"]              = useNdotL;
  j["useAttenuation"]        = useAttenuation;
  j["useShadow"]             = useShadow;
  j["useLightMask"]          = useLightMask;
  j["useReflection"]         = useReflection;

  j["tileTextures"]          = tileTextures;

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

  albedo                = tp_math_utils::vec3FromJSON(TPJSON(j, "albedo"  ), albedo  );
  sss                   = tp_math_utils::vec3FromJSON(TPJSON(j, "sss"     ), sss     );
  emission              = tp_math_utils::vec3FromJSON(TPJSON(j, "emission"), emission);
  velvet                = tp_math_utils::vec3FromJSON(TPJSON(j, "velvet"  ), velvet  );

  albedoScale           = TPJSONFloat(j, "albedoScale"   , albedoScale   );
  sssScale              = TPJSONFloat(j, "sssScale"      , sssScale      );
  emissionScale         = TPJSONFloat(j, "emissionScale" , emissionScale );
  velvetScale           = TPJSONFloat(j, "velvetScale"   , velvetScale   );
  heightScale           = TPJSONFloat(j, "heightScale"   , heightScale   );
  heightMidlevel        = TPJSONFloat(j, "heightMidlevel", heightMidlevel);

  alpha                 = TPJSONFloat(j, "alpha"                , alpha                );
  roughness             = TPJSONFloat(j, "roughness"            , roughness            );
  metalness             = TPJSONFloat(j, "metalness"            , metalness            );
  specular              = TPJSONFloat(j, "specular"             , specular             );
  transmission          = TPJSONFloat(j, "transmission"         , transmission         );
  transmissionRoughness = TPJSONFloat(j, "transmissionRoughness", transmissionRoughness);
  ior                   = TPJSONFloat(j, "ior"                  , ior                  );
  sheen                 = TPJSONFloat(j, "sheen"                , sheen                );
  sheenTint             = TPJSONFloat(j, "sheenTint"            , sheenTint            );
  clearCoat             = TPJSONFloat(j, "clearCoat"            , clearCoat            );
  clearCoatRoughness    = TPJSONFloat(j, "clearCoatRoughness"   , clearCoatRoughness   );
  iridescentFactor      = TPJSONFloat(j, "iridescentFactor"     , iridescentFactor     );
  iridescentOffset      = TPJSONFloat(j, "iridescentOffset"     , iridescentOffset     );
  iridescentFrequency   = TPJSONFloat(j, "iridescentFrequency"  , iridescentFrequency  );

  sssRadius        = tp_math_utils::vec3FromJSON(TPJSON(j, "sssRadius"), sssRadius);

  albedoBrightness = TPJSONFloat(j, "albedoBrightness", albedoBrightness);
  albedoContrast   = TPJSONFloat(j, "albedoContrast"  , albedoContrast  );
  albedoGamma      = TPJSONFloat(j, "albedoGamma"     , albedoGamma     );
  albedoHue        = TPJSONFloat(j, "albedoHue"       , albedoHue       );
  albedoSaturation = TPJSONFloat(j, "albedoSaturation", albedoSaturation);
  albedoValue      = TPJSONFloat(j, "albedoValue"     , albedoValue     );
  albedoFactor     = TPJSONFloat(j, "albedoFactor"    , albedoFactor    );

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
