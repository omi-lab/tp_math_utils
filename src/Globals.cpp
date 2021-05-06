#include "tp_math_utils/Globals.h"
#include <limits>
namespace tp_math_utils
{

//##################################################################################################
bool isPowerOf2(size_t v)
{
  return v && (powerOf2(v) == v);
}

//##################################################################################################
size_t powerOf2(size_t v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  if constexpr(sizeof(size_t)>= 4) v |= v >> 16; // sizeof(size_t) >=  32bit
  if constexpr(sizeof(size_t)>= 8) v |= v >> 32; // sizeof(size_t) >=  64bit

#ifndef TP_WIN32_MSVC
  if constexpr(sizeof(size_t)>=16) v |= v >> 64; // sizeof(size_t) >= 128bit
#endif

  v++;
  return v;
}

//##################################################################################################
size_t closestPowerOf2(size_t v)
{
  size_t vUp = powerOf2(v);
  size_t vDn = vUp >> 1;
  return ((vUp-v) < (v-vDn))?vUp:vDn;
}

}
