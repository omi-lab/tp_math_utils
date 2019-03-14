#ifndef tp_math_utils_JSONUtils_h
#define tp_math_utils_JSONUtils_h

#include "tp_math_utils/Globals.h"

#include "tp_utils/JSONUtils.h"

#include "json.hpp"

#include "glm/gtc/type_ptr.hpp"

namespace tp_math_utils
{

//##################################################################################################
inline nlohmann::json ivec2ToJSON(const glm::ivec2& vec)
{
  return nlohmann::json::array({vec.x, vec.y,});
}

//##################################################################################################
inline glm::ivec2 getJSONIvec2(const nlohmann::json& j, const std::string& key, const glm::ivec2& defaultValue)
{
  std::vector<nlohmann::json> c = tp_utils::getJSONArray(j, key);
  if(c.size() == 2)
    return glm::ivec2(c.at(0), c.at(1));

  return defaultValue;
}

//##################################################################################################
inline nlohmann::json vec2ToJSON(const glm::vec2& vec)
{
  return nlohmann::json::array({vec.x, vec.y,});
}

//##################################################################################################
inline glm::vec2 getJSONVec2(const nlohmann::json& j, const std::string& key, const glm::vec2& defaultValue)
{
  std::vector<nlohmann::json> c = tp_utils::getJSONArray(j, key);
  if(c.size() == 2)
    return glm::vec2(c.at(0), c.at(1));

  return defaultValue;
}

//##################################################################################################
inline nlohmann::json vec3ToJSON(const glm::vec3& vec)
{
  return nlohmann::json::array({vec.x, vec.y, vec.z});
}

//##################################################################################################
inline glm::vec3 vec3FromJSON(const nlohmann::json& j)
{
  return (j.size() == 3)?glm::vec3(j.at(0), j.at(1), j.at(2)):glm::vec3(0,0,0);
}

//##################################################################################################
inline glm::vec3 getJSONVec3(const nlohmann::json& j, const std::string& key, const glm::vec3& defaultValue)
{
  std::vector<nlohmann::json> c = tp_utils::getJSONArray(j, key);
  if(c.size() == 3)
    return glm::vec3(c.at(0), c.at(1), c.at(2));

  return defaultValue;
}

//##################################################################################################
inline nlohmann::json vec4ToJSON(const glm::vec4& vec)
{
  return nlohmann::json::array({vec.x, vec.y, vec.z, vec.w});
}

//##################################################################################################
inline glm::vec4 getJSONVec4(const nlohmann::json& j, const std::string& key, const glm::vec4& defaultValue)
{
  std::vector<nlohmann::json> c = tp_utils::getJSONArray(j, key);
  if(c.size() == 4)
    return glm::vec4(c.at(0), c.at(1), c.at(2), c.at(3));

  return defaultValue;
}

//##################################################################################################
inline nlohmann::json mat4ToJSON(const glm::mat4& mat)
{
  nlohmann::json j = nlohmann::json::array();
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
inline glm::mat4 getJSONMat4(const nlohmann::json& j, const std::string& key, const glm::mat4& defaultValue)
{
  std::vector<nlohmann::json> c = tp_utils::getJSONArray(j, key);
  if(c.size() == 16)
  {
    glm::mat4 mat;
    float* v = glm::value_ptr(mat);
    for(size_t i=0; i<16; i++, v++)
      (*v) = c.at(i);
    return mat;
  }

  return defaultValue;
}


//##################################################################################################
inline nlohmann::json vec3VectorToJSON(const std::vector<glm::vec3>& coords)
{
  nlohmann::json j = nlohmann::json::array();
  for(const auto& coord : coords)
    j.push_back(vec3ToJSON(coord));
  return j;
}

//##################################################################################################
inline const std::vector<glm::vec3> vec3VectorFromJSON(const nlohmann::json& j)
{
  std::vector<glm::vec3> coords;
  if(j.is_array())
    for(const auto& jj : j)
      coords.push_back(vec3FromJSON(jj));
  return coords;
}











}

#endif
