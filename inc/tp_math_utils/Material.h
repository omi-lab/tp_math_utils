#ifndef tp_math_utils_Material_h
#define tp_math_utils_Material_h

#include "tp_math_utils/Globals.h"

#include "tp_utils/StringID.h"

#include "json.hpp"

#include <unordered_set>

namespace tp_math_utils
{
class OpenGLMaterial;
class LegacyMaterial;
class ExternalMaterial;

//##################################################################################################
class TP_MATH_UTILS_EXPORT ExtendedMaterial
{
public:
  //################################################################################################
  virtual ~ExtendedMaterial();

  //################################################################################################
  virtual void saveState(nlohmann::json& j) const=0;

  //################################################################################################
  virtual void loadState(const nlohmann::json& j)=0;

  //################################################################################################
  virtual void allTextureIDs(std::unordered_set<tp_utils::StringID>& textureIDs) const;

  //################################################################################################
  virtual void appendBlendFileIDs(std::unordered_set<tp_utils::StringID>& blendFileIDs) const;

  //################################################################################################
  std::unordered_set<tp_utils::StringID> allTextures() const;
};

//##################################################################################################
struct UVTransformation
{
  glm::vec2      skewUV{0.0f, 0.0f}; //!< Skew the texture coords. m[1][0] = glm::tan(glm::radians(skewUV.x)); m[0][1] = glm::tan(glm::radians(skewUV.y));
  glm::vec2     scaleUV{1.0f, 1.0f}; //!< Scale the texture coords.
  glm::vec2 translateUV{0.0f, 0.0f}; //!< Translate the texture coords.
  float        rotateUV{0.0f};       //!< Rotate the texture coords. Degrees.

  //################################################################################################
  glm::mat3 uvMatrix() const;

  //################################################################################################
  bool isIdentity() const;

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};


//##################################################################################################
struct TP_MATH_UTILS_EXPORT Material
{
  tp_utils::StringID name;

  std::vector<ExtendedMaterial*> extendedMaterials;

  UVTransformation uvTransformation; //! Modify UV coords.

  //################################################################################################
  Material();

  //################################################################################################
  Material(const tp_utils::StringID& name_);

  //################################################################################################
  Material(const Material& other);

  //################################################################################################
  Material(Material&& other) noexcept;

  //################################################################################################
  Material& operator=(const Material& other);

  //################################################################################################
  Material& operator=(Material&& other) noexcept;

  //################################################################################################
  ~Material();

  //################################################################################################
  OpenGLMaterial* findOrAddOpenGL();

  //################################################################################################
  LegacyMaterial* findOrAddLegacy();

  //################################################################################################
  ExternalMaterial* findOrAddExternal(const tp_utils::StringID& assetType);

  //################################################################################################
  bool hasExternal(const tp_utils::StringID& assetType);

  //################################################################################################
  void updateOpenGL(const std::function<void(OpenGLMaterial&)>& closure) const;

  //################################################################################################
  void updateLegacy(const std::function<void(LegacyMaterial&)>& closure) const;

  //################################################################################################
  void updateExternal(const tp_utils::StringID& assetType,
                    const std::function<void(ExternalMaterial&)>& closure) const;

  //################################################################################################
  void viewOpenGL(const std::function<void(const OpenGLMaterial&)>& closure) const;

  //################################################################################################
  void viewLegacy(const std::function<void(const LegacyMaterial&)>& closure) const;

  //################################################################################################
  void viewExternal(const tp_utils::StringID& assetType,
                    const std::function<void(const ExternalMaterial&)>& closure) const;

  //################################################################################################
  void allTextureIDs(std::unordered_set<tp_utils::StringID>& textureIDs) const;

  //################################################################################################
  void appendBlendFileIDs(std::unordered_set<tp_utils::StringID>& blendFileIDs) const;

  //################################################################################################
  std::unordered_set<tp_utils::StringID> allTextures() const;

  //################################################################################################
  std::unordered_set<tp_utils::StringID> allBlendFiles() const;

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void saveUVMatrix(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
