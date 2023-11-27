#ifndef tp_math_utils_Material_h
#define tp_math_utils_Material_h

#include "tp_math_utils/Globals.h"

#include "tp_utils/StringID.h"

#include "json.hpp"

#include <unordered_set>

namespace tp_math_utils
{

//##################################################################################################
enum class Colorspace
{
  sRGB = 0, //!< The default colorspace
  FilmicSRGB = 1  //!< Newer colorspace
};

//##################################################################################################
std::string colorspaceToString(Colorspace colorspace);

//##################################################################################################
Colorspace colorspaceFromString(const std::string& colorspace);

//##################################################################################################
enum class SSSMethod
{
  ChristensenBurley = 0,
  RandomWalk        = 1
};

//##################################################################################################
std::string sssMethodToString(SSSMethod sssMethod);

//##################################################################################################
SSSMethod sssMethodFromString(const std::string& sssMethod);

//##################################################################################################
enum class ShaderType
{
  Principled = 0, //!< The default shaded material.
  None       = 1  //!< Just use the albedo color/texture directly.
};

//##################################################################################################
std::string shaderTypeToString(ShaderType shaderType);

//##################################################################################################
ShaderType shaderTypeFromString(const std::string& shaderType);

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

  ShaderType shaderType{ShaderType::Principled}; //!< What shader should be used to draw the object.

  Colorspace albedoColorspace{Colorspace::sRGB};

  glm::vec3 albedo  {0.4f, 0.0f, 0.0f}; //!< mtl: Kd or Ka
  glm::vec3 sss     {1.0f, 1.0f, 1.0f}; //!< Subsurface scattering color.
  glm::vec3 emission{0.0f, 0.0f, 0.0f}; //!< Emission color.
  glm::vec3 velvet  {0.0f, 0.0f, 0.0f}; //!< Velvet color. OBSOLETE

  float albedoScale{1.0f};              //!< Multiplied by the albedo
  float sssScale{0.0f};                 //!< Subsurface scattering factor.
  float emissionScale{0.0f};            //!< Emission factor.
  float velvetScale{0.0f};              //!< Velvet factor. OBSOLETE
  float heightScale{0.01f};             //!< Used to scale subdivision height maps.
  float heightMidlevel{0.5f};           //!< Used to offset subdivision height maps.

  float alpha{1.0f};                    //!< mtl: d
  float roughness{0.5f};                //!<
  float metalness{0.0f};                //!<
  float specular{0.5f};                 //!<
  float transmission{0.0f};             //!< Transmission for refractive materials like glass or water.
  float transmissionRoughness{0.0f};    //!<
  float ior{1.45f};                     //!< Index of refraction.
  float sheen{0.0f};                    //!<
  float sheenTint{0.5f};                //!<
  float clearCoat{0.0f};                //!<
  float clearCoatRoughness{0.0f};       //!<

  float    iridescentFactor{0.0f}; // OBSOLETE
  float    iridescentOffset{0.0f}; // OBSOLETE
  float iridescentFrequency{0.0f}; // OBSOLETE

  glm::vec3 sssRadius{1.0f,0.2f,0.1f};  //!< Subsurface scattering radius per color.

  SSSMethod sssMethod{SSSMethod::ChristensenBurley}; //!< Subsurface scattering method.

  float normalStrength{1.0f};           //!< Used to scale normals.

  float albedoBrightness{0.0f};         //!< Modify the albedo texture brightness.
  float albedoContrast  {0.0f};         //!< Modify the albedo texture contrast.
  float albedoGamma     {1.0f};         //!< Modify the albedo texture gamma.
  float albedoHue       {0.5f};         //!< Modify the albedo texture hue.
  float albedoSaturation{1.0f};         //!< Modify the albedo texture saturation.
  float albedoValue     {1.0f};         //!< Modify the albedo texture value.
  float albedoFactor    {1.0f};         //!< Modify the albedo texture factor.

  float useAmbient    {1.0f};           //!< Should the ambient light be used to modulate albedo.
  float useDiffuse    {1.0f};           //!< Should the diffuse calculation be used to modulate albedo.
  float useNdotL      {1.0f};           //!< Should the angel of the light be used to calculate the color.
  float useAttenuation{1.0f};           //!< Should the distance to the light be used to attenuate the color.
  float useShadow     {1.0f};           //!< Should the shadow be used in the color calculation.
  float useLightMask  {1.0f};           //!< Should the light mask texture be used in the color calculation.
  float useReflection {1.0f};           //!< Should the reflection be used in the final color calculation.

  bool tileTextures{false};             //!< True to GL_REPEAT textures else GL_CLAMP_TO_EDGE

  UVTransformation uvTransformation;    //! Modify UV coords.

  bool rayVisibilityCamera        {true};  //!< Blender cycles ray visibility options.
  bool rayVisibilityDiffuse       {true};  //!< Blender cycles ray visibility options.
  bool rayVisibilityGlossy        {true};  //!< Blender cycles ray visibility options.
  bool rayVisibilityTransmission  {true};  //!< Blender cycles ray visibility options.
  bool rayVisibilityScatter       {true};  //!< Blender cycles ray visibility options.
  bool rayVisibilityShadow        {true};  //!< Blender cycles ray visibility options.
  bool rayVisibilityShadowCatcher {false};  //!< Blender cycles ray visibility options.

