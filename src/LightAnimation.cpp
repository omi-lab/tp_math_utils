#include "tp_math_utils/LightAnimation.h"
#include "tp_math_utils/JSONUtils.h"

namespace {

template<typename T>
T valueFromJSON(const nlohmann::json& j){
  return j.get<T>();
}

template<>
float valueFromJSON(const nlohmann::json& j){
  if (j.is_string())
    return std::stof(j.get<std::string>());
  else
    return j.get<float>();
}

template<>
glm::vec3 valueFromJSON(const nlohmann::json& j){
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

nlohmann::json valueToJSON(const float& val){
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
void updateMaxRange(std::pair<float,float> &range, std::map<K,T> map){
  if(!map.empty()){
    range.first = std::min(range.first, map.begin()->first);
    range.second = std::max(range.second, map.rbegin()->first);
  }
}

}

namespace tp_math_utils
{

void LightAnimation::updateMaxRange(std::pair<float,float>& range) // min, max value over all animation data
{  
  ::updateMaxRange(range, location);
  ::updateMaxRange(range, rotation_euler);
  ::updateMaxRange(range, scale);
  ::updateMaxRange(range, spot_size);
  ::updateMaxRange(range, energy);
  ::updateMaxRange(range, color);
  ::updateMaxRange(range, shadow_soft_size);
  ::updateMaxRange(range, spot_blend);
}



void LightAnimation::saveState(nlohmann::json& j) const
{
  j = nlohmann::json::object();
  saveObjectToJSON(j, "location"        , location);
  saveObjectToJSON(j, "rotation_euler"  , rotation_euler);
  saveObjectToJSON(j, "scale"           , scale);
  saveObjectToJSON(j, "spot_size"       , spot_size);
  saveObjectToJSON(j, "energy"          , energy);
  saveObjectToJSON(j, "color"           , color);
  saveObjectToJSON(j, "shadow_soft_size", shadow_soft_size);
  saveObjectToJSON(j, "spot_blend"      , spot_blend);
}

void LightAnimation::loadState(const nlohmann::json& j)
{

  loadObjectFromJSON(j, "location"        , location);
  loadObjectFromJSON(j, "rotation_euler"  , rotation_euler);
  loadObjectFromJSON(j, "scale"           , scale);
  loadObjectFromJSON(j, "spot_size"       , spot_size);
  loadObjectFromJSON(j, "energy"          , energy);
  loadObjectFromJSON(j, "color"           , color);
  loadObjectFromJSON(j, "shadow_soft_size", shadow_soft_size);
  loadObjectFromJSON(j, "spot_blend"      , spot_blend);
}

}
