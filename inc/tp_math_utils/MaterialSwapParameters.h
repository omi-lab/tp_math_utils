#ifndef tp_math_utils_MaterialSwapParameters_h
#define tp_math_utils_MaterialSwapParameters_h

#include "tp_math_utils/Globals.h"
#include "tp_math_utils/Material.h"

#include "json.hpp"

#include <unordered_set>

namespace tp_math_utils
{


//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT MaterialSwapParameters
{
  tp_utils::StringID name;

  glm::vec3 albedoUse;
  glm::vec3 albedoScale;
  glm::vec3 albedoBias;

  glm::vec3 sssUse;
  glm::vec3 sssScale;
  glm::vec3 sssBias;

  glm::vec3 emissionUse;
  glm::vec3 emissionScale;
  glm::vec3 emissionBias;

  glm::vec3 velvetUse;
  glm::vec3 velvetScale;
  glm::vec3 velvetBias;

  float albedoHue;

  tp_math_utils::Material materialWithSwappedParameters( const tp_math_utils::Material& material, const glm::vec3& color ) const;

  //################################################################################################
  nlohmann::json saveState() const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
