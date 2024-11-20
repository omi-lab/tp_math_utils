#include "tp_math_utils/Polygon.h"

#include "tp_utils/JSONUtils.h"

namespace tp_math_utils
{

//##################################################################################################
void deserializeGeometry(const nlohmann::json& j, Polygon& polygon)
{
  if(auto i = j.find("coordinates"); i != j.end() && i->is_array())
  {
    for(const nlohmann::json& shape : *i)
    {
      if(!shape.is_array())
        continue;

      std::vector<glm::vec2> loop;
      for(const nlohmann::json& coord : shape)
      {
        if(!coord.is_array() || coord.size()!=2 || !coord.at(0).is_number() || !coord.at(1).is_number())
          break;

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
  jCoordinates.get_ptr<nlohmann::json::array_t*>()->reserve(polygon.holes.size() + 1);

  auto addLoop = [&jCoordinates](const std::vector<glm::vec2>& loop)
  {
    jCoordinates.emplace_back();
    nlohmann::json& jLoop = jCoordinates.back();
    jLoop = nlohmann::json::array();
    jLoop.get_ptr<nlohmann::json::array_t*>()->reserve(loop.size());
    for(const glm::vec2& coord : loop)
      jLoop.emplace_back(nlohmann::json::array({coord.x, coord.y}));
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
  j.get_ptr<nlohmann::json::array_t*>()->reserve(polygons.size());

  for(const Polygon& polygon : polygons)
    j.emplace_back(serializePolygon(polygon));

  return j;
}

//##################################################################################################
nlohmann::json serializePolygonsVector(const std::vector<std::vector<Polygon>>& polygonsVector)
{
  nlohmann::json j = nlohmann::json::array();
  j.get_ptr<nlohmann::json::array_t*>()->reserve(polygonsVector.size());

  for(const std::vector<Polygon>& polygons : polygonsVector)
    j.emplace_back(serializePolygons(polygons));

  return j;
}

}
