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
