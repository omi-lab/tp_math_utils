#include "tp_math_utils/LightAnimation.h"
#include "tp_math_utils/JSONUtils.h"

namespace {

template<typename T>
T valueFromJSON(const nlohmann::json& j)
{
  return j.get<T>();
}

template<>
float valueFromJSON(const nlohmann::json& j)
{
  if (j.is_string())
    return std::stof(j.get<std::string>());
  else
    return j.get<float>();
}

template<>
glm::vec3 valueFromJSON(const nlohmann::json& j)
{
  return tp_math_utils::vec3FromJSON(j);
}

template<typename K, typename T>
void loadObjectFromJSON(const nlohmann::json& j, const char* key , std::map<K, T>& map)
{
  map.clear();
  if(auto i=j.find(key); i!=j.end() && i->is_object())
  {
    for(auto p=i->begin(); p!=i->end(); ++p)
      map[valueFromJSON<K>(p.key())] = valueFromJSON<T>(p.value());
  }
}

nlohmann::json valueToJSON(const glm::vec3& vec){
  return tp_math_utils::vec3ToJSON(vec);
}

nlohmann::json valueToJSON(const float& val)
{
  return nlohmann::json(val);
}

template<typename T, typename K>
void saveObjectToJSON(nlohmann::json& j, const char* key , const std::map<K, T>& map)
{
  if(!map.empty()){
    nlohmann::json jmap;
    for(auto& i: map){
      jmap[std::to_string(i.first)] = valueToJSON(i.second);
    }
    j[key] = jmap;
  }  
}

template<typename T, typename K>
void updateMaxRange(std::pair<float,float> &range, std::map<K,T> map)
{
  if(!map.empty()){
    range.first = std::min(range.first, map.begin()->first);
    range.second = std::max(range.second, map.rbegin()->first);
  }
}

}

namespace tp_math_utils
{

//################################################################################################
void LightAnimation::updateMaxRange(std::pair<float,float>& range) // min, max value over all animation data
{  
  ::updateMaxRange(range, location);
  ::updateMaxRange(range, rotationEuler);
  ::updateMaxRange(range, scale);
  ::updateMaxRange(range, fov);
  ::updateMaxRange(range, diffuseScale);
  ::updateMaxRange(range, diffuse);
  ::updateMaxRange(range, offsetScale);
  ::updateMaxRange(range, spotLightBlend);
}



//################################################################################################
void LightAnimation::saveState(nlohmann::json& j) const
{
  j = nlohmann::json::object();
  saveObjectToJSON(j, "location"        , location);
  saveObjectToJSON(j, "rotationEuler"  , rotationEuler);
  saveObjectToJSON(j, "scale"           , scale);
  saveObjectToJSON(j, "fov"             , fov);
  saveObjectToJSON(j, "diffuseScale"    , diffuseScale);
  saveObjectToJSON(j, "diffuse"         , diffuse);
  saveObjectToJSON(j, "offsetScale"     , offsetScale);
  saveObjectToJSON(j, "spotLightBlend"  , spotLightBlend);
}

//################################################################################################
void LightAnimation::loadState(const nlohmann::json& j)
{

  // for blender export is define then we use blender naming for parameters
  if(tp_utils::getJSONBool(j, "blenderExport", false))
  {
    // load from from blender export
    //relation between blender parameters
    //and Light class members:

    // spot_size        = math.radians(fov)
    // energy           = diffuseScale * 830.0
    // color            = diffuse
    // shadow_soft_size = offsetScale[0]
    // spot_blend       = spotLightBlend

    loadObjectFromJSON(j, "location"        , location);
    loadObjectFromJSON(j, "rotation_euler"  , rotationEuler);
    loadObjectFromJSON(j, "scale"           , scale);
    loadObjectFromJSON(j, "spot_size"       , fov);
    loadObjectFromJSON(j, "energy"          , diffuseScale);
    loadObjectFromJSON(j, "color"           , diffuse);
    loadObjectFromJSON(j, "shadow_soft_size", offsetScale);
    loadObjectFromJSON(j, "spot_blend"      , spotLightBlend);

    // correcting some values into SB format
    for(auto& i: fov)
      i.second = glm::degrees(i.second);

    for(auto& i: diffuseScale)
      i.second /= 830;

  }
  else
  {
    // load from scene builder store
    loadObjectFromJSON(j, "location"        , location);
    loadObjectFromJSON(j, "rotationEuler"   , rotationEuler);
    loadObjectFromJSON(j, "scale"           , scale);
    loadObjectFromJSON(j, "fov"             , fov);
    loadObjectFromJSON(j, "diffuseScale"    , diffuseScale);
    loadObjectFromJSON(j, "diffuse"         , diffuse);
    loadObjectFromJSON(j, "offsetScale"     , offsetScale);
    loadObjectFromJSON(j, "spotLightBlend"  , spotLightBlend);
  }
}

}
