#ifndef tp_math_utils_Polygon_h
#define tp_math_utils_Polygon_h

#include "tp_math_utils/Globals.h"

#include "json.hpp"

#include <unordered_map>

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT Polygon
{
  std::vector<glm::vec2> outer;
  std::vector<std::vector<glm::vec2>> holes;
  std::unordered_map<std::string, std::string> properties;
};

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT PolygonD
{
  std::vector<glm::dvec2> outer;
  std::vector<std::vector<glm::dvec2>> holes;
  std::unordered_map<std::string, std::string> properties;
};

//##################################################################################################
void TP_MATH_UTILS_SHARED_EXPORT deserializeGeometry(const nlohmann::json& j, Polygon& polygon);

//##################################################################################################
void TP_MATH_UTILS_SHARED_EXPORT deserializeProperties(const nlohmann::json& j, Polygon& polygon);

//##################################################################################################
void TP_MATH_UTILS_SHARED_EXPORT deserializePolygon(const nlohmann::json& j, Polygon& polygon);

//##################################################################################################
void TP_MATH_UTILS_SHARED_EXPORT deserializePolygons(const nlohmann::json& j, std::vector<Polygon>& polygons);

//##################################################################################################
//! Produces the 'geometry' part of a GeoJSON feature
nlohmann::json TP_MATH_UTILS_SHARED_EXPORT serializeGeometry(const Polygon& polygon);

//##################################################################################################
//! Produces the 'properties' part of a GeoJSON feature
nlohmann::json TP_MATH_UTILS_SHARED_EXPORT serializeProperties(const Polygon& polygon);

//##################################################################################################
nlohmann::json TP_MATH_UTILS_SHARED_EXPORT serializePolygon(const Polygon& polygon);

//##################################################################################################
nlohmann::json TP_MATH_UTILS_SHARED_EXPORT serializePolygons(const std::vector<Polygon>& polygons);

//##################################################################################################
nlohmann::json TP_MATH_UTILS_SHARED_EXPORT serializePolygonsVector(const std::vector<std::vector<Polygon>>& polygonsVector);

}

#endif
