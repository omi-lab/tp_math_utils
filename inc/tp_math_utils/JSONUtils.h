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

namespace tp_utils {

template<typename T> struct type_is_glm_vector_container { static const bool value = false; };
template<> struct type_is_glm_vector_container<glm::vec2>     { static const bool value = true;  };
template<> struct type_is_glm_vector_container<glm::vec3>     { static const bool value = true;  };
template<> struct type_is_glm_vector_container<glm::vec4>     { static const bool value = true;  };
template<> struct type_is_glm_vector_container<glm::ivec2>    { static const bool value = true;  };
template<> struct type_is_glm_vector_container<glm::ivec3>    { static const bool value = true;  };
template<> struct type_is_glm_vector_container<glm::ivec4>    { static const bool value = true;  };
template<> struct type_is_glm_vector_container<glm::quat>     { static const bool value = true;  };

template<typename T>
struct saveValueToJSON<T, typename std::enable_if<type_is_glm_vector_container<T>::value>::type>
{
  T const& d;
  saveValueToJSON(T const& d_):d(d_){}

  T const& saveState(nlohmann::json& j)
  {
    try
    {
      nlohmann::json arr=nlohmann::json::array();
      arr.get_ptr<nlohmann::json::array_t*>()->reserve(d.length());
      for(int i = 0; i < d.length(); ++i){
        nlohmann::json object;
        saveValueToJSON<typename T::value_type>(d[i]).saveState(object);
        arr.emplace_back(object);
      }
      j = arr;
    }
    catch(...)
    {
    }
    return d;
  }
};


template<typename T>
struct loadValueFromJSON<T, typename std::enable_if<type_is_glm_vector_container<T>::value>::type>
{
  T& d;
  loadValueFromJSON(T& d_):d(d_){}

  T& loadState(const nlohmann::json& j)
  {
    try
    {
      int index = 0;
      for(auto i = j.begin(); i != j.end() && index < d.length(); ++i, ++index){
        loadValueFromJSON<typename T::value_type>(d[index]).loadState(i.value());
      }
    }
    catch(...)
    {
    }
    return d;
  }
};

//! serialization of non square matrix as list of arrays. Please, note that squared matrix
//! serialized as pure array for backward compatiblity (see code below)
template<typename T> struct type_is_glm_matrix_container { static const bool value = false; };
//template<> struct type_is_glm_matrix_container<glm::mat2x2>     { static const bool value = true;  };
template<> struct type_is_glm_matrix_container<glm::mat2x3>     { static const bool value = true;  };
template<> struct type_is_glm_matrix_container<glm::mat2x4>     { static const bool value = true;  };
template<> struct type_is_glm_matrix_container<glm::mat3x2>     { static const bool value = true;  };
//template<> struct type_is_glm_matrix_container<glm::mat3x3>     { static const bool value = true;  };
template<> struct type_is_glm_matrix_container<glm::mat3x4>     { static const bool value = true;  };
template<> struct type_is_glm_matrix_container<glm::mat4x2>     { static const bool value = true;  };
template<> struct type_is_glm_matrix_container<glm::mat4x3>     { static const bool value = true;  };
//template<> struct type_is_glm_matrix_container<glm::mat4x4>     { static const bool value = true;  };

template<typename T>
struct saveValueToJSON<T, typename std::enable_if<type_is_glm_matrix_container<T>::value>::type>
{
  T const& d;
  saveValueToJSON(T const& d_):d(d_){}

  T const& saveState(nlohmann::json& j)
  {
    try
    {
      nlohmann::json arr=nlohmann::json::array();
      arr.get_ptr<nlohmann::json::array_t*>()->reserve(d.length());
      for(int i = 0; i < d.length(); ++i){
        nlohmann::json object;
        saveValueToJSON<typename T::col_type>(d[i]).saveState(object);
        arr.emplace_back(object);
      }
      j = arr;
    }
    catch(...)
    {
    }
    return d;
  }
};


template<typename T>
struct loadValueFromJSON<T, typename std::enable_if<type_is_glm_matrix_container<T>::value>::type>
{
  T& d;
  loadValueFromJSON(T& d_):d(d_){}

  T& loadState(const nlohmann::json& j)
  {
    try
    {
      int index = 0;
      for(auto i = j.begin(); i != j.end() && index < d.length(); ++i, ++index){
        loadValueFromJSON<typename T::col_type>(d[index]).loadState(i.value());
      }
    }
    catch(...)
    {
    }
    return d;
  }
};

//! Serialization of squared matrix can be done same way as non squared (list of vectors). However for
//! backward compatibilty it is done as flat array
template<typename T> struct type_is_glm_matrix_as_vector_container { static const bool value = false; };
template<> struct type_is_glm_matrix_as_vector_container<glm::mat2x2>     { static const bool value = true;  };
//template<> struct type_is_glm_matrix_container<glm::mat2x3>     { static const bool value = true;  };
//template<> struct type_is_glm_matrix_container<glm::mat2x4>     { static const bool value = true;  };
//template<> struct type_is_glm_matrix_container<glm::mat3x2>     { static const bool value = true;  };
template<> struct type_is_glm_matrix_as_vector_container<glm::mat3x3>     { static const bool value = true;  };
//template<> struct type_is_glm_matrix_container<glm::mat3x4>     { static const bool value = true;  };
//template<> struct type_is_glm_matrix_container<glm::mat4x2>     { static const bool value = true;  };
//template<> struct type_is_glm_matrix_container<glm::mat4x3>     { static const bool value = true;  };
template<> struct type_is_glm_matrix_as_vector_container<glm::mat4x4>     { static const bool value = true;  };
template<typename T>
struct saveValueToJSON<T, typename std::enable_if<type_is_glm_matrix_as_vector_container<T>::value>::type>
{
  T const& d;
  saveValueToJSON(T const& d_):d(d_){}

  T const& saveState(nlohmann::json& j)
  {
    try
    {
      nlohmann::json arr=nlohmann::json::array();
      arr.get_ptr<nlohmann::json::array_t*>()->reserve(d.length());
      auto v = glm::value_ptr(d);
      auto length = d.length()*d[0].length();
      for(int i = 0; i < length; ++i){
        nlohmann::json object;
        saveValueToJSON<typename T::value_type>(v[i]).saveState(object);
        arr.emplace_back(object);
      }
      j = arr;
    }
    catch(...)
    {
    }
    return d;
  }
};


template<typename T>
struct loadValueFromJSON<T, typename std::enable_if<type_is_glm_matrix_as_vector_container<T>::value>::type>
{
  T& d;
  loadValueFromJSON(T& d_):d(d_){}

  T& loadState(const nlohmann::json& j)
  {
    try
    {
      int index = 0;
      auto length = d.length()*d[0].length();
      auto v = glm::value_ptr(d);
      for(auto i = j.begin(); i != j.end() && index < length; ++i, ++index){
        loadValueFromJSON<typename T::value_type>(v[index]).loadState(i.value());
      }
    }
    catch(...)
    {
    }
    return d;
  }
};

}


#endif
