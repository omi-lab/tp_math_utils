#include "tp_math_utils/materials/ExternalMaterial.h"

#include "tp_utils/JSONUtils.h"

namespace tp_math_utils
{
//##################################################################################################
void ExternalMaterial::saveState(nlohmann::json& j) const
{
  j["assetType"] = assetType.toString();
  j["subPath"]   = subPath.toString();

  j["materialVariables"] = nlohmann::json::array();
  j["materialVariables"].get_ptr<nlohmann::json::array_t*>()->reserve(materialVariables.size());
  
  for(auto& matVar : materialVariables)
  {
    j["materialVariables"].push_back(nlohmann::json::object());
    matVar.saveState(j["materialVariables"].back());
  }
}

//##################################################################################################
void ExternalMaterial::loadState(const nlohmann::json& j)
{
  assetType = TPJSONString(j, "assetType");
  subPath = TPJSONString(j, "subPath");

  materialVariables.clear();

  if(auto i=j.find("materialVariables"); i!=j.end() && i->is_array())
  {
    materialVariables.reserve(i->size());
    for(const auto& jj : *i)
      materialVariables.emplace_back().loadState(jj);
  }
}

//################################################################################################
void ExternalMaterial::appendFileIDs(std::vector<std::pair<tp_utils::StringID, tp_utils::StringID>>& fileIDs) const
{ 
  fileIDs.push_back({assetType, subPath});
}

//##################################################################################################
void ExternalMaterial::view(const Material& material,
                            const tp_utils::StringID& assetType,
                            const std::function<void(const ExternalMaterial&)>& closure)
{
  for(const auto& extendedMaterial : material.extendedMaterials)
  {
    if(auto m = dynamic_cast<const ExternalMaterial*>(extendedMaterial); m && m->assetType == assetType)
    {
      closure(*m);
      return;
    }
  }
}

}
