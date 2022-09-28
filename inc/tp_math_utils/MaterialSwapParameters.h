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

  glm::vec3 albedoUse     {0.0f, 0.0f, 0.0f};
  glm::vec3 albedoScale   {1.0f, 1.0f, 1.0f};
  glm::vec3 albedoBias    {0.0f, 0.0f, 0.0f};

  glm::vec3 sssUse        {0.0f, 0.0f, 0.0f};
  glm::vec3 sssScale      {1.0f, 1.0f, 1.0f};
  glm::vec3 sssBias       {0.0f, 0.0f, 0.0f};

  glm::vec3 emissionUse   {0.0f, 0.0f, 0.0f};
  glm::vec3 emissionScale {1.0f, 1.0f, 1.0f};
  glm::vec3 emissionBias  {0.0f, 0.0f, 0.0f};

  glm::vec3 velvetUse     {0.0f, 0.0f, 0.0f};
  glm::vec3 velvetScale   {1.0f, 1.0f, 1.0f};
  glm::vec3 velvetBias    {0.0f, 0.0f, 0.0f};

  float albedoHue;

  glm::vec3 initialColor{0.1f, 0.7f, 0.2f};

  //################################################################################################
  tp_math_utils::Material materialWithSwappedParameters( const tp_math_utils::Material& material, const glm::vec3& color ) const;

  //################################################################################################
  nlohmann::json saveState() const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
