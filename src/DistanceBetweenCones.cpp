#include "tp_math_utils/DistanceBetweenCones.h"
#include "tp_math_utils/ClosestPointsOnRays.h"
#include "tp_math_utils/ClosestPointsOnLines.h"
#include "tp_math_utils/Cone.h"
#include "tp_math_utils/Ray.h"

namespace tp_math_utils
{

//##################################################################################################
double distanceBetweenCones(const tp_math_utils::DCone& c1, const tp_math_utils::DCone& c2)
{
  const double SMALL_NUM = 0.00001;

  DRay ray1(c1.p0, c1.p1);
  DRay ray2(c2.p0, c2.p1);

  //The points on the center rays of each cone where the outside edges of the cone are closest to
  //each other.
  glm::dvec3 P1;
  glm::dvec3 P2;

  //================================================================================================
  //Calculate the vector of the closest points between the center rays of each cone.
  glm::dvec3 v1;
  glm::dvec3 v2;
  bool translateCenterRays = false;
  {
    closestPointsOnRays(ray1, ray2, P1, P2);

    v1 = P2 - P1;
    auto vLength = glm::length(v1);
    if(vLength>=SMALL_NUM)
    {
      v1 /= vLength;
      translateCenterRays = true;
      v2 = -v1;
    }
    else
    {
      //The center rays intersect so use the nearest points on the center line segments. In this
      //situation the negative value representing the depth of intersection maybe less than it
      //should be. For non parallel lines this could be corrected by calculating the plane between
      //them and using its normal to calculate v1 and v2. For parallel lines an arbitary vector that
      //is perpendicular to the rays could be used to calculate v1 and v2. For now I don't care.
      closestPointsOnLines(ray1, ray2, P1, P2);
    }
  }

  //================================================================================================
  //Now calculate the ray running down the outside of each cone that is closest to the other cone.
  //Then find the two closest points on those rays and project this back onto the center line.
  if(translateCenterRays)
  {
    //The closest edges
    DRay e1(c1.p0+(v1*c1.r0), c1.p1+(v1*c1.r1));
    DRay e2(c2.p0+(v2*c2.r0), c2.p1+(v2*c2.r1));

    closestPointsOnLines(e1, e1, P1, P2);

    glm::dvec3 tmp;
    closestPointsOnRays(DRay(P1, P1-v1), ray1, tmp, P1);
    closestPointsOnRays(DRay(P2, P2-v2), ray2, tmp, P2);
  }

  //================================================================================================
  //Now P1 and P2 are points on the center lines of c1 and c2 where the outside edges of the cones
  //are closest to each other. We need to calculate the radius of each cone at this position along
  //the cone.
  {
    auto l1 = glm::distance(c1.p0, c1.p1);
    auto l2 = glm::distance(c2.p0, c2.p1);

    //This should be fraction of the way along each cone, it should be a value between 0 and 1.
    auto f1 = (l1>=SMALL_NUM)?(glm::distance(c1.p0, P1)/l1):0.0;
    auto f2 = (l2>=SMALL_NUM)?(glm::distance(c2.p0, P2)/l2):0.0;

    auto radius1 = (c1.r0*(1.0-f1)) + c1.r0*f1;
    auto radius2 = (c2.r0*(1.0-f2)) + c2.r0*f2;

    //Calculate the gap
    return glm::distance(P1, P2) - (radius1+radius2);
  }
}

}
