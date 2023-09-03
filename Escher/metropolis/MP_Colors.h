#pragma once
#include "IPlugConstants.h"

namespace MP{
struct Pallete
{
  const iplug::igraphics::IColor BG_DARK = IColor(255, 36, 37, 41);
  const iplug::igraphics::IColor BG_LABEL_DARK = IColor(255, 20, 20, 22);
  const iplug::igraphics::IColor BG_KNOB_LOW = IColor(255, 44, 47, 53);
  const iplug::igraphics::IColor BG_KNOB = IColor(255, 21, 22, 24);
  const iplug::igraphics::IColor KNOB_VALUE = IColor(255, 145, 145, 149);
  const iplug::igraphics::IColor LIGHT_GRAY = IColor(255, 125, 128, 123);
  /// accents
  const iplug::igraphics::IColor ACC_BLUE = IColor(255, 90, 118, 173);
  const iplug::igraphics::IColor ACC_GOLD = IColor(255, 192, 184, 87);
  const iplug::igraphics::IColor ACC_RED = IColor(255, 221, 39, 58);
  const iplug::igraphics::IColor ACC_GREEN = IColor(255, 87, 166, 74);
  const iplug::igraphics::IColor ACC_PURPLE = IColor(255, 189, 99, 197);


};
struct Constants
{
  const float LabelThickness = 35.f;
};

const static MP::Pallete pallete = MP::Pallete();
const static MP::Constants constants = MP::Constants();
}

