#include "tp_math_utils/MidPointBetweenLines.h"
#include "tp_math_utils/ClosestPointsOnLines.h"
#include "tp_math_utils/Ray.h"

#include "tp_utils/Globals.h"

namespace tp_math_utils
{

//http://geomalgorithms.com/a07-_distance.html

//##################################################################################################
// Copyright 2001 softSurfer, 2012 Dan Sunday
// This code may be freely used, distributed and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
void midPointBetweenLines(const tp_math_utils::DRay& S1, const tp_math_utils::DRay& S2, glm::dvec3& midPoint)
{
  glm::dvec3 sc3D;
  glm::dvec3 tc3D;
  closestPointsOnLines(S1, S2, sc3D, tc3D);

  glm::dvec3 shortestVec = sc3D - tc3D;

  midPoint = tc3D + (shortestVec/2.0);
}

}
