#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "Amalgam/Amalgam.hpp"

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kParamA,
  kParamB,
  kDistortionType,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class Corridor final : public Plugin
{
public:
  Corridor(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
  Amalgam amalgam;
  void ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames) override;
#endif
};
