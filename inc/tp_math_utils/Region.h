#ifndef tp_math_utils_Region_h
#define tp_math_utils_Region_h

#include "tp_math_utils/Globals.h"

namespace tp_math_utils
{

//##################################################################################################
struct TP_MATH_UTILS_EXPORT Region
{
  size_t classIndex;
  std::string className;

  double probability{1.0};

  //Bounding box
  float mixX{0.0f};
  float mixY{0.0f};
  float maxX{0.0f};
  float maxY{0.0f};

  //Shape if available
  std::vector<glm::vec2> shape;
};

}

#endif
