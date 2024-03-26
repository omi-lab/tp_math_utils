#include "tp_math_utils/materials/ExternalMaterial.h"

#include "tp_utils/JSONUtils.h"

namespace tp_math_utils
{

//##################################################################################################
void ExternalMaterial::saveState(nlohmann::json& j) const
{
  j["assetType"] = assetType.toString();
  j["subPath"]   = subPath.toString();
}

//##################################################################################################
void ExternalMaterial::loadState(const nlohmann::json& j)
{
  assetType = TPJSONString(j, "assetType");
  subPath = TPJSONString(j, "subPath");
}

//################################################################################################
void ExternalMaterial::appendBlendFileIDs(std::unordered_set<tp_utils::StringID>& blendFileIDs) const
{
  if(assetType.toString() == "blend")
  {
    blendFileIDs.insert(subPath);
  }
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
