#include "tp_math_utils/Light.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/JSONUtils.h"

namespace tp_math_utils
{

//##################################################################################################
std::vector<std::string> lightTypes()
{
  return {"Directional", "Global", "Spot"};
}

//##################################################################################################
LightType lightTypeFromString(const std::string& lightType)
{
  if(lightType == "Directional")
    return LightType::Directional;

  if(lightType == "Global")
    return LightType::Global;

  if(lightType == "Spot")
    return LightType::Spot;

  return LightType::Directional;
}

//##################################################################################################
std::string lightTypeToString(LightType lightType)
{
  switch(lightType)
  {
  case LightType::Directional: return "Directional";
  case LightType::Global:      return "Global";
  case LightType::Spot:        return "Spot";
  }
  return "Directional";
}

//##################################################################################################
void Light::setPosition(const glm::vec3& position)
{
  auto m = glm::inverse(viewMatrix);
  m[3] = glm::vec4(position, 1.0f);
  viewMatrix = glm::inverse(m);
}

//##################################################################################################
glm::vec3 Light::position() const
{
  return glm::inverse(viewMatrix)[3];
}

//##################################################################################################
void Light::setDirection(const glm::vec3& direction)
{
  viewMatrix = glm::lookAt(position(), position() + direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

//##################################################################################################
glm::vec3 Light::direction() const
{
  return glm::mat3(glm::inverse(viewMatrix))*glm::vec3(0.0f, 0.0f, -1.0f);
}

//##################################################################################################
nlohmann::json Light::saveState() const
{
  nlohmann::json j;

  j["name"] = name.toString();

  j["type"] = lightTypeToString(type);

  j["viewMatrix"] = tp_math_utils::mat4ToJSON(viewMatrix);

  j["ambient"] = tp_math_utils::vec3ToJSON(ambient);
  j["diffuse"] = tp_math_utils::vec3ToJSON(diffuse);
  j["specular"] = tp_math_utils::vec3ToJSON(specular);

  j["diffuseScale"] = diffuseScale;

  j["constant"] = constant;
  j["linear"] = linear;
  j["quadratic"] = quadratic;

  j["spotLightBlend"] = spotLightBlend;

  j["near"]        = near;
  j["far"]         = far;
  j["fov"]         = fov;
  j["orthoRadius"] = orthoRadius;

  j["offsetScale"] = tp_math_utils::vec3ToJSON(offsetScale);

  j["castShadows"] = castShadows;

  return j;
}

//##################################################################################################
void Light::loadState(const nlohmann::json& j)
{
  name = TPJSONString(j, "name");

  type = lightTypeFromString(TPJSONString(j, "type"));

  viewMatrix = tp_math_utils::mat4FromJSON(TPJSON(j, "viewMatrix"));

  ambient = tp_math_utils::vec3FromJSON(TPJSON(j, "ambient"));
  diffuse = tp_math_utils::vec3FromJSON(TPJSON(j, "diffuse"));
  specular = tp_math_utils::vec3FromJSON(TPJSON(j, "specular"));

  diffuseScale = TPJSONFloat(j, "diffuseScale", diffuseScale);

  constant = TPJSONFloat(j, "constant", constant);
  linear = TPJSONFloat(j, "linear", linear);
  quadratic = TPJSONFloat(j, "quadratic", quadratic);

  spotLightBlend = TPJSONFloat(j, "spotLightBlend", spotLightBlend);

  near        = TPJSONFloat(j, "near"       , near       );
  far         = TPJSONFloat(j, "far"        , far        );
  fov         = TPJSONFloat(j, "fov"        , fov        );
  orthoRadius = TPJSONFloat(j, "orthoRadius", orthoRadius);

  offsetScale = tp_math_utils::vec3FromJSON(TPJSON(j, "offsetScale", tp_math_utils::vec3ToJSON(glm::vec3(0.1f,0.1f,0.1f))));

  castShadows = TPJSONBool(j, "castShadows", true);
}

//##################################################################################################
nlohmann::json Light::saveLights(const std::vector<Light>& lights)
{
  nlohmann::json j = nlohmann::json::array();
  j.get_ptr<nlohmann::json::array_t*>()->reserve(lights.size());
  for(const auto& light : lights)
    j.push_back(light.saveState());
  return j;
}

//##################################################################################################
std::vector<Light> Light::loadLights(const nlohmann::json& j)
{
  std::vector<Light> lights;
  if(j.is_array())
    for(const nlohmann::json& i : j)
      lights.emplace_back().loadState(i);
  return lights;
}

//##################################################################################################
const std::vector<glm::vec2>& Light::lightLevelOffsets()
{
  static const std::vector<glm::vec2> offsets =
  {
    { 0.0f,  0.0f},

    {-1.0f,  0.0f},
    { 0.0f,  1.0f},
    { 1.0f,  0.0f},
    { 0.0f, -1.0f},

    { 0.8f,  0.8f},
    { 0.8f, -0.8f},
    {-0.8f, -0.8f},
    {-0.8f,  0.8f},

    { 0.6f,  0.6f},
    { 0.6f, -0.6f},
    {-0.6f, -0.6f},
    {-0.6f,  0.6f},

    {-0.5f,  0.0f},
    { 0.0f,  0.5f},
    { 0.5f,  0.0f},
    { 0.0f, -0.5f},

    { 0.3f,  0.3f},
    { 0.3f, -0.3f},
    {-0.3f, -0.3f},
    {-0.3f,  0.3f},

    { 0.2f,  0.2f},
    { 0.2f, -0.2f},
    {-0.2f, -0.2f},
    {-0.2f,  0.2f},

    { 0.3f,  0.7f},
    { 0.7f, -0.3f},
    {-0.3f, -0.7f},
    {-0.7f,  0.3f},

    { 0.7f,  0.3f},
    { 0.3f, -0.7f},
    {-0.7f, -0.3f},
    {-0.3f,  0.7f},

    {-0.2f,  0.0f},
    { 0.0f,  0.2f},
    { 0.2f,  0.0f},
    { 0.0f, -0.2f}
  };

  return offsets;
}

}
