#include "IPlugInstrument.h"
#include "IPlug_include_in_plug_src.h"
#include "LFO.h"
#include "SA_Knob.h"
#include "IGraphicsFlexBox.h"

IPlugInstrument::IPlugInstrument(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kParamGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
  GetParam(kDecay1)->InitDouble("Decay1", 1750., 1., 3000., 0.1, "ms", IParam::kFlagsNone, "");
  GetParam(kDecay2)->InitDouble("Decay2", 1250., 1., 3000., 0.1, "ms", IParam::kFlagsNone, "");
  GetParam(kDecay3)->InitDouble("Decay3", 1250., 1., 3000., 0.1, "ms", IParam::kFlagsNone, "");
  GetParam(kWaveParam)->InitInt("Wave", 0, 0, 63);
  GetParam(kAlgo)->InitInt("Algo", 1, 0, 5);
  GetParam(kFeedback)->InitDouble("Feedback", 0, 0, 1.0f, 0.01);
  GetParam(kStart)->InitDouble("StartPitch", 250, 20, 18000, 1, "hz");
  GetParam(kEnd)->InitDouble("EndPitch", 500, 20, 4, 18000, "hz");
  GetParam(kRatioIndex)->InitInt("Ratio", 0, 0, 31);

  for (size_t i = 0; i < 7; i++)
  {
    GetParam(kRatioIndex + 1 + 1)->InitDouble(ironOxide.names[i].c_str(), 0.5, 0, 1.0, 0.01);
  }


  psi.setSampletime(GetSampleRate());
  ironOxide.sampleRate = GetSampleRate();
#ifdef APP_API
  auto loader = SA_ConfigLoader();
  for (size_t i = 0; i < (int)kNumParams; i++)
  {
    loader.load(GetParam(i), i);
  }
#endif
#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_HEIGHT));
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_BLACK);
    pGraphics->EnableMouseOver(true);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    //pGraphics->LoadFont("Dense-Regular", DENSE_FN);
    const IRECT b = pGraphics->GetBounds().GetPadded(-20.f);
    //const IRECT lfoPanel = b.GetFromLeft(300.f).GetFromTop(200.f);
    IRECT keyboardBounds = b.GetFromBottom(300);
    IRECT wheelsBounds = keyboardBounds.ReduceFromLeft(100.f).GetPadded(-10.f);
    pGraphics->AttachControl(new IVKeyboardControl(keyboardBounds), kCtrlTagKeyboard)->Hide(true);
    const IRECT controls = b.GetGridCell(1, 2, 2);

    pGraphics->AttachControl(new IVLEDMeterControl<2>(controls.GetFromRight(100).GetPadded(-30)), kCtrlTagMeter);
    GetParam(0)->Value();

    auto knobStyle = SA_Knob::KnobStyle();
    knobStyle.size = vec2f(80, 100);
    auto textStyle = IText().WithFGColor(COLOR_RED);
    knobStyle.trackColor = COLOR_RED;
    knobStyle.style.labelText = textStyle;
    knobStyle.style.valueText = textStyle.WithVAlign(EVAlign::Bottom);
    knobStyle.textStyle = textStyle.WithVAlign(EVAlign::Bottom);
    knobStyle.trackSize = 4;



    auto TopLabelStyle = DEFAULT_STYLE.WithColor(kBG, COLOR_RED).WithDrawShadows(false).WithDrawFrame(false).WithValueText(IText().WithSize(40).WithFGColor(COLOR_BLACK));



    pGraphics->AttachControl(new IVLabelControl(IRECT().MakeXYWH(0, 0, GetEditorWidth(), 50), "VINDALOOO", TopLabelStyle));

     //mainControls = IRECT().MakeXYWH(0, 50, 0,0 );
     auto mainControls = b.GetCentredInside(90 * 4, GetEditorHeight() * 0.7);
    IFlexBox fb;
    fb.Init(mainControls, YGFlexDirectionRow, YGJustifyFlexStart, YGWrapWrap, 0, 0);
    for (size_t i = 0; i < 16; i++)
    {
      fb.AddItem(IRECT().MakeXYWH(0, 0, 80, 100), YGAlign::YGAlignFlexStart, 1, 1);

    }
    fb.CalcLayout();
    std::vector<IRECT> co = fb.GetItemBounds();
    int pos = 0;
    
    pGraphics->AttachControl(new SA_Knob(co[pos++].GetVShifted(-30), kAlgo, knobStyle, "ALGO"));
    pGraphics->AttachControl(new SA_Knob(co[pos++].GetVShifted(-30), kRatioIndex, knobStyle, "RATIO"));
    pGraphics->AttachControl(new SA_Knob(co[pos++].GetVShifted(-30), kWaveParam, knobStyle, "WAVE"));
    pGraphics->AttachControl(new SA_Knob(co[pos++].GetVShifted(-30), kFeedback, knobStyle, "FEEDBACK"));


    pGraphics->AttachControl(new SA_Knob(co[pos++], kStart, knobStyle, "START"));
    pGraphics->AttachControl(new SA_Knob(co[pos++], kAttack3, knobStyle, "PITCH ATT"));
    pGraphics->AttachControl(new SA_Knob(co[pos++], kAttack1, knobStyle, "AMP1 ATT"));
    pGraphics->AttachControl(new SA_Knob(co[pos++], kAttack2, knobStyle, "AMP2 ATT"));


    pGraphics->AttachControl(new SA_Knob(co[pos++], kEnd, knobStyle, "END"));
    pGraphics->AttachControl(new SA_Knob(co[pos++], kDecay3, knobStyle, "PITCH DEC"));
    pGraphics->AttachControl(new SA_Knob(co[pos++], kDecay1, knobStyle, "AMP1 DEC"));
    pGraphics->AttachControl(new SA_Knob(co[pos++], kDecay2, knobStyle, "AMP2 DEC"));

    pos++;// skip one cell
    pGraphics->AttachControl(new SA_Knob(co[pos++], kPitchEnvDecayShape, knobStyle, "PitchSHAPE"));
    pGraphics->AttachControl(new SA_Knob(co[pos++], kShape1, knobStyle, "SHAPE 1"));
    pGraphics->AttachControl(new SA_Knob(co[pos++], kShape2, knobStyle, "SHAPE 2"));
    

    auto bottomControls = b.GetFromBottom(50).GetVShifted(-25);
    IFlexBox fbBottom;
    fbBottom.Init(bottomControls, YGFlexDirectionRow, YGJustifyFlexStart, YGWrapNoWrap, 0, 0);
    for (size_t i = 0; i < 7; i++)
    {
      fbBottom.AddItem(IRECT().MakeXYWH(0, 0, 70, 100), YGAlign::YGAlignFlexStart, 1, 1);

    }
    fbBottom.CalcLayout();
    std::vector<IRECT> coBottom = fbBottom.GetItemBounds();
    int posBottom = 0;

    auto getXY = [this](const IRECT& ir) {return vec2f(ir.L, ir.T); };
    knobStyle.size = vec2f(66, 66);
    
    knobStyle.trackSize = 1;
    pGraphics->AttachControl(new SA_Knob(getXY(coBottom[posBottom++]), kIO_Trim, knobStyle, "TRIM"));
    pGraphics->AttachControl(new SA_Knob(getXY(coBottom[posBottom++]), kIO_LOW_SPEED_IPS, knobStyle, "LOW_IPS"));
    pGraphics->AttachControl(new SA_Knob(getXY(coBottom[posBottom++]), kIO_HIGH_SPEED_IPS, knobStyle, "HIGH_IPS"));
    pGraphics->AttachControl(new SA_Knob(getXY(coBottom[posBottom++]), kIO_FLUTTER, knobStyle, "FLUTTER"));
    pGraphics->AttachControl(new SA_Knob(getXY(coBottom[posBottom++]), kIO_NOISE, knobStyle, "NOISE"));
    pGraphics->AttachControl(new SA_Knob(getXY(coBottom[posBottom++]), kIO_OUTPUT_TRIM, knobStyle, "OUTPUT"));
    pGraphics->AttachControl(new SA_Knob(getXY(coBottom[posBottom++]), kIO_DRY_WET, knobStyle, "DRY/WET"));
    


    pGraphics->SetQwertyMidiKeyHandlerFunc([pGraphics](const IMidiMsg& msg) {
      dynamic_cast<IVKeyboardControl*>(pGraphics->GetControlWithTag(kCtrlTagKeyboard))->SetNoteFromMidi(msg.NoteNumber(), msg.StatusMsg() == IMidiMsg::kNoteOn);
    });
  };
