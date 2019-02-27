#ifndef tp_math_utils_BoxPacker_h
#define tp_math_utils_BoxPacker_h

#include "tp_math_utils/Globals.h"

#include "tp_utils/DebugUtils.h"

namespace tp_math_utils
{

//##################################################################################################
template<typename PixelType, typename InputBoxT>
struct BoxPacker
{
  struct InputBox : public InputBoxT
  {
    size_t  width{0};         //!< The width of the input data.
    size_t height{0};         //!< The height of the input data.

    size_t outputBoxIndex{0}; //!< Set by pack() to the output box index.

    glm::vec2 topRight;    //!< Texture coord set by pack().
    glm::vec2 bottomRight; //!< Texture coord set by pack().
    glm::vec2 bottomLeft;  //!< Texture coord set by pack().
    glm::vec2 topLeft;     //!< Texture coord set by pack().

  private:
    glm::ivec2 origin;
    bool rotate{false};
    size_t  rotatedWidth{0};
    size_t rotatedHeight{0};
    friend struct BoxPacker;
  };

  struct OutputBox
  {
    bool padding{false};
    size_t size{16};
    std::vector<size_t> inputBoxIndexes;
  };

  //################################################################################################
  static void pack(std::vector<InputBox>& inputBoxes,
                   std::vector<OutputBox>& outputBoxes,
                   size_t maxSize,
                   bool padding)
  {
    for(auto& inputBox : inputBoxes)
    {
      if(inputBox.height > inputBox.width)
      {
        inputBox.rotatedWidth  = inputBox.height;
        inputBox.rotatedHeight = inputBox.width;
        inputBox.rotate = true;
      }
      else
      {
        inputBox.rotatedWidth  = inputBox.width;
        inputBox.rotatedHeight = inputBox.height;
      }

      if(padding)
      {
        inputBox.rotatedWidth  += 2;
        inputBox.rotatedHeight += 2;
      }

      if(inputBox.rotatedWidth>maxSize)
        maxSize = inputBox.rotatedWidth;

      if(inputBox.rotatedHeight>maxSize)
        maxSize = inputBox.rotatedHeight;
    }

    auto po2 = [](size_t v)
    {
      v--;
      v |= v >> 1;
      v |= v >> 2;
      v |= v >> 4;
      v |= v >> 8;
      v |= v >> 16;
      v++;
      return v;
    };
    maxSize = po2(maxSize);

    std::vector<size_t> sortedBoxes;
    sortedBoxes.reserve(inputBoxes.size());
    for(size_t i=0; i<inputBoxes.size(); i++)
      sortedBoxes.push_back(i);

    std::sort(sortedBoxes.begin(), sortedBoxes.end(), [&](size_t a, size_t b)
    {
      return inputBoxes[a].rotatedHeight > inputBoxes[b].rotatedHeight;}
    );

    while(!sortedBoxes.empty())
    {
      //-- The box iterator function ---------------------------------------------------------------
      auto iterateBoxes = [](size_t outputSize, auto& sortedBoxes, const auto& inputBoxes, auto f)
      {
        size_t tallest = 0;
        for(size_t currentY=0; currentY<outputSize; currentY+=tallest)
        {
          if(sortedBoxes.empty())
            break;

          tallest = 0;

          for(size_t currentX=0; currentX<outputSize;)
          {
            bool doneX = true;
            for(size_t i=0; i<sortedBoxes.size(); i++)
            {
              auto ii = sortedBoxes.at(i);
              auto& inputBox = inputBoxes.at(ii);

              if((currentX+inputBox.rotatedWidth) > outputSize)
                continue;

              if((currentY+inputBox.rotatedHeight) > outputSize)
                continue;

              doneX = false;
              f(currentX, currentY, ii);
              currentX+=inputBox.rotatedWidth;
              tallest = tpMax(tallest, inputBox.rotatedHeight);
              tpRemoveAt(sortedBoxes, i);
              break;
            }

            if(doneX)
              break;
          }

          if(tallest<1)
            break;
        }
      };

      //-- Calculate the size for the next texture -------------------------------------------------
      OutputBox outputBox;
      outputBox.padding = padding;
      for(;;)
      {
        std::vector<size_t> sortedBoxesCopy = sortedBoxes;

        iterateBoxes(outputBox.size, sortedBoxesCopy, inputBoxes, [](size_t, size_t, size_t){});

        if(sortedBoxesCopy.empty())
          break;

        if((outputBox.size*2) > maxSize)
          break;

        outputBox.size *= 2;
      }

      //-- Calculate positions for each box in this texture ----------------------------------------
      iterateBoxes(outputBox.size, sortedBoxes, inputBoxes, [&](size_t x, size_t y, size_t inputBoxIndex)
      {
        auto& inputBox = inputBoxes.at(inputBoxIndex);
        inputBox.outputBoxIndex = outputBoxes.size();
        inputBox.origin = {int(x), int(y)};

        float p = padding?1.0f:0.0f;
        float w = float(inputBox.rotate?inputBox.height:inputBox.width );
        float h = float(inputBox.rotate?inputBox.width :inputBox.height);

        inputBox.topRight    = {float(x)+p+w, float(y)+p+h};
        inputBox.bottomRight = {float(x)+p+w, float(y)+p  };
        inputBox.bottomLeft  = {float(x)+p  , float(y)+p  };
        inputBox.topLeft     = {float(x)+p  , float(y)+p+h};

        inputBox.topRight    /= float(outputBox.size);
        inputBox.bottomRight /= float(outputBox.size);
        inputBox.bottomLeft  /= float(outputBox.size);
        inputBox.topLeft     /= float(outputBox.size);

        if(inputBox.rotate)
        {
          auto topRight = inputBox.topRight;
          inputBox.topRight    = inputBox.bottomRight;
          inputBox.bottomRight = inputBox.bottomLeft;
          inputBox.bottomLeft  = inputBox.topLeft;
          inputBox.topLeft     = topRight;

          std::swap(inputBox.topRight,    inputBox.topLeft   );
          std::swap(inputBox.bottomRight, inputBox.bottomLeft);
        }

        outputBox.inputBoxIndexes.push_back(inputBoxIndex);
      });

      outputBoxes.push_back(outputBox);
    }
  }

