#ifndef tp_math_utils_ExternalMaterial_h
#define tp_math_utils_ExternalMaterial_h

#include "tp_math_utils/Material.h"
#include "tp_math_utils/materials/ExternalMaterialMetadata.h"

#include <string>
#include <vector>

namespace tp_math_utils
{
//##################################################################################################
class TP_MATH_UTILS_EXPORT ExternalMaterial : public ExtendedMaterial
{
public:
  tp_utils::StringID assetType;
  tp_utils::StringID subPath;
  std::vector<ExternalMaterialMetadata> materialVariables;

  //################################################################################################
  void saveState(nlohmann::json& j) const override;

  //################################################################################################
  void loadState(const nlohmann::json& j) override;

  //################################################################################################
  void appendFileIDs(std::vector<std::pair<tp_utils::StringID, tp_utils::StringID>>& fileIDs) const;

  //################################################################################################
  static void view(const Material& material,
                   const tp_utils::StringID& assetType,
                   const std::function<void(const ExternalMaterial&)>& closure);
};

}

#endif
