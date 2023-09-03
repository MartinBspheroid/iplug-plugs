#include "Corridor.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "SA_Grid.h"

Corridor::Corridor(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");
  GetParam(kDistortionType)->InitDouble("Dist", 0., 0., 17.0, 1.0);
  GetParam(kParamA)->InitDouble("ParamA", 0., 0., 1.0, 0.01);
  GetParam(kParamB)->InitDouble("ParamB", 0., 0., 1.0, 0.01);
  amalgam.setSamplerate(GetSampleRate());
#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    auto grid = SA_Grid(b, 4, 4).getAllCells(.8);
    int c = 0;
    pGraphics->AttachControl(new IVKnobControl(grid[c++], kGain, "InputGain"));
    pGraphics->AttachControl(new IVKnobControl(grid[c++], kParamA, "par A"));
    pGraphics->AttachControl(new IVKnobControl(grid[c++], kParamB, "par B"));
    pGraphics->AttachControl(new IVKnobControl(grid[c++], kDistortionType, "type"));
    
    
  };
#endif
}

#if IPLUG_DSP
void Corridor::ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;
  const int nChans = NOutChansConnected();
  const double inputGain = GetParam(kGain)->Value() / 100.;
  const int dist_type = (int)GetParam(kDistortionType)->Value();
  if (dist_type != amalgam.vecAmalgam.getMode()) {
    amalgam.vecAmalgam.setMode(dist_type);

  }
  amalgam.paramA = GetParam(kParamA)->Value();
  amalgam.paramB = GetParam(kParamB)->Value();
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      amalgam.xGain = inputGain;
      amalgam.yGain = inputGain;
      
      amalgam.process(inputs[0][s], inputs[1][s], 1, 1);

      outputs[0][s] = amalgam.outputL * gain;
      outputs[1][s] = amalgam.outputR * gain;
    }
  }
  
}
#endif
