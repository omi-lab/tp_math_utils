#ifndef tp_math_utils_Grid3DBase_h
#define tp_math_utils_Grid3DBase_h

#include "tp_math_utils/Globals.h" // IWYU pragma: keep

namespace tp_math_utils
{

//##################################################################################################
enum class InterpolationType
{
  Trilinear,
  Tricubic
};

//##################################################################################################
class Grid3DBase
{
public:
  //################################################################################################
  Grid3DBase();

  //################################################################################################
  virtual ~Grid3DBase();

  //################################################################################################
  float bad_value(double*) const; // always returns nan

  //################################################################################################
  float trilinear(double* r) const;

  //################################################################################################
  float tricubic(double* r) const;

  //################################################################################################
  //Calculate a value at position (x, y, z) by interpolating the grid values.
  float interpolatedValue(double x, double y, double z);

  //################################################################################################
  int setInterpolation(InterpolationType interpolationType);

  //################################################################################################
  //! Get a grid point value
  virtual float value(size_t x, size_t y, size_t z) const = 0;

  //################################################################################################
  virtual glm::vec<3, size_t> gridSize() const = 0;

private:
  struct Private;
  friend struct Private;
  Private* d;
};

}

#endif