#endif
}

#if IPLUG_DSP
void IPlugInstrument::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  //mDSP.ProcessBlock(nullptr, outputs, 2, nFrames, mTimeInfo.mPPQPos, mTimeInfo.mTransportIsRunning);
  mMeterSender.ProcessBlock(outputs, nFrames, kCtrlTagMeter);
  const int nChans = NOutChansConnected();
  psi.ampEnvDecay1 = math::rescale(GetParam(kDecay1)->Value(), 0, 3000, 0, 1);
  psi.ampEnvDecay2 = math::rescale(GetParam(kDecay2)->Value(), 0, 3000, 0, 1);
  psi.pitchEnvDecay = math::rescale(GetParam(kDecay3)->Value(), 0, 3000, 0, 1);
  psi.table = GetParam(kWaveParam)->Value();
  psi.algo = GetParam(kAlgo)->Value();
  psi.feedback = GetParam(kFeedback)->Value();

  psi.startPitch = math::rescale(GetParam(kStart)->Value(), 20, 18000, -5, 4);
  psi.endPitch = math::rescale(GetParam(kEnd)->Value(), 20, 18000, -5, 4);
  psi.ratioIndex = GetParam(kRatioIndex)->Value();



  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = psi.process();

    }
    //mLFOVisSender.PushData({kCtrlTagLFOVis, {float(mDSP.mLFO.GetLastOutput())}});
  }
  ironOxide.processDoubleReplacing(outputs, outputs, nFrames);

}
void IPlugInstrument::OnIdle()
{
  mMeterSender.TransmitData(*this);
  //mLFOVisSender.TransmitData(*this);

  ironOxide.A = GetParam(kIO_Trim)->Value();
  ironOxide.B = GetParam(kIO_HIGH_SPEED_IPS)->Value();
  ironOxide.C = GetParam(kIO_LOW_SPEED_IPS)->Value();
  ironOxide.D = GetParam(kIO_FLUTTER)->Value();
  ironOxide.E = GetParam(kIO_NOISE)->Value();
  ironOxide.F = GetParam(kIO_OUTPUT_TRIM)->Value();
  ironOxide.G = GetParam(kIO_DRY_WET)->Value();
  psi.Attack1 = GetParam(kAttack1)->Value();
  psi.Attack2 = GetParam(kAttack2)->Value();
  psi.Attack3 = GetParam(kAttack3)->Value();
  psi.Shape1 = GetParam(kShape1)->Value();
  psi.Shape2 = GetParam(kShape2)->Value();
  psi.PitchEnvDecayShape = GetParam(kPitchEnvDecayShape)->Value();

}

