#ifndef tp_math_utils_ExternalMaterial_h
#define tp_math_utils_ExternalMaterial_h

#include "tp_math_utils/Material.h"

namespace tp_math_utils
{
//##################################################################################################
class TP_MATH_UTILS_EXPORT ExternalMaterial : public ExtendedMaterial
{
public:
  tp_utils::StringID assetType;
  tp_utils::StringID subPath;

  //################################################################################################
  void saveState(nlohmann::json& j) const override;

  //################################################################################################
  void loadState(const nlohmann::json& j) override;

  //################################################################################################
  void appendBlendFileIDs(std::unordered_set<tp_utils::StringID>& blendFileIDs) const override;

  //################################################################################################
  static void view(const Material& material,
                   const tp_utils::StringID& assetType,
                   const std::function<void(const ExternalMaterial&)>& closure);
};

}

#endif
