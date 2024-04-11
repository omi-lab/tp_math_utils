#include "tp_math_utils/materials/ExternalMaterialMetadata.h"

#include "tp_utils/FileUtils.h"
#include "tp_utils/JSONUtils.h"
#include "tp_utils/DebugUtils.h"

#include <sstream>

namespace tp_math_utils
{
//##################################################################################################
void ExternalMaterialVariable::loadState(const nlohmann::json& j)
{
  _name = TPJSONString(j, "name");
  std::string type = TPJSONString(j, "type");
  _data.reset(new ExternalMaterialVariable::Data());

  if(j.find("data") == j.end())
    return;

  _data->type() = type;
  if(type == ExternalMaterialVariable::Value::TypeStr)
  {
    auto newValue = new ExternalMaterialVariable::Value();
    newValue->load(j["data"]);
    _data.reset(newValue);
  }
  else if(type == ExternalMaterialVariable::RGB::TypeStr)
  {
    auto newValue = new ExternalMaterialVariable::RGB();
    newValue->load(j["data"]);
    _data.reset(newValue);
  }
  else
  {
    tpWarning()<<"Warn:: ExternalMaterialVariable::loadState:: Unrecognized type";
    _data->type() = ExternalMaterialVariable::Data::TypeStr;
    return;
  }
}

//##################################################################################################
void ExternalMaterialVariable::saveState(nlohmann::json& j) const
{
  j["name"]  = _name;
  j["type"]  = _data->type();
  j["data"]  = nlohmann::json();
  _data->save(j["data"]);
}

//##################################################################################################
 void ExternalMaterialVariable::Value::load(const nlohmann::json& jData)
 {
  value = 0.0f;
  if(jData.is_number())
    value = jData.get<float>();
 }

//##################################################################################################
void ExternalMaterialVariable::Value::save(nlohmann::json& j) const
{
  j = value;
}

//##################################################################################################
 void ExternalMaterialVariable::RGB::load(const nlohmann::json& jData)
 {
  r = 0.0f;
  g = 0.0f;
  b = 0.0f;
  a = 0.0f;
  
  if(jData.is_array() && jData.size() == 4)
  {
    r = jData[0].get<float>();
    g = jData[1].get<float>();
    b = jData[2].get<float>();
    a = jData[3].get<float>();
  }
 }

//##################################################################################################
void ExternalMaterialVariable::RGB::save(nlohmann::json& j) const
{
  j = nlohmann::json::array();
  j.get_ptr<nlohmann::json::array_t*>()->reserve(4);
  j.push_back(r);
  j.push_back(g);
  j.push_back(b);
  j.push_back(a);
}

//##################################################################################################
void ExternalMaterialMetadata::loadState(const nlohmann::json& j)
{
  name = TPJSONString(j, "name");

  if(j.find("vars") == j.end())
    return;

  auto& jvars = j["vars"];

  if(!jvars.is_array())
    return;

  variables.clear();
  for(auto& jvar : jvars)
  {
    ExternalMaterialVariable& variable = variables.emplace_back();
    variable.loadState(jvar);
  }
}

//##################################################################################################
void ExternalMaterialMetadata::saveState(nlohmann::json& j) const
{
  j["name"]  = name;
  j["vars"] = nlohmann::json::array();
  j["vars"].get_ptr<nlohmann::json::array_t*>()->reserve(variables.size());
  for(auto& var : variables)
  {
    j["vars"].push_back(nlohmann::json::object());
    var.saveState(j["vars"].back());
  }
}

//##################################################################################################
std::vector<ExternalMaterialMetadata> LoadMaterialMetadataFromFile(const std::string metadataFilePath)
{
  if(!tp_utils::exists(metadataFilePath))
    return {};

  return LoadMaterialMetadataFromJson(tp_utils::readJSONFile(metadataFilePath));
}

//##################################################################################################
std::vector<ExternalMaterialMetadata> LoadMaterialMetadataFromTextJson(const std::string metadataDataJson)
{
  nlohmann::json metadataFileJ;
  try
  {
    return LoadMaterialMetadataFromJson(nlohmann::json::parse(metadataDataJson));
  }
  catch(...)
  {
    return {};
  }
}

//##################################################################################################
std::vector<ExternalMaterialMetadata> LoadMaterialMetadataFromJson(const nlohmann::json& metadataDataJ)
{
  if(!metadataDataJ.is_array())
    return {};

  std::vector<ExternalMaterialMetadata> metadataVector;
  metadataVector.reserve(metadataDataJ.size());
  for(auto& materialMetadataJ : metadataDataJ)
  {
    auto& metadata = metadataVector.emplace_back();
    metadata.loadState(materialMetadataJ);
  }

  return metadataVector;
}

}
