#ifndef tp_math_utils_MaterialSwapParameters_h
#define tp_math_utils_MaterialSwapParameters_h

#include "tp_math_utils/Globals.h"
#include "tp_math_utils/Material.h"
#include "tp_math_utils/SwapParameters.h"

#include "json.hpp"

#include <unordered_set>

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_EXPORT MaterialSwapParameters
{
  tp_utils::StringID name;

  Vec3SwapParameters albedo;
  Vec3SwapParameters sss;
  Vec3SwapParameters emission;
  Vec3SwapParameters velvet;

  bool useAlbedoHue{false};

  glm::vec3 initialColor{0.1f, 0.7f, 0.2f};

  FloatSwapParameters albedoSaturation;
  FloatSwapParameters albedoValue;

  //################################################################################################
  tp_math_utils::Material materialWithSwappedParameters(const tp_math_utils::Material& material,
                                                        const glm::vec3& color) const;

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
