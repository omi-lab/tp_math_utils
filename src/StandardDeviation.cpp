#include "tp_math_utils/StandardDeviation.h"

#include <algorithm>

namespace tp_math_utils
{

//##################################################################################################
double calculateSD(const std::vector<float>& data)
{
  if(data.empty())
    return 0.0;

  double sum=0.0;
  for(float f : data)
    sum += double(f);

  double mean=sum/double(data.size());

  double standardDeviation = 0.0;
  for(float f : data)
    standardDeviation += std::pow(double(f)-mean, 2.0);

  return std::sqrt(standardDeviation / double(data.size()));
}

//##################################################################################################
double calculateSD(const std::vector<float>& data, double& mean)
{
  if(data.empty())
  {
    mean=0.0;
    return 0.0;
  }

  double sum=0.0;
  for(float f : data)
    sum += double(f);

  mean=sum/double(data.size());

  double standardDeviation = 0.0;
  for(float f : data)
    standardDeviation += std::pow(double(f)-mean, 2.0);

  return std::sqrt(standardDeviation / double(data.size()));
}

//##################################################################################################
void sortedDifferences(std::vector<float>& data)
{
  std::sort(data.begin(), data.end());

  int iMax = int(data.size())-1;
  for(int i=0; i<iMax; i++)
    data[i] = data[i+1] - data[i];

  if(iMax>0)
    data.pop_back();
}

}
