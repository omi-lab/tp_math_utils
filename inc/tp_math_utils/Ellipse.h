#ifndef tp_math_utils_Ellipse_h
#define tp_math_utils_Ellipse_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_SHARED_EXPORT Ellipse
{
  float semiMajor;
  float semiMinor;
  float angle;

  Ellipse(float semiMajor_=1.0f, float semiMinor_=1.0f, float angle_=0.0f):
    semiMajor(semiMajor_),
    semiMinor(semiMinor_),
    angle(angle_)
  {
  }
};

}

#endif
