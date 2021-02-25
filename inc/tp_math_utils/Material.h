#ifndef tp_math_utils_Material_h
#define tp_math_utils_Material_h

#include "tp_math_utils/Globals.h"

#include "json.hpp"

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT Material
{
  tp_utils::StringID name;

  glm::vec3 albedo  {0.4f, 0.0f, 0.0f}; //!< mtl: Kd or Ka
  glm::vec3 sss     {1.0f, 1.0f, 1.0f}; //!< Subsurface scattering color.
  glm::vec3 emission{0.0f, 0.0f, 0.0f}; //!< Emission color.

  float albedoScale{1.0f};              //!< Multiplied by the albedo
  float sssScale{0.0f};                 //!< Subsurface scattering factor.
  float emissionScale{0.0f};            //!< Subsurface scattering factor.
  float heightScale{0.01f};              //!< Used to scale subdivision height maps.
  float heightMidlevel{0.5f};           //!< Used to offset subdivision height maps.

  float alpha{1.0f};                    //!< mtl: d
  float roughness{1.0f};                //!<
  float metalness{0.0f};                //!<
  float transmission{0.0f};             //!< Transmission for refractive materials like glass or water.
  float ior{1.45f};                     //!< Index of refraction.

  glm::vec3 sssRadius{0.2f,0.2f,0.2f};  //!< Subsurface scattering radius per color.

  float useAmbient    {1.0f};           //!< Should the ambient light be used to modulate albedo.
  float useDiffuse    {1.0f};           //!< Should the diffuse calculation be used to modulate albedo.
  float useNdotL      {1.0f};           //!< Should the angel of the light be used to calculate the color.
  float useAttenuation{1.0f};           //!< Should the distance to the light be used to attenuate the color.
  float useShadow     {1.0f};           //!< Should the shadow be used in the color calculation.
  float useLightMask  {1.0f};           //!< Should the light mask texture be used in the color calculation.
  float useReflection {1.0f};           //!< Should the reflection be used in the final color calculation.

  bool tileTextures{false};             //!< True to GL_REPEAT textures else GL_CLAMP_TO_EDGE

  tp_utils::StringID    albedoTexture;  //!< mtl: map_Kd or map_Ka
  tp_utils::StringID     alphaTexture;  //!< mtl: map_d
  tp_utils::StringID   normalsTexture;  //!< mtl: map_Bump
  tp_utils::StringID roughnessTexture;  //!<
  tp_utils::StringID metalnessTexture;  //!<
  tp_utils::StringID  emissionTexture;  //!<
  tp_utils::StringID       sssTexture;  //!<
  tp_utils::StringID    heightTexture;  //!< Subdivision height.

  //################################################################################################
  nlohmann::json saveState() const;

  //################################################################################################
  void loadState(const nlohmann::json& j);
};

}

#endif
