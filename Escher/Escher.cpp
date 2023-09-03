#include "Escher.h"
#include "IPlug_include_in_plug_src.h"
#include "SA_Grid.h"
#include "metropolis/MP_Colors.h"
#include "metropolis/MP_Knob.h"
#include "metropolis/MP_Button_Gate.h"
Escher::Escher(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
#ifdef DEBUG
  dbg::loadDebugPlayer();
#endif // DEBUG
  GetParam(kGain)->InitGain("Gain");
  
  GetParam(kParamDry)->InitDouble("Dry", 1.0, 0.0, 1.0, 0.001);
  GetParam(kParamWet)->InitDouble("Wet", 0.5, 0.0, 1.0, 0.001);
  GetParam(kParamPreDelay)->InitDouble("PreDelay", 0.0, 0.0, 0.5, 0.001);
  GetParam(kParamPreDelayCVSens)->InitDouble("PreDelayCVSens", 0.5, 0.0, 1.0, 0.001);
  GetParam(kParamSize)->InitDouble("Size", 0.5, 0.0, 1.0, 0.001);
  GetParam(kParamDiffusion)->InitDouble("Diffusion",0.5, 0.0, 1.0, 0.001);
  GetParam(kParamDecay)->InitDouble("Decay", 0.5, 0.0, 0.999, 0.001);
  GetParam(kParamInputSensitivity)->InitDouble("InputSensitivity", 0.5, 0.0, 1.0, 0.001);
  GetParam(kParamInputDampLow)->InitDouble("InputDampLow", 0.5, 0.0, 10.0, 0.001);
  GetParam(kParamInputDampHigh)->InitDouble("InputDampHigh", 0.5, 0.0, 10.0, 0.001);
  GetParam(kParamReverbDampLow)->InitDouble("ReverbDampLow", 0.5, 0.0, 10.0, 0.001);
  GetParam(kParamReverbDampHigh)->InitDouble("ReverbDampHigh", 0.5, 0.0, 10.0, 0.001);
  GetParam(kParamModSpeed)->InitDouble("ModSpeed", 0.5, 0.0, 1.0, 0.001);
  GetParam(kParamModShape)->InitDouble("ModShape", 0.5, 0.0, 1.0, 0.001);
  GetParam(kParamModDepth)->InitDouble("ModDepth", 0.5, 0.0, 16.0, 0.001);
  plat.Initialize(GetSampleRate());
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, 1.);
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(MP::pallete.BG_DARK.WithOpacity(0.5));
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    pGraphics->LoadFont("Dense-Regular", DENSE_FN);
    pGraphics->LoadFont("Jost-Italic", JOST_FN);
    pGraphics->LoadFont("Brixton-Bold", BRIXTON_FN);

    IRECT b = pGraphics->GetBounds().GetPadded(-10.f);
    IRECT s = b.ReduceFromRight(50.f);
    
    const IVStyle meterStyle = DEFAULT_STYLE.WithColor(kFG, COLOR_WHITE.WithOpacity(0.3f));
    pGraphics->AttachControl(mInputMeter = new IVMeterControl<4>(b.FracRectVertical(0.1, true), "Inputs", meterStyle, EDirection::Horizontal, {"Main L", "Main R", "SideChain L", "SideChain R"}), kCtrlTagInputMeter);
    pGraphics->AttachControl(mOutputMeter = new IVMeterControl<2>(b.FracRectVertical(0.1, false), "Outputs", meterStyle, EDirection::Vertical, {"Main L", "Main R"}), kCtrlTagOutputMeter);

    auto grid = SA_Grid(b.GetOffset(0, 50, 0, 0), 5, 5).getAllCells(0.95);
    int c = 0;
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamWet, "Wet"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamDry, "Dry"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamPreDelay, "PreDelay"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamPreDelayCVSens, "PreDelayCVSens"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamSize, "Size"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamDiffusion, "Diffusion"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamDecay, "Decay"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamInputSensitivity, "InputSensitivity"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamInputDampLow, "InputDampLow"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamInputDampHigh, "InputDampHigh"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamReverbDampLow, "ReverbDampLow"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamReverbDampHigh, "ReverbDampHigh"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamModSpeed, "ModSpeed"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamModShape, "ModShape"));
    pGraphics->AttachControl(new MP::KnobControl(grid[c++], kParamModDepth, "ModDepth"));
    pGraphics->AttachControl(new IVSliderControl(s, kGain));


