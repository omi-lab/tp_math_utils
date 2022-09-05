#include "tp_math_utils/MaterialSwapParameters.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/JSONUtils.h"
#include "tp_utils/DebugUtils.h"

#include "glm/gtc/matrix_transform.hpp"

namespace tp_math_utils
{

tp_math_utils::Material MaterialSwapParameters::materialWithSwappedParameters( const tp_math_utils::Material& material, const glm::vec3& color ) const
{
  tp_math_utils::Material swapped = material;

  auto swapValue = [](float existingValue, float newValue, float use, float scale, float bias )
  {
    auto value = (existingValue*(1.0f-use)) + ( ( (newValue*scale) + bias) * use);
    if( value > 1.0f ) {
      value = 1.0f;
    }
    return value;
  };


  swapped.albedo.x = swapValue(swapped.albedo.x, color.x, albedoUse.x, albedoScale.x, albedoBias.x );
  swapped.albedo.y = swapValue(swapped.albedo.y, color.y, albedoUse.y, albedoScale.y, albedoBias.y );
  swapped.albedo.z = swapValue(swapped.albedo.z, color.z, albedoUse.z, albedoScale.z, albedoBias.z );

  swapped.sss.x = swapValue(swapped.sss.x, color.x, sssUse.x, sssScale.x, sssBias.x );
  swapped.sss.y = swapValue(swapped.sss.y, color.y, sssUse.y, sssScale.y, sssBias.y );
  swapped.sss.z = swapValue(swapped.sss.z, color.z, sssUse.z, sssScale.z, sssBias.z );

  swapped.emission.x = swapValue(swapped.emission.x, color.x, emissionUse.x, emissionScale.x, emissionBias.x );
  swapped.emission.y = swapValue(swapped.emission.y, color.y, emissionUse.y, emissionScale.y, emissionBias.y );
  swapped.emission.z = swapValue(swapped.emission.z, color.z, emissionUse.z, emissionScale.z, emissionBias.z );

  swapped.velvet.x = swapValue(swapped.velvet.x, color.x, velvetUse.x, velvetScale.x, velvetBias.x );
  swapped.velvet.y = swapValue(swapped.velvet.y, color.y, velvetUse.y, velvetScale.y, velvetBias.y );
  swapped.velvet.z = swapValue(swapped.velvet.z, color.z, velvetUse.z, velvetScale.z, velvetBias.z );

  swapped.albedoHue = albedoHue;

  return swapped;
}

//##################################################################################################
nlohmann::json MaterialSwapParameters::saveState() const
{
  nlohmann::json j;

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

  j["albedoHue"] = albedoHue;

  return j;
}

//##################################################################################################
void MaterialSwapParameters::loadState(const nlohmann::json& j)
{
  albedoUse        = tp_math_utils::vec3FromJSON(TPJSON(j, "albedoUse"  ),   albedoUse  );
  albedoScale      = tp_math_utils::vec3FromJSON(TPJSON(j, "albedoScale"),   albedoScale);
  albedoBias       = tp_math_utils::vec3FromJSON(TPJSON(j, "albedoBias" ),   albedoBias );

  sssUse           = tp_math_utils::vec3FromJSON(TPJSON(j, "sssUse"  ),      sssUse     );
  sssScale         = tp_math_utils::vec3FromJSON(TPJSON(j, "sssScale"),      sssScale   );
  sssBias          = tp_math_utils::vec3FromJSON(TPJSON(j, "sssBias" ),      sssBias    );

  emissionUse      = tp_math_utils::vec3FromJSON(TPJSON(j, "emissionUse"  ), emissionUse  );
  emissionScale    = tp_math_utils::vec3FromJSON(TPJSON(j, "emissionScale"), emissionScale);
  emissionBias     = tp_math_utils::vec3FromJSON(TPJSON(j, "emissionBias" ), emissionBias );

  velvetUse        = tp_math_utils::vec3FromJSON(TPJSON(j, "velvetUse"  ),   velvetUse  );
  velvetScale      = tp_math_utils::vec3FromJSON(TPJSON(j, "velvetScale"),   velvetScale);
  velvetBias       = tp_math_utils::vec3FromJSON(TPJSON(j, "velvetBias" ),   velvetBias );

  albedoHue        = TPJSONFloat(j, "albedoHueUse"   , albedoHue   );
}

}
