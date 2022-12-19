#include "tp_math_utils/external/MC33/Grid3DBits.h"
namespace tp_math_utils
{

//##################################################################################################
struct Grid3DBits::Private
{
  glm::vec<3, size_t> size;
  const void* data;

  size_t strideY;
  size_t strideZ;

  //################################################################################################
  Private(const glm::vec<3, size_t>& size_, const void* data_):
    size(size_),
    data(data_)
  {
    strideY = size.x;
    strideZ = size.x * size.y;
  }
};

//##################################################################################################
Grid3DBits::Grid3DBits(const glm::vec<3, size_t>& size, const void* data):
  Grid3DBase(),
  d(new Private(size, data))
{

}

//##################################################################################################
Grid3DBits::~Grid3DBits()
{
  delete d;
}

//##################################################################################################
float Grid3DBits::value(size_t x, size_t y, size_t z) const
{
  if(x>=d->size.x || y>=d->size.y || z>=d->size.z)
    return 0.0f;

  size_t bit = (z*d->strideZ) + (y*d->strideY) + x;

  return (static_cast<const uint8_t*>(d->data)[bit>>4] & uint8_t(1<<(bit&0x0F)))?1.0f:0.0f;
}

//##################################################################################################
glm::vec<3, size_t> Grid3DBits::gridSize() const
{
  return d->size;
}

}
