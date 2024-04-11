#include "tp_math_utils/Material.h"
#include "tp_math_utils/JSONUtils.h"
#include "tp_math_utils/materials/OpenGLMaterial.h"
#include "tp_math_utils/materials/LegacyMaterial.h"
#include "tp_math_utils/materials/ExternalMaterial.h"

#include "tp_utils/JSONUtils.h"

#include "glm/gtx/matrix_transform_2d.hpp" // IWYU pragma: keep

namespace tp_math_utils
{

namespace
{
//##################################################################################################
glm::mat3 skew(const glm::mat3& m_, const glm::vec2& uv)
{
  glm::mat3 m{1.0f};
  m[1][0] = glm::tan(glm::radians(uv.x));
  m[0][1] = glm::tan(glm::radians(uv.y));
  return m_ * m;
}

//##################################################################################################
void cloneExtendedMaterials(const std::vector<ExtendedMaterial*>& from, std::vector<ExtendedMaterial*>& to)
{
  tpDeleteAll(to);
  to.clear();

  for(const auto& extendedMaterial : from)
  {
    if(auto m=dynamic_cast<const OpenGLMaterial*>(extendedMaterial); m)
      to.push_back(new OpenGLMaterial(*m));

    else if(auto m=dynamic_cast<const LegacyMaterial*>(extendedMaterial); m)
      to.push_back(new LegacyMaterial(*m));

    else if(auto m=dynamic_cast<const ExternalMaterial*>(extendedMaterial); m)
      to.push_back(new ExternalMaterial(*m));
  }
}
}

//##################################################################################################
ExtendedMaterial::~ExtendedMaterial()
{

}

//##################################################################################################
void ExtendedMaterial::allTextureIDs(std::unordered_set<tp_utils::StringID>& textureIDs) const
{
  TP_UNUSED(textureIDs);
}

//################################################################################################
void ExtendedMaterial::appendFileIDs(std::vector<std::pair<tp_utils::StringID, tp_utils::StringID>>& fileIDs) const
{
  TP_UNUSED(fileIDs);
}

//##################################################################################################
std::unordered_set<tp_utils::StringID> ExtendedMaterial::allTextures() const
{
  std::unordered_set<tp_utils::StringID> textureIDs;
  allTextureIDs(textureIDs);
  return textureIDs;
}

//##################################################################################################
glm::mat3 UVTransformation::uvMatrix() const
{
  glm::mat3 m{1.0f};
  m = glm::scale(m, scaleUV);
  m = skew(m, skewUV);
  m = glm::rotate(m, glm::radians(rotateUV));
  m = glm::translate(m, translateUV);
  return m;
}

//##################################################################################################
bool UVTransformation::isIdentity() const
{
  if((std::fabs(skewUV.x) + std::fabs(skewUV.y)) > 0.0001f)
    return false;

  if((std::fabs(scaleUV.x-1.0f) + std::fabs(scaleUV.y-1.0f)) > 0.0001f)
    return false;

  if((std::fabs(translateUV.x) + std::fabs(translateUV.y)) > 0.0001f)
    return false;

  if(std::fabs(rotateUV) > 0.0001f)
    return false;

  return true;
}

//##################################################################################################
void UVTransformation::saveState(nlohmann::json& j) const
{
  j["skewUV"]                = tp_math_utils::vec2ToJSON(skewUV);
  j["scaleUV"]               = tp_math_utils::vec2ToJSON(scaleUV);
  j["translateUV"]           = tp_math_utils::vec2ToJSON(translateUV);
  j["rotateUV"]              = rotateUV;
}

//##################################################################################################
void UVTransformation::loadState(const nlohmann::json& j)
{
  (*this) = UVTransformation();

  skewUV      = tp_math_utils::getJSONVec2(j,      "skewUV",      skewUV);
  scaleUV     = tp_math_utils::getJSONVec2(j,     "scaleUV",     scaleUV);
  translateUV = tp_math_utils::getJSONVec2(j, "translateUV", translateUV);
  rotateUV    = TPJSONFloat               (j,    "rotateUV",    rotateUV);
}

//##################################################################################################
Material::Material()
{

}

//##################################################################################################
Material::Material(const tp_utils::StringID& name_):
  name(name_)
{

}

//##################################################################################################
Material::Material(const Material& other):
  name(std::move(other.name)),
  uvTransformation(other.uvTransformation)
{
  cloneExtendedMaterials(other.extendedMaterials, extendedMaterials);
}

//##################################################################################################
Material::Material(Material&& other) noexcept:
  name(std::move(other.name)),
  uvTransformation(std::move(other.uvTransformation))
{
  std::swap(extendedMaterials, other.extendedMaterials);
}

//##################################################################################################
Material& Material::operator=(const Material& other)
{
  if(this != &other)
  {
    name = other.name;
    uvTransformation = other.uvTransformation;
    cloneExtendedMaterials(other.extendedMaterials, extendedMaterials);
  }

  return *this;
}

//##################################################################################################
Material& Material::operator=(Material&& other) noexcept
{
  if(this != &other)
  {
    name = std::move(other.name);
    uvTransformation = std::move(other.uvTransformation);
    std::swap(extendedMaterials, other.extendedMaterials);
  }

  return *this;
}

//##################################################################################################
Material::~Material()
{
  tpDeleteAll(extendedMaterials);
}

//##################################################################################################
OpenGLMaterial* Material::findOrAddOpenGL()
{
  for(auto material : extendedMaterials)
    if(auto m=dynamic_cast<OpenGLMaterial*>(material); m)
      return m;

  auto m = new OpenGLMaterial();
  extendedMaterials.push_back(m);
  return m;
}

//##################################################################################################
LegacyMaterial* Material::findOrAddLegacy()
{
  for(auto material : extendedMaterials)
    if(auto m=dynamic_cast<LegacyMaterial*>(material); m)
      return m;

  auto m = new LegacyMaterial();
  extendedMaterials.push_back(m);
  return m;
}

//##################################################################################################
ExternalMaterial* Material::findOrAddExternal(const tp_utils::StringID& assetType)
{
  for(auto material : extendedMaterials)
    if(auto m=dynamic_cast<ExternalMaterial*>(material); m)
      if(m->assetType == assetType)
        return m;

  auto m = new ExternalMaterial();
  m->assetType = assetType;
  extendedMaterials.push_back(m);
  return m;
}

//################################################################################################
bool Material::hasExternal(const tp_utils::StringID& assetType)
{
  for(auto material : extendedMaterials)
    if(auto m=dynamic_cast<ExternalMaterial*>(material); m)
      if(m->assetType == assetType)
        return true;
  return false;
}

//##################################################################################################
void Material::updateOpenGL(const std::function<void(OpenGLMaterial&)>& closure) const
{
  for(auto material : extendedMaterials)
    if(auto m=dynamic_cast<OpenGLMaterial*>(material); m)
      return closure(*m);
}

//##################################################################################################
void Material::updateLegacy(const std::function<void(LegacyMaterial&)>& closure) const
{
  for(auto material : extendedMaterials)
    if(auto m=dynamic_cast<LegacyMaterial*>(material); m)
      return closure(*m);
}

//##################################################################################################
void Material::updateExternal(const tp_utils::StringID& assetType,
                              const std::function<void(ExternalMaterial&)>& closure) const
{
  for(auto material : extendedMaterials)
    if(auto m=dynamic_cast<ExternalMaterial*>(material); m)
      if(m->assetType == assetType)
        return closure(*m);
}

//##################################################################################################
void Material::viewOpenGL(const std::function<void(const OpenGLMaterial&)>& closure) const
{
  OpenGLMaterial::view(*this, closure);
}

//##################################################################################################
void Material::viewLegacy(const std::function<void(const LegacyMaterial&)>& closure) const
{
  LegacyMaterial::view(*this, closure);
}

//##################################################################################################
void Material::viewExternal(const tp_utils::StringID& assetType,
                            const std::function<void(const ExternalMaterial&)>& closure) const
{
  ExternalMaterial::view(*this, assetType, closure);
}

//##################################################################################################
void Material::allTextureIDs(std::unordered_set<tp_utils::StringID>& textureIDs) const
{
  for(const auto& extendedMaterial : extendedMaterials)
    extendedMaterial->allTextureIDs(textureIDs);
}

//################################################################################################
void Material::appendFileIDs(std::vector<std::pair<tp_utils::StringID, tp_utils::StringID>>& fileIDs) const
{
  for(const auto& extendedMaterial : extendedMaterials)
   extendedMaterial->appendFileIDs(fileIDs);
}

//##################################################################################################
std::unordered_set<tp_utils::StringID> Material::allTextures() const
{
  std::unordered_set<tp_utils::StringID> textureIDs;
  allTextureIDs(textureIDs);
  return textureIDs;
}

//################################################################################################
std::vector<std::pair<tp_utils::StringID, tp_utils::StringID>> Material::allFiles() const
{
  std::vector<std::pair<tp_utils::StringID, tp_utils::StringID>> files;
  appendFileIDs(files);
  return files;
}

//##################################################################################################
void Material::saveState(nlohmann::json& j) const
{
  j["name"] = name.toString();
  j["version"] = "2.0";
  uvTransformation.saveState(j);

  auto& extendedMaterialsJ = j["extendedMaterials"];
  extendedMaterialsJ = nlohmann::json::array();
  extendedMaterialsJ.get_ptr<nlohmann::json::array_t*>()->reserve(extendedMaterials.size());
  for(auto extendedMaterial : extendedMaterials)
  {
    extendedMaterialsJ.emplace_back();
    auto& extendedMaterialJ = extendedMaterialsJ.back();

    extendedMaterial->saveState(extendedMaterialJ);

    if(dynamic_cast<OpenGLMaterial*>(extendedMaterial))
      extendedMaterialJ["type"] = "OpenGL";

    else if(dynamic_cast<LegacyMaterial*>(extendedMaterial))
      extendedMaterialJ["type"] = "Legacy";

    else if(dynamic_cast<ExternalMaterial*>(extendedMaterial))
      extendedMaterialJ["type"] = "External";
  }
}

//##################################################################################################
void Material::saveUVMatrix(nlohmann::json& j) const
{
  uvTransformation.saveState(j);
  j["uvMatrix"] = tp_math_utils::mat3ToJSON(uvTransformation.uvMatrix());
  j["isIdentity"] = uvTransformation.isIdentity();
}

//##################################################################################################
void Material::loadState(const nlohmann::json& j)
{
  tpDeleteAll(extendedMaterials);
  extendedMaterials.clear();

  if(TPJSONString(j, "version") == "2.0")
  {
    // New format
    if(auto i=j.find("extendedMaterials"); i!=j.end() && i->is_array())
    {
      extendedMaterials.reserve(i->size());
      for(const auto& extendedMaterialJ : *i)
      {
        ExtendedMaterial* extendedMaterial{nullptr};

        std::string type = TPJSONString(extendedMaterialJ, "type");
        if(type == "OpenGL")
          extendedMaterial = findOrAddOpenGL();

        else if(type == "Legacy")
          extendedMaterial = findOrAddLegacy();

        else if(type == "External")
        {
          extendedMaterial = new ExternalMaterial();
          extendedMaterials.push_back(extendedMaterial);
        }

        if(extendedMaterial)
          extendedMaterial->loadState(extendedMaterialJ);
      }
    }
  }
  else if(j.find("albedo") != j.end())
  {
    // Legacy format
    findOrAddOpenGL()->loadState(j);
    findOrAddLegacy()->loadState(j);
  }

  name = TPJSONString(j, "name");
  uvTransformation.loadState(j);
}

}
