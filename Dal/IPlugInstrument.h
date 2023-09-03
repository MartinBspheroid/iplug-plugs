#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"
#include "externals/PSIOP.hpp"
#include <SA_Config.h>
#include "SA_AW_IronOxide.h"

const int kNumPresets = 1;

enum EParams
{
  kParamGain = 0,
  kDecay1,
  kDecay2,
  kDecay3,
  kWaveParam,
  kAlgo,
  kFeedback,
  kStart,
  kEnd,
  kRatioIndex,
  kIO_Trim,
  kIO_HIGH_SPEED_IPS,
  kIO_LOW_SPEED_IPS,
  kIO_FLUTTER,
  kIO_NOISE,
  kIO_OUTPUT_TRIM,
  kIO_DRY_WET,
  kAttack1,
  kAttack2,
  kAttack3,
  kShape1,
  kShape2,
  kPitchEnvDecayShape,

  kNumParams

};

#if IPLUG_DSP
// will use EParams in IPlugInstrument_DSP.h
//#include "IPlugInstrument_DSP.h"
#endif

enum EControlTags
{
  kCtrlTagMeter = 0,
  kCtrlTagLFOVis,
  kCtrlTagScope,
  kCtrlTagRTText,
  kCtrlTagKeyboard,
  kCtrlTagBender,
  kNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class IPlugInstrument final : public Plugin
{
public:
  IPlugInstrument(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
public:
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void ProcessMidiMsg(const IMidiMsg& msg) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void OnIdle() override;
  bool OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData) override;
  void OnUIClose() override {
#ifdef APP_API
    SA_ConfigSaver saver = SA_ConfigSaver();
    for (int  i = 0; i < (int)kNumParams ; i++)
    {
      saver.save(GetParam(i));
    }
#endif
  };
private:
  /*IPlugInstrumentDSP<sample> mDSP {16};*/
  SA_AW_IronOxide ironOxide;
  IPeakSender<2> mMeterSender;
  //ISender<1> mLFOVisSender;
  PSIOP psi;
#endif
};