  tp_utils::StringID                  rgbaTexture; //!< RGBA, combined albedo and alpha.
  tp_utils::StringID                 rmttrTexture; //!< RGBA, combined roughness, metalness, transmission, transmission roughness.

  tp_utils::StringID                albedoTexture; //!< RGB,                   mtl: map_Kd or map_Ka
  tp_utils::StringID                 alphaTexture; //!< Grey scale,            mtl: map_d
  tp_utils::StringID               normalsTexture; //!< RGB (XYZ) Unit vector, mtl: map_Bump
  tp_utils::StringID             roughnessTexture; //!< Grey scale.
  tp_utils::StringID             metalnessTexture; //!< Grey scale.
  tp_utils::StringID              specularTexture; //!< Grey scale.
  tp_utils::StringID              emissionTexture; //!< RGB.
  tp_utils::StringID                   sssTexture; //!< RGB.
  tp_utils::StringID              sssScaleTexture; //!< Grey scale.
  tp_utils::StringID                heightTexture; //!< Grey scale, Subdivision height.
  tp_utils::StringID          transmissionTexture; //!< Grey scale.
  tp_utils::StringID transmissionRoughnessTexture; //!< Grey scale.
  tp_utils::StringID                 sheenTexture; //!< Grey scale.
  tp_utils::StringID             sheenTintTexture; //!< Grey scale.
  tp_utils::StringID             clearCoatTexture; //!< Grey scale.
  tp_utils::StringID    clearCoatRoughnessTexture; //!< Grey scale.
  tp_utils::StringID                velvetTexture; //!< OBSOLETE Color
  tp_utils::StringID          velvetFactorTexture; //!< OBSOLETE Grey scale, Mix 1=Velvet, 0=Principled
  tp_utils::StringID      iridescentFactorTexture; //!< OBSOLETE Grey scale, Mix 1=Iridescent color, 0=Base color.
  tp_utils::StringID      iridescentOffsetTexture; //!< OBSOLETE Grey scale, rotate the hue of the iridescent color.
  tp_utils::StringID   iridescentFrequencyTexture; //!< OBSOLETE Grey scale, multiplied with the hue angle to alter the frequency of color change.


  //################################################################################################
  template<typename T>
  void updateTypedTextures(const T& closure)
  {
    closure(                 "rgbaTexture",                  rgbaTexture, "RGBA"                  );
    closure(                "rmttrTexture",                 rmttrTexture, "RMTTR"                 );

    closure(               "albedoTexture",                albedoTexture, "Albedo"                );
    closure(                "alphaTexture",                 alphaTexture, "Alpha"                 );
    closure(              "normalsTexture",               normalsTexture, "Normals"               );
    closure(            "roughnessTexture",             roughnessTexture, "Roughness"             );
    closure(            "metalnessTexture",             metalnessTexture, "Metalness"             );
    closure(             "specularTexture",              specularTexture, "Specular"              );
    closure(             "emissionTexture",              emissionTexture, "Emission"              );
    closure(                  "sssTexture",                   sssTexture, "SSS"                   );
    closure(             "sssScaleTexture",              sssScaleTexture, "SSS scale"             );
    closure(               "heightTexture",                heightTexture, "Height"                );
    closure(         "transmissionTexture",          transmissionTexture, "Transmission"          );
    closure("transmissionRoughnessTexture", transmissionRoughnessTexture, "Transmission roughness");
    closure(                "sheenTexture",                 sheenTexture, "Sheen"                 );
    closure(            "sheenTintTexture",             sheenTintTexture, "Sheen tint"            );
    closure(            "clearCoatTexture",             clearCoatTexture, "Clear coat"            );
    closure(   "clearCoatRoughnessTexture",    clearCoatRoughnessTexture, "Clear coat roughness"  );
    closure(               "velvetTexture",                velvetTexture, "Velvet"                );
    closure(         "velvetFactorTexture",          velvetFactorTexture, "Velvet factor"         );
    closure(     "iridescentFactorTexture",      iridescentFactorTexture, "Iridescent factor"     );
    closure(     "iridescentOffsetTexture",      iridescentOffsetTexture, "Iridescent offset"     );
    closure(  "iridescentFrequencyTexture",   iridescentFrequencyTexture, "Iridescent frequency"  );
  }

  //################################################################################################
  template<typename T>
  void viewTypedTextures(const T& closure) const
  {
    const_cast<Material*>(this)->updateTypedTextures([&](const auto& type, const auto& value, const auto& pretty){closure(type, value, pretty);});
  }

  //################################################################################################
  template<typename T>
  void updateTextures(const T& closure)
  {
    updateTypedTextures([&](const auto&, auto& value, const auto&){closure(value);});
  }

  //################################################################################################
  template<typename T>
  void viewTextures(const T& closure) const
  {
    viewTypedTextures([&](const auto&, const auto& value, const auto&){closure(value);});
  }

  //################################################################################################
  inline std::unordered_set<tp_utils::StringID> allTextures() const
  {
    std::unordered_set<tp_utils::StringID> allTextures;
    viewTextures([&](const tp_utils::StringID& value){if(value.isValid())allTextures.insert(value);});
    return allTextures;
  }

  //################################################################################################
  void saveState(nlohmann::json& j) const;

  //################################################################################################
  void saveExtendedState(nlohmann::json& j) const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
