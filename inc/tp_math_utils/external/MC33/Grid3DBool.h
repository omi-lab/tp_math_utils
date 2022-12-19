#ifndef tp_math_utils_Grid3DBool_h
#define tp_math_utils_Grid3DBool_h

#include "tp_math_utils/external/MC33/Grid3DBase.h" // IWYU pragma: keep

namespace tp_math_utils
{

//##################################################################################################
class Grid3DBool : public Grid3DBase
{
public:
  //################################################################################################
  Grid3DBool(const glm::vec<3, size_t>& size, const std::vector<bool>* data = nullptr);

  //################################################################################################
  ~Grid3DBool() override;

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
