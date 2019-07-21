#include "tp_math_utils/DistanceBetweenLines.h"
#include "tp_math_utils/Ray.h"

namespace tp_math_utils
{

//##################################################################################################
// Copyright 2001 softSurfer, 2012 Dan Sunday
// This code may be freely used, distributed and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
float distanceBetweenLines(const tp_math_utils::Ray& l1, const tp_math_utils::Ray& l2)
{
  glm::vec3 u = l1.p1 - l1.p0;
  glm::vec3 v = l2.p1 - l2.p0;
  glm::vec3 w = l1.p0 - l2.p0;
  float     a = glm::dot(u,u); // always >= 0
  float     b = glm::dot(u,v);
  float     c = glm::dot(v,v); // always >= 0
  float     d = glm::dot(u,w);
  float     e = glm::dot(v,w);
  float     D = a*c - b*b;     // always >= 0

  // compute the line parameters of the two closest points
  float sc;
  float tc;
  if(D < 0.00001f)
  {
    // the lines are almost parallel
    sc = 0.0;
    tc = (b>c ? d/b : e/c);    // use the largest denominator
  }
  else
  {
    sc = (b*e - c*d) / D;
    tc = (a*e - b*d) / D;
  }

  // get the difference of the two closest points
  glm::vec3 dP = w + (sc * u) - (tc * v);  // =  L1(sc) - L2(tc)

  return std::sqrt(glm::dot(dP,dP));   // return the closest distance
}

}
