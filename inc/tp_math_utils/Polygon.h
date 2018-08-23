#ifndef tp_math_utils_Polygon_h
#define tp_math_utils_Polygon_h

#include "json.hpp"

#include <glm/glm.hpp>

#include <unordered_map>

namespace tp_math_utils
{

//##################################################################################################
struct Polygon
{
  std::vector<glm::vec2> outer;
  std::vector<std::vector<glm::vec2>> holes;
  std::unordered_map<std::string, std::string> properties;
};

//##################################################################################################
struct PolygonD
{
  std::vector<glm::dvec2> outer;
  std::vector<std::vector<glm::dvec2>> holes;
  std::unordered_map<std::string, std::string> properties;
};

//##################################################################################################
void deserializeGeometry(const nlohmann::json& j, Polygon& polygon);

//##################################################################################################
void deserializeProperties(const nlohmann::json& j, Polygon& polygon);

//##################################################################################################
void deserializePolygon(const nlohmann::json& j, Polygon& polygon);

//##################################################################################################
void deserializePolygons(const nlohmann::json& j, std::vector<Polygon>& polygons);

//##################################################################################################
//! Produces the 'geometry' part of a GeoJSON feature
nlohmann::json serializeGeometry(const Polygon& polygon);

//##################################################################################################
//! Produces the 'properties' part of a GeoJSON feature
nlohmann::json serializeProperties(const Polygon& polygon);

//##################################################################################################
nlohmann::json serializePolygon(const Polygon& polygon);

//##################################################################################################
nlohmann::json serializePolygons(const std::vector<Polygon>& polygons);

//##################################################################################################
nlohmann::json serializePolygonsVector(const std::vector<std::vector<Polygon>>& polygonsVector);

}

#endif