void IPlugInstrument::OnReset()
{
  //mDSP.Reset(GetSampleRate(), GetBlockSize());
}

void IPlugInstrument::ProcessMidiMsg(const IMidiMsg& msg)
{
  TRACE;

  int status = msg.StatusMsg();

  switch (status)
  {
  case IMidiMsg::kNoteOn:
    psi.fakeTrigger = 1;
    break;
  case IMidiMsg::kNoteOff:
    psi.fakeTrigger = 0;
    break;
  case IMidiMsg::kPolyAftertouch:
  case IMidiMsg::kControlChange:
  case IMidiMsg::kProgramChange:
  case IMidiMsg::kChannelAftertouch:
  case IMidiMsg::kPitchWheel:
  {
    goto handle;
  }
  default:
    return;
  }

handle:
  //mDSP.ProcessMidiMsg(msg);
  SendMidiMsg(msg);
}

void IPlugInstrument::OnParamChange(int paramIdx)
{
  //mDSP.SetParam(paramIdx, GetParam(paramIdx)->Value());
}

bool IPlugInstrument::OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData)
{
  if (ctrlTag == (int)kCtrlTagBender && msgTag == IWheelControl::kMessageTagSetPitchBendRange)
  {
    const int bendRange = *static_cast<const int*>(pData);
    //mDSP.mSynth.SetPitchBendRange(bendRange);
  }

  return false;
}
#endif
