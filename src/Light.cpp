#include "tp_math_utils/Light.h"
#include "tp_math_utils/JSONUtils.h"

#include "tp_utils/DebugUtils.h"
#include "tp_utils/JSONUtils.h"

#include "lib_platform/Format.h"

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



template<typename K, typename V>
V interpolate(const std::map<K,V>& data, int frame, int frameMax, const V& defVal)
{
  if(data.empty())
    return defVal;
   auto range = data.rbegin()->first - data.begin()->first;
   auto time = data.begin()->first + range * float(frame)/float(frameMax);
   auto right = data.upper_bound(time); // strictly greater than value
   assert(right!=data.begin()); //this should not happen - we are searching value within the [min,max] range!
   auto left = right; --left;
   if(right == data.end())
     return left->second;
   else {
     float alpha = (right->first - time)/(right->first - left->first);
     return alpha * left->second + (1.0f-alpha) * right->second;
   }
}

//##################################################################################################
void Light::applyAnimation(int frame, int frameMax)
{
  if(!animation.timeToLocation.empty()){
    auto pos = interpolate(animation.timeToLocation, frame, frameMax, {});
    auto rot = interpolate(animation.timeToRotation, frame, frameMax, {});

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
  }
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
