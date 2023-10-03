#ifndef tp_math_utils_Light_h
#define tp_math_utils_Light_h

#include "tp_math_utils/Globals.h"

#include "tp_utils/StringID.h"

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
struct TP_MATH_UTILS_EXPORT Light
{
  tp_utils::StringID name; //!< User visible for the light.

  LightType type{LightType::Directional};

  glm::mat4 viewMatrix{1.0}; //!< World to light

  glm::vec3 ambient {0.0f, 0.0f, 0.0f};
  glm::vec3 diffuse {0.6f, 0.6f, 0.6f};
  glm::vec3 specular{1.0f, 1.0f, 1.0f};

  float diffuseScale{1.0f};              //! Multiplied with the diffuse lighting calculation.

  // Used to calculate the fall off in brightness as we get further from a point light.
  float constant{1.0f};
  float linear{0.1f};
  float quadratic{0.0f};

  float spotLightBlend{0.6f}; //!< How sharp is the edge of the light 0=sharp 1=soft

  float near{0.1f};        //!< Near plane of the light frustum.
  float far{100.0f};       //!< Far plane of the light frustum.
  float fov{50.0f};        //!< Field of view for spot lights.
  float orthoRadius{10.0f}; //!< The radius of fixed directional light ortho projection.

  glm::vec3 offsetScale{0.0f, 0.0f, 0.0f};

  bool castShadows{true};

  //################################################################################################
  void setPosition(const glm::vec3& position);

  //################################################################################################
  glm::vec3 position() const;

  //################################################################################################
  void setDirection(const glm::vec3& direction);

  //################################################################################################
  glm::vec3 direction() const;

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);

  //################################################################################################
  static const std::vector<glm::vec2>& lightLevelOffsets();
};

}

#endif
