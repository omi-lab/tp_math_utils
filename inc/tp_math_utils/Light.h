#ifndef tp_math_utils_Light_h
#define tp_math_utils_Light_h

#include "tp_math_utils/Globals.h"

#include "json.hpp"

namespace tp_math_utils
{

//##################################################################################################
enum class LightType
{
  Directional,
  Global,
  Spot
};

//##################################################################################################
std::vector<std::string> lightTypes();

//##################################################################################################
LightType lightTypeFromString(const std::string& lightType);

//##################################################################################################
std::string lightTypeToString(LightType lightType);

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT Light
{
  tp_utils::StringID name; //!< User visible for the light.

  LightType type{LightType::Directional};

  glm::mat4 viewMatrix{1.0}; //!< World to light

  glm::vec3 ambient {0.4f, 0.4f, 0.4f};
  glm::vec3 diffuse {0.6f, 0.6f, 0.6f};
  glm::vec3 specular{1.0f, 1.0f, 1.0f};

  float diffuseScale{1.0f};              //! Multiplied with the diffuse lighting calculation.

  // Used to calculate the fall off in brightness as we get further from a point light.
  float constant{1.0f};
  float linear{0.1f};
  float quadratic{0.1f};

  // Spot light textures can contain multiple shapes, this is used to select what to use. The
  // default image contains only a single shape.
  glm::vec2 spotLightUV{0.0f, 0.0f}; //!< The origin of the spot light texture.
  glm::vec2 spotLightWH{1.0f, 1.0f}; //!< The size of the spot light texture as a fraction.

  float near{0.1f};        //!< Near plane of the light frustum.
  float far{100.0f};       //!< Far plane of the light frustum.
  float fov{50.0f};        //!< Field of view for spot lights.
  float orthoRadius{10.0f}; //!< The radius of fixed directional light ortho projection.

  glm::vec3 offsetScale{0.0f, 0.1f, 0.0f};

  //################################################################################################
  void setPosition(const glm::vec3& position);

  //################################################################################################
  glm::vec3 position() const;

  //################################################################################################
  void setDirection(const glm::vec3& direction);

  //################################################################################################
  glm::vec3 direction() const;

  //################################################################################################
  nlohmann::json saveState() const;

  //################################################################################################
  void loadState(const nlohmann::json& j);

  //################################################################################################
  static nlohmann::json saveLights(const std::vector<Light>& lights);

  //################################################################################################
  static std::vector<Light> loadLights(const nlohmann::json& j);

  //################################################################################################
  static const std::vector<glm::vec3>& lightLevelOffsets();
};

}

#endif
