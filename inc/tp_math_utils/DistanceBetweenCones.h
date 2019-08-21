#ifndef tp_math_utils_DistanceBetweenCones_h
#define tp_math_utils_DistanceBetweenCones_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{
struct DCone;

//##################################################################################################
/*!
\note This algorithm currently puts a sphear on the end of each cone.

\param c1 The first cone.
\param c2 The second cone.
\return The distance between the surfaces of the two cones at the closest point.
*/
double TP_MATH_UTILS_SHARED_EXPORT distanceBetweenCones(const tp_math_utils::DCone& c1, const tp_math_utils::DCone& c2);

}

#endif
