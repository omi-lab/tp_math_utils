#ifndef LIGHTANIMATION_H
#define LIGHTANIMATION_H

#include "tp_math_utils/Globals.h"

#include <glm/glm.hpp>

#include <map>

#include <json.hpp>

namespace tp_math_utils
{

struct TP_MATH_UTILS_EXPORT LightAnimation
{
  std::map<float, glm::vec3> timeToLocation;
  std::map<float, glm::vec3> timeToRotation;
  std::map<float, glm::vec3> timeToScale;
  std::map<float, float> timeToSpotSize;
  std::map<float, float> timeToEnergy;

  void saveState(nlohmann::json& j) const;
  void loadState(const nlohmann::json& j);
};

}

#endif // LIGHTANIMATION_H
