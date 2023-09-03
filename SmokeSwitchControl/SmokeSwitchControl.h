#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "SmokeSwitchControl.h"
const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kMode1,
  kMode2,
  kMode3,
  kMode4,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class SmokeSwitchControl final : public Plugin
{
public:
  SmokeSwitchControl(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
