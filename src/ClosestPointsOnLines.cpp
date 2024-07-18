#include "tp_math_utils/ClosestPointsOnLines.h"
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
void closestPointsOnLines(const DRay& S1, const DRay& S2, glm::dvec3& P1, glm::dvec3& P2)
{
  const double SMALL_NUM = 0.00001;

  glm::dvec3 u = S1.p1 - S1.p0;
  glm::dvec3 v = S2.p1 - S2.p0;
  glm::dvec3 w = S1.p0 - S2.p0;
  double     a = glm::dot(u,u);         // always >= 0
  double     b = glm::dot(u,v);
  double     c = glm::dot(v,v);         // always >= 0
  double     d = glm::dot(u,w);
  double     e = glm::dot(v,w);
  double     D = a*c - b*b;        // always >= 0
  double     sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
  double     tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

  // compute the line parameters of the two closest points
  if(D < SMALL_NUM)
  {
    // the lines are almost parallel
    sN = 0.0;         // force using point near on segment S1
    sD = 1.0;         // to prevent possible division by 0.0 later
    tN = e;
    tD = c;
  }
  else
  {
    // get the closest points on the infinite lines
    sN = (b*e - c*d);
    tN = (a*e - b*d);
    if(sN < 0.0)
    {
      // sc < 0 => the s=0 edge is visible
      sN = 0.0;
      tN = e;
      tD = c;
    }
    else if(sN > sD)
    {
      // sc > 1  => the s=1 edge is visible
      sN = sD;
      tN = e + b;
      tD = c;
    }
  }

  if(tN < 0.0)
  {
    // tc < 0 => the t=0 edge is visible
    tN = 0.0;
    // recompute sc for this edge
    if(-d < 0.0)
      sN = 0.0;
    else if(-d > a)
      sN = sD;
    else
    {
      sN = -d;
      sD = a;
    }
  }
  else if(tN > tD)
  {
    // tc > 1  => the t=1 edge is visible
    tN = tD;
    // recompute sc for this edge
    if((-d + b) < 0.0)
      sN = 0;
    else if((-d + b) > a)
      sN = sD;
    else
    {
      sN = (-d +  b);
      sD = a;
    }
  }
  // finally do the division to get sc and tc
  sc = (std::abs(sN) < SMALL_NUM ? 0.0 : sN / sD);
  tc = (std::abs(tN) < SMALL_NUM ? 0.0 : tN / tD);

  P1 = S1.p0 + (sc * u);
  P2 = S2.p0 + (tc * v);
}

}
