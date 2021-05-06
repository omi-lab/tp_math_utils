#include "tp_math_utils/Polygon.h"

#include "tp_utils/JSONUtils.h"

namespace tp_math_utils
{

//##################################################################################################
void deserializeGeometry(const nlohmann::json& j, Polygon& polygon)
{
  try
  {
    for(const nlohmann::json& shape : TPJSON(j, "coordinates"))
    {
      std::vector<glm::vec2> loop;
      for(const nlohmann::json& coord : shape)
      {
        float x = coord.at(0);
        float y = coord.at(1);
        loop.emplace_back(x, y);
      }

      if(polygon.outer.empty())
        polygon.outer = loop;
      else
        polygon.holes.push_back(loop);
    }
  }
  catch(...)
  {

  }
}

//##################################################################################################
void deserializeProperties(const nlohmann::json& j, Polygon& polygon)
{
  try
  {
    for(nlohmann::json::const_iterator it = j.begin(); it != j.end(); ++it)
    {
      std::string value = it.value();
      polygon.properties[it.key()] = value;
    }
  }
  catch(...)
  {

  }
}

//##################################################################################################
void deserializePolygon(const nlohmann::json& j, Polygon& polygon)
{
  deserializeGeometry(TPJSON(j, "geometry"), polygon);
  deserializeProperties(TPJSON(j, "properties"), polygon);
}

//##################################################################################################
void deserializePolygons(const nlohmann::json& j, std::vector<Polygon>& polygons)
{
  try
  {
    for(const nlohmann::json& data : j)
    {
      Polygon polygon;
      deserializePolygon(data, polygon);
      polygons.push_back(polygon);
    }
  }
  catch(...)
  {

  }
}
//##################################################################################################
nlohmann::json serializeGeometry(const Polygon& polygon)
{
  nlohmann::json j;
  j["type"] = "Polygon";

  nlohmann::json& jCoordinates = j["coordinates"];
  jCoordinates = nlohmann::json::array();

  auto addLoop = [&jCoordinates](const std::vector<glm::vec2>& loop)
  {
    nlohmann::json jLoop = nlohmann::json::array();
    for(const glm::vec2& coord : loop)
    {
      nlohmann::json jCoord = nlohmann::json::array();
      jCoord.push_back(coord.x);
      jCoord.push_back(coord.y);
      jLoop.push_back(jCoord);
    }
    jCoordinates.push_back(jLoop);
  };

  addLoop(polygon.outer);
  for(const std::vector<glm::vec2>& loop : polygon.holes)
    addLoop(loop);

  return j;
}

//##################################################################################################
nlohmann::json serializeProperties(const Polygon& polygon)
{
  nlohmann::json j = nlohmann::json::object();

  for(const auto& i : polygon.properties)
    j[i.first] = i.second;

  return j;
}

//##################################################################################################
nlohmann::json serializePolygon(const Polygon& polygon)
{
  nlohmann::json j;

  j["type"] = "Feature";
  j["geometry"] = serializeGeometry(polygon);
  j["properties"] = serializeProperties(polygon);

  return j;
}

//##################################################################################################
nlohmann::json serializePolygons(const std::vector<Polygon>& polygons)
{
  nlohmann::json j = nlohmann::json::array();

  for(const Polygon& polygon : polygons)
    j.push_back(serializePolygon(polygon));

  return j;
}

//##################################################################################################
nlohmann::json serializePolygonsVector(const std::vector<std::vector<Polygon>>& polygonsVector)
{
  nlohmann::json j = nlohmann::json::array();

  for(const std::vector<Polygon>& polygons : polygonsVector)
    j.push_back(serializePolygons(polygons));

  return j;
}

}
