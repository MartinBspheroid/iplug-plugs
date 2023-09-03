#pragma once
#include "SA_Reorder.h"
#if IPLUG_DSP // http://bit.ly/2S64BDd
#include "SA_Processor.h"
#include "Oscillator.h"
#include "SA_AW_Interstage.h"

#include "SA_AW_PowerSag2.h"

#include "SA_AW_Diode.h"
#include "SA_TransistorVCA.h"
#include "SA_AW_Tape6.h"
#include "SA_AW_Coils.h"
#include "SA_AW_Capacitor.h"
#include "SA_AW_Vinyl.h"
#endif

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kPowerSag_Range,
  kPowerSag_Dry_Inv,
  kDummyGain,
  kDiode_Triode,
  kDiode_ClassAB,
  kDiode_ClassB,
  kDiode_DryWet,
  kTransistorVCA_Gain,
  kCoils_Saturation,
  kCoils_DC,
  kCoils_DrWet,
  kTape_InputGain,
  kTape_Soften,
  kTape_HeadBump,
  kTape_Flutter,
  kTape_OutputGain,
  kTape_DryWet,
  kCapacitor_Lowpass,
  kCapacitor_Highpass,
  kCapacitor_NonLin,
  kCapacitor_DryWet,
  kVinyl_MidHP,
  kVinyl_SideHP,
  kVinyl_HighLimit,
  kVinyl_GrooveWear,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class IPlugEffect final : public Plugin
{
public:
  IPlugEffect(const InstanceInfo& info);
  
#if IPLUG_DSP // http://bit.ly/2S64BDd

  void OnActivate(bool enable) override;
  SA_AW_Interstage interstage;
  FastSinOscillator<sample> sinOsc{ 0.0, 220.f };
  
  SA::EffectQueue eq;
  SA_AW_PowerSag2 power_Sag2;
  SA_AW_Diode diode;
  SA_TransistorVCA transistor;
  SA_AW_Coils coils;
  SA_AW_Tape6 tape;
  SA_AW_Capacitor capacitor;
  SA_AW_Vinyl vinyl;
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
private:
  
#endif
};
