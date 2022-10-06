#include "tp_math_utils/Globals.h"
#include <limits>
namespace tp_math_utils
{

//##################################################################################################
bool isPowerOf2(size_t v)
{
  return v && (powerOf2(v) == v);
}

//##################################################################################################
size_t powerOf2(size_t v)
{
  v--;
  v |= v >> 1;
  v |= v >> 2;
  v |= v >> 4;
  v |= v >> 8;
  if constexpr(sizeof(size_t)>= 4) v |= v >> 16; // sizeof(size_t) >=  32bit
  if constexpr(sizeof(size_t)>= 8) v |= v >> 32; // sizeof(size_t) >=  64bit

#ifndef TP_WIN32_MSVC
  if constexpr(sizeof(size_t)>=16) v |= v >> 64; // sizeof(size_t) >= 128bit
#endif

  v++;
  return v;
}

//##################################################################################################
size_t closestPowerOf2(size_t v)
{
  size_t vUp = powerOf2(v);
  size_t vDn = vUp >> 1;
  return ((vUp-v) < (v-vDn))?vUp:vDn;
}

namespace hsvConvert
{
// See https://stackoverflow.com/a/6930407

typedef struct {
    double r;       // a fraction between 0 and 1
    double g;       // a fraction between 0 and 1
    double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;


static hsv   rgbToHsv(rgb in);
static rgb   hsvToRgb(hsv in);

////##################################################################################################
hsv rgbToHsv(rgb in)
{
    hsv         out;
    double      min, max, delta;

    min = in.r < in.g ? in.r : in.g;
    min = min  < in.b ? min  : in.b;

    max = in.r > in.g ? in.r : in.g;
    max = max  > in.b ? max  : in.b;

    out.v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.s = 0;
        out.h = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        out.s = 0.0;
        out.h = NAN;                            // its now undefined
        return out;
    }
    if( in.r >= max )                           // > is bogus, just keeps compilor happy
        out.h = ( in.g - in.b ) / delta;        // between yellow & magenta
    else
    if( in.g >= max )
        out.h = 2.0 + ( in.b - in.r ) / delta;  // between cyan & yellow
    else
        out.h = 4.0 + ( in.r - in.g ) / delta;  // between magenta & cyan

    out.h *= 60.0;                              // degrees

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}

////##################################################################################################
rgb hsvToRgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}
}

//##################################################################################################
glm::vec3 rgb2hsv(const glm::vec3& rgb)
{
  hsvConvert::rgb rgbIn{ rgb.x, rgb.y, rgb.z };
  hsvConvert::hsv result;
  result = hsvConvert::rgbToHsv(rgbIn);
  glm::vec3 hsv{result.h / 360.0f, result.s, result.v};

  return hsv;
}

//##################################################################################################
glm::vec3 hsv2rgb(const glm::vec3& hsv)
{
  hsvConvert::hsv hsvIn{ hsv.x * 360.0f, hsv.y, hsv.z };
  hsvConvert::rgb result;
  result = hsvConvert::hsvToRgb(hsvIn);
  glm::vec3 rgb{result.r, result.g, result.b};

  return rgb;
}

}
