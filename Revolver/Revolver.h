#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"

const int kNumPresets = 1;

enum EParams
{
  kParamGain = 0,
  kParamMapX,
  kParamMapY,
  kParamChaos,
  kParamBDFill,
  kParamSNFill,
  kParamHHFill,
  kNumParams
};

#if IPLUG_DSP
#include "Topograph/Topograph.hpp"
#endif

enum EControlTags
{
  kCtrlTagMeter = 0,
  kNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class Revolver final : public Plugin
{
public:
  Revolver(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
public:
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void ProcessMidiMsg(const IMidiMsg& msg) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void OnIdle() override;
  bool OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData) override;

private:
  Topograph topo;
  float speed = 0.0001;
#endif
};
