#include "tp_math_utils/external/MC33/Grid3DBool.h"
namespace tp_math_utils
{

//##################################################################################################
struct Grid3DBool::Private
{
  glm::vec<3, size_t> size;
  std::vector<bool> ownedData;
  const std::vector<bool>* data;

  size_t strideY;
  size_t strideZ;

  //################################################################################################
  Private(const glm::vec<3, size_t>& size_, const std::vector<bool>* data_):
    size(size_),
    data(data_)
  {
    if(!data)
    {
      ownedData.resize(size.x * size.y * size.z, false);
      data = &ownedData;
    }

    strideY = size.x;
    strideZ = size.x * size.y;
  }
};

//##################################################################################################
Grid3DBool::Grid3DBool(const glm::vec<3, size_t>& size, const std::vector<bool>* data):
  Grid3DBase(),
  d(new Private(size, data))
{

}

//##################################################################################################
Grid3DBool::~Grid3DBool()
{
  delete d;
}

//##################################################################################################
float Grid3DBool::value(size_t x, size_t y, size_t z) const
{
  if(x>=d->size.x || y>=d->size.y || z>=d->size.z)
    return 0.0f;

  return d->data->at((z*d->strideZ) + (y*d->strideY) + x)?1.0f:0.0f;
}

//##################################################################################################
glm::vec<3, size_t> Grid3DBool::gridSize() const
{
  return d->size;
}

}