#ifdef DEBUG
    auto db = SA_Grid(b.GetFromTop(40).GetFromRight(100), 4, 1).getAllCells(0.8);
    int c2 = 0;
    pGraphics->AttachControl(new MP::ButtonGate(db[c2++], "O", [&]() {}, [&]() {
      dbg::tone = dbg::TestToneType::OSC;
    }));
    pGraphics->AttachControl(new MP::ButtonGate(db[c2++], "S", [&]() {}, [&]() {
      dbg::tone = dbg::TestToneType::SAMPLE;
    }));
    pGraphics->AttachControl(new MP::ButtonGate(db[c2++], "N", [&]() {}, [&]() {
      dbg::tone = dbg::TestToneType::NOISE;
    }));
    pGraphics->AttachControl(new MP::ButtonGate(db[c2++], "N2", [&]() {}, [&]() {
      dbg::tone = dbg::TestToneType::NOISE_STEREO;
    }));
#endif // DEBUG

  };

}

#if IPLUG_DSP
void Escher::OnIdle()
{
  mInputPeakSender.TransmitData(*this);
  mOutputPeakSender.TransmitData(*this);
  
  if(mSendUpdate)
  {
    if(GetUI())
    {
      mInputMeter->SetTrackName(0, mInputChansConnected[0] ? "Main L (Connected)" : "Main L (Not connected)");
      mInputMeter->SetTrackName(1, mInputChansConnected[1] ? "Main R (Connected)" : "Main R (Not connected)");
      mInputMeter->SetTrackName(2, mInputChansConnected[2] ? "SideChain L (Connected)" : "SideChain L (Not connected)");
      mInputMeter->SetTrackName(3, mInputChansConnected[3] ? "SideChain R (Connected)" : "SideChain R (Not connected)");
      
      mOutputMeter->SetTrackName(0, mOutputChansConnected[0] ? "Main L (Connected)" : "Main L (Not connected)");
      mOutputMeter->SetTrackName(1, mOutputChansConnected[1] ? "Main R (Connected)" : "Main R (Not connected)");
      
      GetUI()->SetAllControlsDirty();
    }
    mSendUpdate = false;
  }
}

void Escher::GetBusName(ERoute direction, int busIdx, int nBuses, WDL_String& str) const
{
  //could customize bus names here
  IPlugProcessor::GetBusName(direction, busIdx, nBuses, str);
}

void Escher::OnActivate(bool enable)
{
  mSendUpdate = true;
}

void Escher::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->DBToAmp();
  const int nChans = NOutChansConnected();
  //for (int i=0; i < 4; i++) {
  //  bool connected = IsChannelConnected(ERoute::kInput, i);
  //  if(connected != mInputChansConnected[i]) {
  //    mInputChansConnected[i] = connected;
  //    mSendUpdate = true;
  //  }
  //}
  //
  //for (int i=0; i < 2; i++) {
  //  bool connected = IsChannelConnected(ERoute::kOutput, i);
  //  if(connected != mOutputChansConnected[i]) {
  //    mOutputChansConnected[i] = connected;
  //    mSendUpdate = true;
  //  }
  //}
#ifdef DEBUG
  dbg::ProcessBlock(inputs, outputs, nFrames);
#endif
  for (int s = 0; s < nFrames; s++) {

    plat.wet = GetParam(kParamWet)->Value();
    plat.dry = GetParam(kParamDry)->Value();
    plat.preDelay = GetParam(kParamPreDelay)->Value();
    plat.preDelayCVSens = GetParam(kParamPreDelayCVSens)->Value();
    plat.size = GetParam(kParamSize)->Value();
    plat.diffusion = GetParam(kParamDiffusion)->Value();
    plat.decay = GetParam(kParamDecay)->Value();
    plat.inputSensitivity = GetParam(kParamInputSensitivity)->Value();
    plat.inputDampLow = GetParam(kParamInputDampLow)->Value();
    plat.inputDampHigh = GetParam(kParamInputDampHigh)->Value();
    plat.reverbDampLow = GetParam(kParamReverbDampLow)->Value();
    plat.reverbDampHigh = GetParam(kParamReverbDampHigh)->Value();
    plat.modSpeed = GetParam(kParamModSpeed)->Value();
    plat.modShape = GetParam(kParamModShape)->Value();
    plat.modDepth = GetParam(kParamModDepth)->Value();

    sample frameIn[2] = { inputs[0][s], inputs[1][s] };
    sample frameOut[2] = { 0, 0};
    
    plat.process(frameIn, frameOut );
    outputs[0][s] = frameOut[0] * gain;
    outputs[1][s] = frameOut[1] * gain;
  }
  



  mInputPeakSender.ProcessBlock(inputs, nFrames, kCtrlTagInputMeter, 4, 0);
  mOutputPeakSender.ProcessBlock(outputs, nFrames, kCtrlTagOutputMeter, 2, 0);
}
#endif
