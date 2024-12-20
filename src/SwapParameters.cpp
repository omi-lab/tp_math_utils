#include "tp_math_utils/SwapParameters.h"
#include "tp_math_utils/JSONUtils.h"

namespace tp_math_utils
{

namespace
{
//##################################################################################################
void swapValue(float& v, float newValue, float use, float scale, float bias)
{
  v = (v*(1.0f-use)) + (( (newValue*scale) + bias) * use);
}
}

//##################################################################################################
glm::vec3 Vec3SwapParameters::apply(const glm::vec3& originalValue, const glm::vec3& selectedValue) const
{
  auto v = originalValue;
  swapValue(v.x, selectedValue.x, use.x, scale.x, bias.x);
  swapValue(v.y, selectedValue.y, use.y, scale.y, bias.y);
  swapValue(v.z, selectedValue.z, use.z, scale.z, bias.z);
  return v;
}

//##################################################################################################
glm::vec3 Vec3SwapParameters::apply(const glm::vec3& originalValue, float selectedValue) const
{
  return apply(originalValue, glm::vec3(selectedValue, selectedValue, selectedValue));
}

//##################################################################################################
glm::vec3 Vec3SwapParameters::applyClamped(const glm::vec3& originalValue, const glm::vec3& selectedValue) const
{
  return glm::clamp(apply(originalValue, selectedValue), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
}

//##################################################################################################
void Vec3SwapParameters::saveState(nlohmann::json& j) const
{
  j["use"  ] = tp_math_utils::vec3ToJSON(use  );
  j["scale"] = tp_math_utils::vec3ToJSON(scale);
  j["bias" ] = tp_math_utils::vec3ToJSON(bias );
}

//##################################################################################################
void Vec3SwapParameters::loadState(const nlohmann::json& j)
{
  use   = TPJSONVec3(j, "use"  , {0.0f, 0.0f, 0.0f});
  scale = TPJSONVec3(j, "scale", {1.0f, 1.0f, 1.0f});
  bias  = TPJSONVec3(j, "bias" , {0.0f, 0.0f, 0.0f});
}

//##################################################################################################
void Vec3SwapParameters::loadState(const nlohmann::json& j, const std::string& prefix)
{
  if(auto i=j.find(prefix); i!=j.end())
    loadState(*i);
  else
  {
    use   = TPJSONVec3(j, prefix+"Use"  , {0.0f, 0.0f, 0.0f});
    scale = TPJSONVec3(j, prefix+"Scale", {1.0f, 1.0f, 1.0f});
    bias  = TPJSONVec3(j, prefix+"Bias" , {0.0f, 0.0f, 0.0f});
  }
}

//##################################################################################################
float FloatSwapParameters::apply(float originalValue, float selectedValue) const
{
  auto v = originalValue;
  swapValue(v, selectedValue, use, scale, bias);
  return v;
}

//##################################################################################################
float FloatSwapParameters::applyClamped(float originalValue, float selectedValue) const
{
  auto v = originalValue;
  swapValue(v, selectedValue, use, scale, bias);
  return std::clamp(v, 0.0f, 1.0f);
}

//##################################################################################################
void FloatSwapParameters::saveState(nlohmann::json& j) const
{
  j["use"  ] = use  ;
  j["scale"] = scale;
  j["bias" ] = bias ;
}

//##################################################################################################
void FloatSwapParameters::loadState(const nlohmann::json& j)
{
  use   = TPJSONFloat(j, "use"  , 0.0f);
  scale = TPJSONFloat(j, "scale", 1.0f);
  bias  = TPJSONFloat(j, "bias" , 0.0f);
}

//##################################################################################################
void FloatSwapParameters::loadState(const nlohmann::json& j, const std::string& prefix)
{
  if(auto i=j.find(prefix); i!=j.end())
    loadState(*i);
  else
  {
    use   = TPJSONFloat(j, prefix+"Use"  , 0.0f);
    scale = TPJSONFloat(j, prefix+"Scale", 1.0f);
    bias  = TPJSONFloat(j, prefix+"Bias" , 0.0f);
  }
}

}
