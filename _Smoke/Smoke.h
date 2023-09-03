#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "SA_Smoke.h"
#include "IControls.h"
#include "SA_Math.h"
#include "Oscillator.h"

const int kNumPresets = 1;

#ifndef IPLUG_VST3
#define DEBUG
#endif // !IPLUG_VST3


#ifdef DEBUG
#include "looptester.h"
#endif

enum EParams
{
  kGain = 0,
  kOutputGain,
  kSmoke_position,
  kSmoke_size,
  kSmoke_pitch,
  kSmoke_density,
  kSmoke_texture,
  kSmoke_reverb,
  kSmoke_feedback,
  kSmoke_stereo_spread,
  kSmoke_granular_stereo_spread,
  kSmoke_overlap,
  kSmoke_window_shape,
  kSmoke_quantization,
  kSmoke_refresh_rate,
  kSmoke_phase_randomization,
  kSmoke_warp,
  kSmoke_dry_wet,
  kSmoke_playbackMode,
  kSmoke_trigger,
  kNumParams
};
enum EControlTags
{
  kCtrlTagMeterIN = 0,
  kCtrlTagMeterOUT,
  kCtrlTagRedLED,
  kCtrlTagMeterEnv,
  kNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class Smoke final : public Plugin
{
public:
  Smoke(const InstanceInfo& info);

#if IPLUG_DSP 


  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnIdle() override;
  void OnReset() override;
  float oscGain = 0;
  SA::Smoke smokeProcessor;
  IPeakSender<2> mMeterSenderOutput, mMeterSenderInput;
  IPeakSender<1> mEnvFollowerSender;

  
#endif
};
