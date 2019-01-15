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
          tpDebug() << "currentY: " << currentY;
          if(sortedBoxes.empty())
            break;

          tallest = 0;

          for(size_t currentX=0; currentX<outputSize;)
          {
            tpDebug() << "currentX: " << currentX;
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

//          auto topLeft = inputBox.topLeft;
//          inputBox.topLeft     = inputBox.bottomLeft;
//          inputBox.bottomLeft  = inputBox.bottomRight;
//          inputBox.bottomRight = inputBox.topRight;
//          inputBox.topRight    = topLeft;
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
    {
      auto p = outputData;
      auto pMax = p+(outputBox.size*outputBox.size);
      for(; p<pMax; p++)
        (*p) = blank;
    }

    for(const auto& i : outputBox.inputBoxIndexes)
    {
      const auto& inputBox = inputBoxes.at(i);
      auto inputData = getInputData(i);
      auto getPixel{[&](size_t x, size_t y){return inputData[(y*inputBox.width)+x];}};

      size_t p = outputBox.padding?1:0;
      size_t xMax = inputBox.rotatedWidth  - (2*p);
      size_t yMax = inputBox.rotatedHeight - (2*p);

      size_t ox = size_t(inputBox.origin.x) + p;
      size_t oy = size_t(inputBox.origin.y) + p;

      auto setPixel = [&](size_t x, size_t y, PixelType pixel)
      {
        outputData[((oy+y)*outputBox.size)+(ox+x)] = pixel;
      };

      if(inputBox.rotate)
      {
        for(size_t y=0; y<yMax; y++)
          for(size_t x=0; x<xMax; x++)
            setPixel(x, y, getPixel(y, x));
      }
      else
      {
        for(size_t y=0; y<yMax; y++)
          for(size_t x=0; x<xMax; x++)
            setPixel(x, y, getPixel(x, y));
      }
    }
  }
};

}

#endif
