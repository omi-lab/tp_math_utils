#ifndef LIGHTANIMATION_H
#define LIGHTANIMATION_H

#include "tp_math_utils/Globals.h"

#include <glm/glm.hpp>

#include <map>

#include <json.hpp>

namespace tp_math_utils
{

//################################################################################################
struct TP_MATH_UTILS_EXPORT LightAnimation
{
  // note that all parameters has same name as set in blender python
  std::map<float, glm::vec3> location;
  std::map<float, glm::vec3> rotationEuler;
  std::map<float, glm::vec3> scale;

  std::map<float, float>     fov;
  std::map<float, float>     diffuseScale;
  std::map<float, glm::vec3> diffuse;
  std::map<float, float>     offsetScale;
  std::map<float, float>     spotLightBlend;

  //################################################################################################
  //!calculate max and min key frame over all animated parameters
  void updateMaxRange(std::pair<float, float> &range);

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);

};

}

#endif // LIGHTANIMATION_H
