#include "SmokeSwitchControl.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "SmokeSwitch.h"
#include "SA_Grid.h"

SmokeSwitchControl::SmokeSwitchControl(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");
  GetParam(kMode1)->InitDouble("MODE", 0., 0., 1.0, 0.01);

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(MP::pallete.BG_DARK);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    pGraphics->LoadFont("Dense-Regular", DENSE_FN);
    pGraphics->LoadFont("Jost-Italic", JOST_FN);
    pGraphics->LoadFont("Brixton-Bold", BRIXTON_FN);


    const IRECT b = pGraphics->GetBounds();
    auto grid = SA_Grid(b, 1, { .35, .5, .05, .1 }).getAllCells();
    int c = 0;
    pGraphics->AttachControl(new MP::Switch(grid[c++].GetFromLeft(300), kMode1));
    pGraphics->AttachControl(new MP::Switch(grid[c++], kMode2));
    pGraphics->AttachControl(new MP::Switch(grid[c++], kMode3));
    pGraphics->AttachControl(new IVButtonControl(grid[c++].GetFromRight(200), [&](IControl* c) {
      SplashClickActionFunc(c);
      
      
      DBGMSG(std::to_string((int)(GetParam(kMode1)->Value() * 4.0)).c_str());

    }));

  };
#endif
}

#if IPLUG_DSP
void SmokeSwitchControl::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s] * gain;
    }
  }
}
#endif