  //################################################################################################
  static void render(const std::vector<InputBox>& inputBoxes,
                     const OutputBox& outputBox,
                     const std::function<const PixelType*(size_t)>& getInputData,
                     PixelType* outputData,
                     PixelType blank)
  {
    render(inputBoxes,
           outputBox,
           [&](size_t i){return [inputData = getInputData(i)](size_t j){return inputData[j];};},
           [&](size_t i)->PixelType&{return outputData[i];},
           blank);
  }

  //################################################################################################
  static void render(const std::vector<InputBox>& inputBoxes,
                     const OutputBox& outputBox,
                     const std::function<const std::function<PixelType(size_t)>(size_t)>& getInputData,
                     const std::function<PixelType&(size_t)> outputPixel,
                     PixelType blank)
  {
    {
      size_t pMax = outputBox.size*outputBox.size;
      for(size_t p=0; p<pMax; p++)
        outputPixel(p) = blank;
    }

    for(const auto& i : outputBox.inputBoxIndexes)
    {
      const auto& inputBox = inputBoxes.at(i);
      auto inputData = getInputData(i);
      auto getPixel{[&](ptrdiff_t x, ptrdiff_t y){return inputData((y*ptrdiff_t(inputBox.width))+x);}};

      ptrdiff_t p = outputBox.padding?1:0;
      ptrdiff_t xMax = ptrdiff_t(inputBox.rotatedWidth ) - (2*p);
      ptrdiff_t yMax = ptrdiff_t(inputBox.rotatedHeight) - (2*p);

      ptrdiff_t ox = ptrdiff_t(inputBox.origin.x) + p;
      ptrdiff_t oy = ptrdiff_t(inputBox.origin.y) + p;

      auto setPixel = [&](ptrdiff_t x, ptrdiff_t y, PixelType pixel)
      {
        outputPixel(((oy+y)*outputBox.size)+(ox+x)) = pixel;
      };

      auto clonePixel = [&](ptrdiff_t x, ptrdiff_t y, ptrdiff_t xFrom, ptrdiff_t yFrom)
      {
        outputPixel(((oy+y)*outputBox.size)+(ox+x)) = outputPixel(((oy+yFrom)*outputBox.size)+(ox+xFrom));
      };

      if(inputBox.rotate)
      {
        for(ptrdiff_t y=0; y<yMax; y++)
          for(ptrdiff_t x=0; x<xMax; x++)
            setPixel(x, y, getPixel(y, x));
      }
      else
      {
        for(ptrdiff_t y=0; y<yMax; y++)
          for(ptrdiff_t x=0; x<xMax; x++)
            setPixel(x, y, getPixel(x, y));
      }

      if(outputBox.padding)
      {
        clonePixel(  -1,   -1,      0,      0);
        clonePixel(xMax,   -1, xMax-1,      0);
        clonePixel(xMax, yMax, xMax-1, yMax-1);
        clonePixel(  -1, yMax,      0, yMax-1);

        for(ptrdiff_t y=0; y<yMax; y++)
        {
          clonePixel(  -1, y,      0, y);
          clonePixel(xMax, y, xMax-1, y);
        }

        for(ptrdiff_t x=0; x<xMax; x++)
        {
          clonePixel(x,   -1, x,      0);
          clonePixel(x, yMax, x, yMax-1);
        }
      }
    }
  }
};

}

#endif
