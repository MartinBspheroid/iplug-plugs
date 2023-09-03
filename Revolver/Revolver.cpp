#include "Revolver.h"
#include "IPlug_include_in_plug_src.h"
#include "LFO.h"
#include "SA_Grid.h"

Revolver::Revolver(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kParamGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
  GetParam(kParamMapX)->InitDouble("MapX", 0.5, 0., 1.0, 0.001, "");
  GetParam(kParamMapY)->InitDouble("MapY", 0.5, 0., 1.0, 0.001, "");
  GetParam(kParamChaos)->InitDouble("Chaos", 0.5, 0., 1.0, 0.001, "");
  GetParam(kParamBDFill)->InitDouble("BDFill", 0.5, 0., 1.0, 0.001, "");
  GetParam(kParamSNFill)->InitDouble("SNFill", 0.5, 0., 1.0, 0.001, "");
  GetParam(kParamHHFill)->InitDouble("HHFill", 0.5, 0., 1.0, 0.001, "");

  

  topo.Initialize(GetSampleRate());
  topo.sendMidiMsg = [&](IMidiMsg m) {
    return SendMidiMsg(m);
  };
  topo.mapX = GetParam(kParamMapX)->Value();
  topo.mapY = GetParam(kParamMapY)->Value();
  topo.chaos = GetParam(kParamChaos)->Value();
  topo.BDFill = GetParam(kParamBDFill)->Value();
  topo.SNFill = GetParam(kParamSNFill)->Value();
  topo.HHFill = GetParam(kParamHHFill)->Value();

  
#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->EnableMouseOver(true);
    pGraphics->EnableMultiTouch(true);



    //    pGraphics->EnableLiveEdit(true);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds().GetPadded(-20.f);
    auto grid = SA_Grid(b.GetPadded(0), 3, 3).getAllCells(0.95);
    int c = 0;

    pGraphics->AttachControl(new IVKnobControl(grid[c++], kParamMapX, "11MapX"))->SetActionFunction([&](IControl* c) {topo.mapX = c->GetValue(); });
    pGraphics->AttachControl(new IVKnobControl(grid[c++], kParamMapY, "MapY"))->SetActionFunction([&](IControl* c) {topo.mapY = c->GetValue(); });
    pGraphics->AttachControl(new IVKnobControl(grid[c++], kParamChaos, "Chaos"))->SetActionFunction([&](IControl* c) {topo.chaos= c->GetValue(); });
    pGraphics->AttachControl(new IVKnobControl(grid[c++], kParamBDFill, "BDFill"))->SetActionFunction([&](IControl* c) {topo.BDFill = c->GetValue(); });
    pGraphics->AttachControl(new IVKnobControl(grid[c++], kParamSNFill, "SNFill"))->SetActionFunction([&](IControl* c) {topo.SNFill = c->GetValue(); });
    pGraphics->AttachControl(new IVKnobControl(grid[c++], kParamHHFill, "HHFill"))->SetActionFunction([&](IControl* c) {topo.HHFill = c->GetValue(); });
    pGraphics->AttachControl(new IVKnobControl(grid[c++], kNoTag, "speed"))->SetActionFunction([&](IControl* c) {speed= c->GetValue(); });


  };
#endif
}

#if IPLUG_DSP
void Revolver::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{

  //topo.setTempo(GetSamplesPerBeat());
  //topo.update(mTimeInfo.mPPQPos, nFrames, mTimeInfo.mTransportIsRunning);

#ifdef VST3_API
  for (size_t i = 0; i < nFrames; i++)
  {
    topo.update(mTimeInfo.mPPQPos, nFrames, mTimeInfo.mTransportIsRunning);
}
   
#endif

#ifdef APP_API
  for (size_t i = 0; i < nFrames; i++)
  {
    static float time = 0;
    time += speed;
    topo.update(time, nFrames, true);

  }


#endif
  
}

void Revolver::OnIdle()
{
  
}

void Revolver::OnReset()
{
}

void Revolver::ProcessMidiMsg(const IMidiMsg& msg)
{
//  TRACE;
//  
//  int status = msg.StatusMsg();
//  
//  switch (status)
//  {
//    case IMidiMsg::kNoteOn:
//    case IMidiMsg::kNoteOff:
//    case IMidiMsg::kPolyAftertouch:
//    case IMidiMsg::kControlChange:
//    case IMidiMsg::kProgramChange:
//    case IMidiMsg::kChannelAftertouch:
//    case IMidiMsg::kPitchWheel:
//    {
//      goto handle;
//    }
//    default:
//      return;
//  }
//  
//handle:
//  mDSP.ProcessMidiMsg(msg);
//  SendMidiMsg(msg);
}

void Revolver::OnParamChange(int paramIdx)
{
  
}

bool Revolver::OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData)
{
  //if(ctrlTag == kCtrlTagBender && msgTag == IWheelControl::kMessageTagSetPitchBendRange)
  //{
  //  const int bendRange = *static_cast<const int*>(pData);
  //  
  //}
  //
  return false;
}
#endif
