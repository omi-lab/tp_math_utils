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
  // note that all parameters has same name as set in blender python
  std::map<float, glm::vec3> location;
  std::map<float, glm::vec3> rotation_euler;
  std::map<float, glm::vec3> scale;
                                               //relation between blender parameters
                                               //and Light class members:

  std::map<float, float>     spot_size;        // math.radians(fov)
  std::map<float, float>     energy;           // diffuseScale * 830.0
  std::map<float, glm::vec3> color;            // diffuse
  std::map<float, float>     shadow_soft_size; // offsetScale[0]
  std::map<float, float>     spot_blend;       // spotLightBlend

  // calculate max and min key frame over all animated parameters
  void updateMaxRange(std::pair<float, float> &range);

  void saveState(nlohmann::json& j) const;
  void loadState(const nlohmann::json& j);
};

}

#endif // LIGHTANIMATION_H
