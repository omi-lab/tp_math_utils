#include "tp_math_utils/external/MC33/Grid3DBase.h"

namespace tp_math_utils
{

//##################################################################################################
struct Grid3DBase::Private
{
  int periodic; // to work with periodical grids.
  double r0[3]; // Origin of grid
  double d[3];  // Distances between grid points
  unsigned int N[3]; // Grid dimensions
  float L[3]; // size of the grid axes
  unsigned int nsg, maxnsg; // for subgrids
  float (Grid3DBase::*interpolation)(double*) const; // pointer to interpolation function

  int nonortho;
  float Ang[3]; // angles between grid axes.
  double _A[3][3], A_[3][3];
  void update_matrices(); // set _A and A_ by using Ang
};

//##################################################################################################
Grid3DBase::Grid3DBase():
  d(new Private())
{

}

//##################################################################################################
Grid3DBase::~Grid3DBase()
{
  delete d;
}

}
