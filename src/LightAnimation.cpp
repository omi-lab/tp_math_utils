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
int valueFromJSON(const nlohmann::json& j){
  if (j.is_string())
    return std::stoi(j.get<std::string>());
  else
    return j.get<int>();
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

}

namespace tp_math_utils
{

void LightAnimation::saveState(nlohmann::json& j) const
{
  j = nlohmann::json::object();
  saveObjectToJSON(j, "location",   timeToLocation);
  saveObjectToJSON(j, "rotation",   timeToRotation);
  saveObjectToJSON(j, "scale",      timeToScale);
  saveObjectToJSON(j, "energy",     timeToEnergy);
  saveObjectToJSON(j, "spot_size",  timeToSpotSize);
}

void LightAnimation::loadState(const nlohmann::json& j)
{
  loadObjectFromJSON(j, "location",  timeToLocation);
  loadObjectFromJSON(j, "rotation",  timeToRotation);
  loadObjectFromJSON(j, "scale",     timeToScale);
  loadObjectFromJSON(j, "energy",    timeToEnergy);
  loadObjectFromJSON(j, "spot_size", timeToSpotSize);
}

}
