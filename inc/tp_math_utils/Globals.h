#ifndef tp_math_utils_Globals_h
#define tp_math_utils_Globals_h

#if defined(TP_MATH_UTILS_LIBRARY)
#  define TP_MATH_UTILS_SHARED_EXPORT
#else
#  define TP_MATH_UTILS_SHARED_EXPORT
#endif

#include "tp_utils/Globals.h"

#include <glm/glm.hpp>

#include <random>
#include <ostream>

//##################################################################################################
//! Math utils for 3D graphics
/*!

*/
namespace tp_math_utils
{

//##################################################################################################
struct RNG
{
  std::random_device rd;
  std::minstd_rand0 mt;

  //################################################################################################
  RNG():
    mt(rd())
  {

  }

  //################################################################################################
  float randF()
  {
    return float(mt())*(1.0f/2147483646.0f);
  }

  //################################################################################################
  float randF2()
  {
    return (float(mt())*(2.0f/2147483646.0f))-1.0f;
  }

  //################################################################################################
  void randomIndexes(size_t lessThan, size_t count, std::vector<size_t>& results)
  {
    count = tpMin(lessThan, count);

    results.clear();
    results.reserve(count);
    for(size_t c=0; c<count; c++)
    {
      size_t i = mt()%lessThan;

      while(tpContains(results, i))
      {
        i++;
        if(i>=lessThan)
          i=0;
      }

      results.push_back(i);
    }
  }

  //################################################################################################
  size_t randomIndex(size_t lessThan)
  {
    return mt()%lessThan;
  }
};

}

//##################################################################################################
template<typename T, glm::precision P>
std::ostream& operator<<(std::ostream& os, const glm::tvec2<T, P>& v)
{
  os << '(' << v.x << ',' << v.y << ')';
  return os;
}

//##################################################################################################
template<typename T, glm::precision P>
std::ostream& operator<<(std::ostream& os, const glm::tvec3<T, P>& v)
{
  os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
  return os;
}

//##################################################################################################
template<typename T, glm::precision P>
std::ostream& operator<<(std::ostream& os, const glm::tvec4<T, P>& v)
{
  os << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.w << ')';
  return os;
}

//##################################################################################################
template<typename T, glm::precision P>
std::ostream& operator<<(std::ostream& os, const glm::tmat4x4<T, P>& v)
{
  os << "([" << v[0][0] << ',' << v[1][0] << ',' << v[2][0] << ',' << v[3][0] << "]\n";
  os << " [" << v[0][1] << ',' << v[1][1] << ',' << v[2][1] << ',' << v[3][1] << "]\n";
  os << " [" << v[0][2] << ',' << v[1][2] << ',' << v[2][2] << ',' << v[3][2] << "]\n";
  os << " [" << v[0][3] << ',' << v[1][3] << ',' << v[2][3] << ',' << v[3][3] << "])";
  return os;
}

namespace std
{
//##################################################################################################
template <>
struct hash<glm::ivec2>
{
  std::size_t operator()(const glm::ivec2& k) const
  {
    return hash<int>()(k.x) ^ hash<int>()(k.y);
  }
};
}

#endif
