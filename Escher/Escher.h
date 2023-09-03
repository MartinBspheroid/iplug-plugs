#pragma once
//#define DEBUG 1 
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "IPlug_include_in_plug_hdr.h"
#include "ISender.h"
#include "IControls.h"
#include "Plateau/Plateau.hpp"


#ifdef DEBUG
#include "debug/looptester.h"
#endif
const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kParamWet,
  kParamDry,
  kParamPreDelay,
  kParamPreDelayCVSens,
  kParamSize,
  kParamDiffusion,
  kParamDecay,
  kParamInputSensitivity,
  kParamInputDampLow,
  kParamInputDampHigh,
  kParamReverbDampLow,
  kParamReverbDampHigh,
  kParamModSpeed,
  kParamModShape,
  kParamModDepth,
  kNumParams
};

enum ECtrlTags
{
  kCtrlTagInputMeter = 0,
  kCtrlTagOutputMeter,
  kNumCtrlTags
};

enum EMsgTags
{
  kMsgTagConnectionsChanged = 0,
  kNumMsgTags
};

using namespace iplug;
using namespace igraphics;

class Escher final : public Plugin
{
public:
  Escher(const InstanceInfo& info);

  void OnIdle() override;
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnActivate(bool enable) override;
  
  void GetBusName(ERoute direction, int busIdx, int nBuses, WDL_String& str) const override;

  bool mInputChansConnected[4] = {};
  bool mOutputChansConnected[2] = {};
  bool mSendUpdate = false;
  Plateau plat;
  IPeakSender<4> mInputPeakSender;
  IPeakSender<2> mOutputPeakSender;
  IVMeterControl<4>* mInputMeter = nullptr;
  IVMeterControl<2>* mOutputMeter = nullptr;
};
