#include "tp_math_utils/MaterialSwapParameters.h"
#include "tp_math_utils/materials/OpenGLMaterial.h"
#include "tp_math_utils/materials/LegacyMaterial.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/JSONUtils.h"

namespace tp_math_utils
{

//################################################################################################
tp_math_utils::Material MaterialSwapParameters::materialWithSwappedParameters(const tp_math_utils::Material& material,
                                                                              const glm::vec3& color) const
{
  tp_math_utils::Material swapped = material;

  glm::vec3 hsvColor = rgb2hsv(color);

  auto swapValue = [](float v, float newValue, float use, float scale, float bias)
  {
    v = std::clamp((v*(1.0f-use)) + ( ( (newValue*scale) + bias) * use), 0.0f, 1.0f);
  };

  auto swapVec3 = [&](glm::vec3& v, const glm::vec3& use, const glm::vec3& scale, const glm::vec3& bias)
  {
    swapValue(v.x, color.x, use.x, scale.x, bias.x);
    swapValue(v.y, color.y, use.y, scale.y, bias.y);
    swapValue(v.z, color.z, use.z, scale.z, bias.z);
  };


  for(auto& extendedMaterial : swapped.extendedMaterials)
  {
    if(auto m = dynamic_cast<OpenGLMaterial*>(extendedMaterial); m)
    {
      swapVec3(m->albedo  , albedoUse  , albedoScale  , albedoBias  );

      if(useAlbedoHue)
        m->albedoHue = hsvColor.x + 0.5f;

      swapValue(m->albedoSaturation, hsvColor.y, albedoSaturationUse, albedoSaturationScale,  albedoSaturationBias);
      swapValue(m->albedoValue     , hsvColor.z, albedoValueUse     , albedoValueScale     ,  albedoValueBias     );
    }

    else if(auto m = dynamic_cast<LegacyMaterial*>(extendedMaterial); m)
    {
      swapVec3(m->sss     , sssUse     , sssScale     , sssBias     );
      swapVec3(m->emission, emissionUse, emissionScale, emissionBias);
      swapVec3(m->velvet  , velvetUse  , velvetScale  , velvetBias  );
    }
  }

  return swapped;
}

//##################################################################################################
void MaterialSwapParameters::saveState(nlohmann::json& j) const
{
  j["albedoUse"] = tp_math_utils::vec3ToJSON    (albedoUse  );
  j["albedoScale"] = tp_math_utils::vec3ToJSON  (albedoScale);
  j["albedoBias"] = tp_math_utils::vec3ToJSON   (albedoBias );

  j["sssUse"] = tp_math_utils::vec3ToJSON       (sssUse     );
  j["sssScale"] = tp_math_utils::vec3ToJSON     (sssScale   );
  j["sssBias"] = tp_math_utils::vec3ToJSON      (sssBias    );

  j["emissionUse"] = tp_math_utils::vec3ToJSON  (emissionUse   );
  j["emissionScale"] = tp_math_utils::vec3ToJSON(emissionScale );
  j["emissionBias"] = tp_math_utils::vec3ToJSON (emissionBias  );

  j["velvetUse"] = tp_math_utils::vec3ToJSON    (velvetUse   );
  j["velvetScale"] = tp_math_utils::vec3ToJSON  (velvetScale );
  j["velvetBias"] = tp_math_utils::vec3ToJSON   (velvetBias  );

  j["useAlbedoHue"] = useAlbedoHue;

  j["initialColor"] = tp_math_utils::vec3ToJSON(initialColor);

  j["albedoSaturationUse"]    = albedoSaturationUse;
  j["albedoSaturationScale"]  = albedoSaturationScale;
  j["albedoSaturationBias"]   = albedoSaturationBias;

  j["albedoValueUse"]   = albedoValueUse;
  j["albedoValueScale"] = albedoValueScale;
  j["albedoValueBias"]  = albedoValueBias;
}

//##################################################################################################
void MaterialSwapParameters::loadState(const nlohmann::json& j)
{
  albedoUse        = TPJSONVec3(j, "albedoUse"  ,   albedoUse  );
  albedoScale      = TPJSONVec3(j, "albedoScale",   albedoScale);
  albedoBias       = TPJSONVec3(j, "albedoBias" ,   albedoBias );

  sssUse           = TPJSONVec3(j, "sssUse"  ,      sssUse     );
  sssScale         = TPJSONVec3(j, "sssScale",      sssScale   );
  sssBias          = TPJSONVec3(j, "sssBias" ,      sssBias    );

  emissionUse      = TPJSONVec3(j, "emissionUse"  , emissionUse  );
  emissionScale    = TPJSONVec3(j, "emissionScale", emissionScale);
  emissionBias     = TPJSONVec3(j, "emissionBias" , emissionBias );

  velvetUse        = TPJSONVec3(j, "velvetUse"  ,   velvetUse  );
  velvetScale      = TPJSONVec3(j, "velvetScale",   velvetScale);
  velvetBias       = TPJSONVec3(j, "velvetBias" ,   velvetBias );

  useAlbedoHue     = TPJSONBool(j, "useAlbedoHue", useAlbedoHue);

  initialColor     = TPJSONVec3(j, "initialColor", initialColor);

  albedoSaturationUse   = TPJSONFloat(j, "albedoSaturationUse");
  albedoSaturationScale = TPJSONFloat(j, "albedoSaturationScale");
  albedoSaturationBias  = TPJSONFloat(j, "albedoSaturationBias");

  albedoValueUse   = TPJSONFloat(j, "albedoValueUse");
  albedoValueScale = TPJSONFloat(j, "albedoValueScale");
  albedoValueBias  = TPJSONFloat(j, "albedoValueBias");
}

}
