#ifndef tp_math_utils_JSONUtils_h
#define tp_math_utils_JSONUtils_h

#include "tp_math_utils/Globals.h" // IWYU pragma: keep

#include "tp_utils/JSONUtils.h"

#include "json.hpp"

#include "glm/gtc/type_ptr.hpp"

namespace tp_math_utils
{
#define TPJSONVec2  tp_math_utils::getJSONVec2
#define TPJSONVec3  tp_math_utils::getJSONVec3
#define TPJSONVec4  tp_math_utils::getJSONVec4
#define TPJSONIVec2 tp_math_utils::getJSONIvec2
#define TPJSONIVec3 tp_math_utils::getJSONIvec3
#define TPJSONMat4  tp_math_utils::getJSONMat4

//-- ivec2 -----------------------------------------------------------------------------------------

//##################################################################################################
inline nlohmann::json ivec2ToJSON(const glm::ivec2& vec)
{
  return nlohmann::json::array({vec.x, vec.y});
}

//##################################################################################################
inline glm::ivec2 ivec2FromJSON(const nlohmann::json& j, const glm::ivec2& defaultValue=glm::ivec2())
{
  return (j.is_array() && j.size()==2 && j.at(0).is_number() && j.at(1).is_number())?glm::ivec2(j.at(0), j.at(1)):defaultValue;
}

//##################################################################################################
inline glm::ivec2 getJSONIvec2(const nlohmann::json& j, const std::string& key, const glm::ivec2& defaultValue)
{
  return ivec2FromJSON(TPJSON(j, key), defaultValue);
}

//-- vec2 ------------------------------------------------------------------------------------------

//##################################################################################################
inline nlohmann::json vec2ToJSON(const glm::vec2& vec)
{
  return nlohmann::json::array({vec.x, vec.y});
}

//##################################################################################################
inline glm::vec2 vec2FromJSON(const nlohmann::json& j, const glm::vec2& defaultValue=glm::vec2())
{
  return (j.is_array() && j.size()==2 && j.at(0).is_number() && j.at(1).is_number())?glm::vec2(j.at(0), j.at(1)):defaultValue;
}

//##################################################################################################
inline glm::vec2 getJSONVec2(const nlohmann::json& j, const std::string& key, const glm::vec2& defaultValue)
{
  return vec2FromJSON(TPJSON(j, key), defaultValue);
}

//##################################################################################################
inline nlohmann::json vec2VectorToJSON(const std::vector<glm::vec2>& coords)
{
  nlohmann::json j = nlohmann::json::array();
  j.get_ptr<nlohmann::json::array_t*>()->reserve(coords.size());
  for(const auto& coord : coords)
    j.push_back(vec2ToJSON(coord));
  return j;
}

//##################################################################################################
inline const std::vector<glm::vec2> vec2VectorFromJSON(const nlohmann::json& j)
{
  std::vector<glm::vec2> coords;
  if(j.is_array())
  {
    coords.reserve(j.size());
    for(const auto& jj : j)
      coords.push_back(vec2FromJSON(jj));
  }
  return coords;
}

//##################################################################################################
inline const std::vector<glm::vec2> vec2VectorFromJSON(const nlohmann::json& j, const std::string& key)
{
  if(auto i = j.find(key); i != j.end())
    return vec2VectorFromJSON(i.value());

  return {};
}

//-- quat ------------------------------------------------------------------------------------------

//##################################################################################################
inline nlohmann::json quatToJSON(const glm::quat& quat)
{
  return nlohmann::json::array({quat.w, quat.x, quat.y, quat.z});
}

//##################################################################################################
inline glm::quat quatFromJSON(const nlohmann::json& j, const glm::quat& defaultValue=glm::quat())
{
  return (j.is_array() && j.size()==4 && j.at(0).is_number() && j.at(1).is_number()
          && j.at(2).is_number() && j.at(3).is_number())?glm::quat(j.at(0), j.at(1), j.at(2), j.at(3)):defaultValue;
}

//##################################################################################################
inline glm::quat getJSONQuat(const nlohmann::json& j, const std::string& key, const glm::quat& defaultValue)
{
  return quatFromJSON(TPJSON(j, key), defaultValue);
}

//-- ivec3 -----------------------------------------------------------------------------------------

//##################################################################################################
inline nlohmann::json ivec3ToJSON(const glm::ivec3& vec)
{
  return nlohmann::json::array({vec.x, vec.y, vec.z});
}

//##################################################################################################
inline glm::ivec3 ivec3FromJSON(const nlohmann::json& j, const glm::ivec3& defaultValue=glm::ivec3())
{
  return (j.is_array() && j.size()==3 && j.at(0).is_number() && j.at(1).is_number() && j.at(2).is_number())?glm::ivec3(j.at(0), j.at(1), j.at(2)):defaultValue;
}

//##################################################################################################
inline glm::ivec3 getJSONIvec3(const nlohmann::json& j, const std::string& key, const glm::ivec3& defaultValue)
{
  return ivec3FromJSON(TPJSON(j, key), defaultValue);
}

//-- vec3 ------------------------------------------------------------------------------------------

//##################################################################################################
inline nlohmann::json vec3ToJSON(const glm::vec3& vec)
{
  return nlohmann::json::array({vec.x, vec.y, vec.z});
}

//##################################################################################################
inline glm::vec3 vec3FromJSON(const nlohmann::json& j, const glm::vec3& defaultValue=glm::vec3())
{
  return (j.is_array() && j.size()==3 && j.at(0).is_number() && j.at(1).is_number() && j.at(2).is_number())?glm::vec3(j.at(0), j.at(1), j.at(2)):defaultValue;
}

//##################################################################################################
inline glm::vec3 vec3FromJSON(const nlohmann::json& j, const std::string& key, const glm::vec3& defaultValue=glm::vec3())
{
  if(auto i = j.find(key); i != j.end())
    return vec3FromJSON(i.value());

  return defaultValue;
}

//##################################################################################################
inline glm::vec3 getJSONVec3(const nlohmann::json& j, const std::string& key, const glm::vec3& defaultValue)
{
  return vec3FromJSON(TPJSON(j, key), defaultValue);
}

//##################################################################################################
inline nlohmann::json vec3VectorToJSON(const std::vector<glm::vec3>& coords)
{
  nlohmann::json j = nlohmann::json::array();
  j.get_ptr<nlohmann::json::array_t*>()->reserve(coords.size());
  for(const auto& coord : coords)
    j.push_back(vec3ToJSON(coord));
  return j;
}

//##################################################################################################
inline const std::vector<glm::vec3> vec3VectorFromJSON(const nlohmann::json& j)
{
  std::vector<glm::vec3> coords;
  if(j.is_array())
  {
    coords.reserve(j.size());
    for(const auto& jj : j)
      coords.push_back(vec3FromJSON(jj));
  }
  return coords;
}

//##################################################################################################
inline const std::vector<glm::vec3> vec3VectorFromJSON(const nlohmann::json& j, const std::string& key)
{
  if(auto i = j.find(key); i != j.end())
    return vec3VectorFromJSON(i.value());

  return {};
}

//-- vec4 ------------------------------------------------------------------------------------------

//##################################################################################################
inline nlohmann::json vec4ToJSON(const glm::vec4& vec)
{
  return nlohmann::json::array({vec.x, vec.y, vec.z, vec.w});
}

//##################################################################################################
inline glm::vec4 vec4FromJSON(const nlohmann::json& j, const glm::vec4& defaultValue=glm::vec4())
{
  return (j.is_array() && j.size()==4 && j.at(0).is_number() && j.at(1).is_number() && j.at(2).is_number())?glm::vec4(j.at(0), j.at(1), j.at(2), j.at(3)):defaultValue;
}

//##################################################################################################
inline glm::vec4 getJSONVec4(const nlohmann::json& j, const std::string& key, const glm::vec4& defaultValue)
{
  return vec4FromJSON(TPJSON(j, key), defaultValue);
}

//##################################################################################################
inline nlohmann::json vec4VectorToJSON(const std::vector<glm::vec4>& coords)
{
  nlohmann::json j = nlohmann::json::array();
  j.get_ptr<nlohmann::json::array_t*>()->reserve(coords.size());
  for(const auto& coord : coords)
    j.push_back(vec4ToJSON(coord));
  return j;
}

//##################################################################################################
inline const std::vector<glm::vec4> vec4VectorFromJSON(const nlohmann::json& j)
{
  std::vector<glm::vec4> coords;
  if(j.is_array())
  {
    coords.reserve(j.size());
    for(const auto& jj : j)
      coords.push_back(vec4FromJSON(jj));
  }
  return coords;
}

//-- mat3 ------------------------------------------------------------------------------------------

//##################################################################################################
inline nlohmann::json mat3ToJSON(const glm::mat3& mat)
{
  nlohmann::json j = nlohmann::json::array();
  j.get_ptr<nlohmann::json::array_t*>()->reserve(9);
  const float* v = glm::value_ptr(mat);
  const float* vMax = v+9;
  for(; v<vMax; v++)
    j.push_back(*v);
  return j;
}

//##################################################################################################
inline glm::mat3 mat3FromJSON(const nlohmann::json& j)
{
  glm::mat3 mat{1};
  if(j.is_array() && j.size() == 9)
  {
    float* v = glm::value_ptr(mat);
    for(size_t i=0; i<9; i++, v++)
      if(const auto& jj = j.at(i); jj.is_number())
        (*v) = jj;
  }
  return mat;
}

//-- mat4 ------------------------------------------------------------------------------------------

//##################################################################################################
inline nlohmann::json mat4ToJSON(const glm::mat4& mat)
{
  nlohmann::json j = nlohmann::json::array();
  j.get_ptr<nlohmann::json::array_t*>()->reserve(16);
  const float* v = glm::value_ptr(mat);
  const float* vMax = v+16;
  for(; v<vMax; v++)
    j.push_back(*v);
  return j;
}

//##################################################################################################
inline glm::mat4 mat4FromJSON(const nlohmann::json& j)
{
  glm::mat4 mat{1};
  if(j.is_array() && j.size() == 16)
  {
    float* v = glm::value_ptr(mat);
    for(size_t i=0; i<16; i++, v++)
      if(const auto& jj = j.at(i); jj.is_number())
        (*v) = jj;
  }
  return mat;
}

//##################################################################################################
inline const glm::mat4 mat4FromJSON(const nlohmann::json& j, const std::string& key)
{
  if(auto i = j.find(key); i != j.end())
    return mat4FromJSON(i.value());

  return {};
}

//##################################################################################################
inline glm::mat4 getJSONMat4(const nlohmann::json& j, const std::string& key, const glm::mat4& defaultValue)
{
  if(const auto i=j.find(key); i!=j.end() && i->is_array() && i->size()==16)
  {
    glm::mat4 mat;
    float* v = glm::value_ptr(mat);
    for(auto it=i->begin(); it!=i->end(); ++it, v++)
      if(it->is_number())
        (*v) = float(*it);
    return mat;
  }

  return defaultValue;
}

}

#endif
