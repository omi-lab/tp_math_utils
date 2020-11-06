#include "tp_math_utils/ClosestPointsOnRays.h"
#include "tp_math_utils/Ray.h"

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
void closestPointsOnRays(const DRay& S1, const DRay& S2, glm::dvec3& P1, glm::dvec3& P2)
{
  const double SMALL_NUM = 0.00001;

  glm::dvec3 u = S1.p1 - S1.p0;
  glm::dvec3 v = S2.p1 - S2.p0;
  glm::dvec3 w = S1.p0 - S2.p0;
  double     a = glm::dot(u,u); // always >= 0
  double     b = glm::dot(u,v);
  double     c = glm::dot(v,v); // always >= 0
  double     d = glm::dot(u,w);
  double     e = glm::dot(v,w);
  double     D = a*c - b*b;     // always >= 0
  double     sc, tc;

  // compute the line parameters of the two closest points
  if(D < SMALL_NUM) {          // the lines are almost parallel
    sc = 0.0;
    tc = (b>c ? d/b : e/c);    // use the largest denominator
  }
  else {
    sc = (b*e - c*d) / D;
    tc = (a*e - b*d) / D;
  }

  P1 = S1.p0 + (sc * u);
  P2 = S2.p0 + (tc * v);
}

}
