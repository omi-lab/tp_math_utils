#include "tp_math_utils/Light.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/JSONUtils.h"

#include <glm/ext.hpp>

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

template<typename V>
void updateFromKeys(std::map<float,V> const& data, std::pair<float,float> const&  totalRange,
              double lightSetF, const std::function<void(const V&)>& setValue)
{
  if(data.empty()) return;

  // map SB frame interval to blender frame interval
  auto blenderFrameNumber = totalRange.first + (totalRange.second-totalRange.first) * lightSetF;
  // clamp value to the range of this key data
  blenderFrameNumber = std::clamp<double>(blenderFrameNumber, data.begin()->first, data.rbegin()->first);
  auto right = data.upper_bound(float(blenderFrameNumber)); // strictly greater than value
  assert(right!=data.begin()); //this should not happen - we are clamping value within the [min,max] range!
  auto left = right; --left;
  if(right == data.end())
    setValue(left->second);
  else {
    auto alpha = (right->first - blenderFrameNumber)/(right->first - left->first);
    setValue(V(alpha * left->second + (1.0f-alpha) * right->second));
  }
}

//##################################################################################################
void Light::applyAnimation(double lightSetF, std::pair<float,float> const& animationRange)
{
  glm::vec3 pos = position();
  updateFromKeys<glm::vec3>(animation.location, animationRange, lightSetF, [&pos](const auto& v){
    pos = v;
  });

  glm::vec3 rot;
  glm::extractEulerAngleXYZ(viewMatrix, rot.x, rot.y, rot.z);
  rot *= -1;
  updateFromKeys<glm::vec3>(animation.rotation_euler, animationRange, lightSetF, [&rot](const auto& v){
    rot = v;
  });

  // for blender output we have (strange I expect to use here eulerXYZ but only eulerZYX working correctly
  // it seams that blender returned fixedXYZ rotation which is equvalent to eulerZYX):
  // viewMatrix = inv(T*R) where: R = eulerZYX(angleZ,angleY,angleX) = (Rz*Ry*Rx)
  // (from json fields: "euler" and "location"), Rx, Ry, Rz rotation matrices from euler angles.
  // Accepting that inv(T*R) = inv(R)*inv(T)
  // where inv(R) = inv(Rx)*inv(Ry)*inv(Rz) = eulerXYZ(-angleX,-angleY,-angleZ)
  // note that eulerXYZ(-angleX,-angleY,-angleZ) equvalent to
  // fixedZYX(-angleZ,-angleY,-angleX) = inv(Rx)*inv(Ry)*inv(Rz).
  // Finally, to avoid inverse operation we do reverse angles and position:

  auto mRot = glm::eulerAngleXYZ(-rot.x, -rot.y, -rot.z);
  glm::mat4 mPos {1.0};  mPos[3] = glm::vec4(-pos, 1.0);
  viewMatrix = mRot*mPos;

  // please note that we have relation between "blender variables names" and
  // SB Light class properties variables:

  // spot_size        = math.radians(fov)
  // energy           = diffuseScale * 830.0
  // color            = diffuse
  // shadow_soft_size = offsetScale[0,..,2]
  // spot_blend       = spotLightBlend

  updateFromKeys<float>(animation.spot_size, animationRange, lightSetF, [this](const auto& v){
    fov = glm::degrees(v);
  });

  updateFromKeys<float>(animation.energy, animationRange, lightSetF, [this](const auto& v){
    diffuseScale = v/830.0f;
  });

  updateFromKeys<glm::vec3>(animation.color, animationRange, lightSetF, [this](const auto& v){
    diffuse = v;
  });

  updateFromKeys<float>(animation.shadow_soft_size, animationRange, lightSetF, [this](const auto& v){
    offsetScale = glm::vec3{v};
  });

  updateFromKeys<float>(animation.shadow_soft_size, animationRange, lightSetF, [this](const auto& v){
    spotLightBlend = v;
  });
}

void Light::applyAnimation(std::vector<tp_math_utils::Light>& lights, double lightSetF)
{
  // find out total animation range over all lights
  std::pair<float,float> totalAnimationRange{0,0};
  for(auto& i: lights)
    i.animation.updateMaxRange(totalAnimationRange);

  //apply animation to each light
  for(auto& i: lights)
    i.applyAnimation(lightSetF, totalAnimationRange);
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
void Light::saveState(nlohmann::json& j) const
{
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

  animation.saveState(j["animation"]);
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

  tp_utils::loadObjectFromJSON(j, "animation", animation);
}

}
