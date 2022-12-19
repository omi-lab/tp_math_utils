#ifndef tp_math_utils_Grid3DBits_h
#define tp_math_utils_Grid3DBits_h

#include "tp_math_utils/external/MC33/Grid3DBase.h" // IWYU pragma: keep

namespace tp_math_utils
{

//##################################################################################################
class Grid3DBits : public Grid3DBase
{
public:
  //################################################################################################
  Grid3DBits(const glm::vec<3, size_t>& size, const void* data);

  //################################################################################################
  ~Grid3DBits() override;

  //################################################################################################
  float value(size_t x, size_t y, size_t z) const override;

  //################################################################################################
  glm::vec<3, size_t> gridSize() const override;

private:
  struct Private;
  friend struct Private;
  Private* d;
};

}

#endif
