#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "SA_Collider.h"
const int kNumPresets = 1;

enum EParams
{
  kParamGain = 0,
  kContour_param,
  kBow_param,
  kBlow_param,
  kStrike_param,
  kCoarse_param,
  kFine_param,
  kFm_param,

  kFlow_param,
  kMallet_param,
  kGeometry_param,
  kBrightness_param,

  kBow_timbre_param,
  kBlow_timbre_param,
  kStrike_timbre_param,
  kDamping_param,
  kPosition_param,
  kSpace_param,
  kNumParams
};

#if IPLUG_DSP

#endif

enum EControlTags
{
  kCtrlTagMeter = 0,
  kCtrlTagKeyboard,
  kNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class Collider final : public Plugin
{
public:
  Collider(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
public:
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void ProcessMidiMsg(const IMidiMsg& msg) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void OnIdle() override;
  bool OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData) override;

private:
  
  IPeakSender<2> mMeterSender;
  //IVMeterControl<2> mMeter;
  SA::SA_Collider mCollider;
#endif
};
