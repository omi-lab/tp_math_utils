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
  for(auto& extendedMaterial : swapped.extendedMaterials)
  {
    if(auto m = dynamic_cast<OpenGLMaterial*>(extendedMaterial); m)
    {
      m->albedo = albedo.applyClamped(m->albedo, color);

      if(useAlbedoHue)
        m->albedoHue = hsvColor.x + 0.5f;

      m->albedoSaturation = albedoSaturation.applyClamped(m->albedoSaturation, hsvColor.y);
      m->albedoValue      = albedoValue.applyClamped(m->albedoValue, hsvColor.z);
    }

    else if(auto m = dynamic_cast<LegacyMaterial*>(extendedMaterial); m)
    {
      m->sss      = sss     .applyClamped(m->sss     , color);
      m->emission = emission.applyClamped(m->emission, color);
      m->velvet   = velvet  .applyClamped(m->velvet  , color);
    }
  }

  return swapped;
}

//##################################################################################################
void MaterialSwapParameters::saveState(nlohmann::json& j) const
{
  albedo  .saveState(j["albedo"  ]);
  sss     .saveState(j["sss"     ]);
  emission.saveState(j["emission"]);
  velvet  .saveState(j["velvet"  ]);

  j["useAlbedoHue"] = useAlbedoHue;

  j["initialColor"] = tp_math_utils::vec3ToJSON(initialColor);

  albedoSaturation.saveState(j["albedoSaturation"]);
  albedoValue.saveState(j["albedoValue"]);
}

//##################################################################################################
void MaterialSwapParameters::loadState(const nlohmann::json& j)
{
  albedo  .loadState(j, "albedo"  );
  sss     .loadState(j, "sss"     );
  emission.loadState(j, "emission");
  velvet  .loadState(j, "velvet"  );

  useAlbedoHue     = TPJSONBool(j, "useAlbedoHue", useAlbedoHue);

  initialColor     = TPJSONVec3(j, "initialColor", initialColor);

  albedoSaturation.loadState(j, "albedoSaturation");
  albedoValue     .loadState(j, "albedoValue"     );
}

}
