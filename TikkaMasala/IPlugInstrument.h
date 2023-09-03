#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "IVMultiSliderControl.h"
#include "IControls.h"
#include "IPlugMidi.h"
#include <functional>
#include "Tikka_Seq.h"
#include "Tikka_Controls.h"

const int kNumPresets = 1;

enum EParams
{
  kParamGain = 0,
  kOneLength,
  kOneBeats,
  kOneOffset,
  kCtrlMultiSlider,
  kNumParams
};

#if IPLUG_DSP
// will use EParams in IPlugInstrument_DSP.h
#include "IPlugInstrument_DSP.h"
#endif


enum EMsgTags
{
  kMsgTagSliderChanged = 0
};

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
  std::vector<IColor> colors = { IColor(255, 40, 72, 85), IColor(255, 42, 157, 143), IColor(255, 233, 196, 106), IColor(255, 244, 162, 97), IColor(255, 231, 111, 81) };
  std::vector<EuqSeq> tracks;
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void ProcessMidiMsg(const IMidiMsg& msg) override;
  void OnReset() override;
  void OnParamChange(int paramIdx) override;
  void OnIdle() override;
  bool OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData) override;
  

private:
  ITextControl* ppqTxt;

#ifdef APP_API
  double time = 0;
#endif
#endif
};
