#include "Collider.h"
#include "IPlug_include_in_plug_src.h"
#include "SA_Grid.h"
#include "metropolis/MP_Colors.h"
#include "metropolis/MP_Knob.h"
#include "metropolis/MP_Section.h"
Collider::Collider(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kParamGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
  GetParam(kContour_param)->InitDouble("Contour", 1., 0., 1.0, 0.01, "");
  GetParam(kBow_param)->InitDouble("Bow", 0., 0., 1.0, 0.01, "");
  GetParam(kBlow_param)->InitDouble("Blow", 0., 0., 1.0, 0.01, "");
  GetParam(kStrike_param)->InitDouble("Strike", 0.5, 0., 1.0, 0.01, "");
  GetParam(kCoarse_param)->InitDouble("Coarse", 0.f, -30.f, 30.f, 0.01, "");
  GetParam(kFine_param)->InitDouble("Fine", 0., -2.f, 2.f, 0.01, "");
  GetParam(kFm_param)->InitDouble("Fm", 0., -1.f, 1.0, 0.01, "");
  GetParam(kFlow_param)->InitDouble("Flow", 0.5, 0., 1.0, 0.01, "");
  GetParam(kMallet_param)->InitDouble("Mallet", 0.5, 0., 1.0, 0.01, "");
  GetParam(kGeometry_param)->InitDouble("Geometry", 0.5, 0., 1.0, 0.01, "");
  GetParam(kBrightness_param)->InitDouble("Brightness", 0.5, 0., 1.0, 0.01, "");
  GetParam(kBow_timbre_param)->InitDouble("Bow_timbre", 0.5, 0., 1.0, 0.01, "");
  GetParam(kBlow_timbre_param)->InitDouble("Blow_timbre", 0.5, 0., 1.0, 0.01, "");
  GetParam(kStrike_timbre_param)->InitDouble("Strike_timbre", 0.5, 0., 1.0, 0.01, "");
  GetParam(kDamping_param)->InitDouble("Damping", 0.5, 0., 1.0, 0.01, "");
  GetParam(kPosition_param)->InitDouble("Position", 0.5, 0., 1.0, 0.01, "");
  GetParam(kSpace_param)->InitDouble("Space", 0.06, 0.06, 2.0, 0.01, "");

  auto p = mCollider.getParams();
  p->exciter_envelope_shape = GetParam(kContour_param)->Value();
  p->exciter_bow_level = GetParam(kBow_param)->Value();
  p->exciter_blow_level = GetParam(kBlow_param)->Value();
  p->exciter_strike_level = GetParam(kStrike_param)->Value();

  p->exciter_bow_timbre = GetParam(kBlow_timbre_param)->Value();
  p->exciter_blow_meta = GetParam(kFlow_param)->Value();
  p->exciter_blow_timbre = GetParam(kBlow_timbre_param)->Value();
  p->exciter_strike_meta = GetParam(kMallet_param)->Value();
  p->exciter_strike_timbre = GetParam(kStrike_timbre_param)->Value();

  p->resonator_geometry = GetParam(kGeometry_param)->Value();
  p->resonator_brightness = GetParam(kBrightness_param)->Value();
  p->resonator_damping = GetParam(kDamping_param)->Value();
  p->resonator_position = GetParam(kPosition_param)->Value();

  mCollider.getParams()->space = GetParam(kSpace_param)->Value();
  auto pr = mCollider.getPerformanceState();
  pr->modulation = 0.5; //fm??/

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(MP::pallete.BG_DARK);
    pGraphics->EnableMouseOver(true);
    


    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    pGraphics->LoadFont("Dense-Regular", DENSE_FN);
    pGraphics->LoadFont("Jost-Italic", JOST_FN);
    pGraphics->LoadFont("Brixton-Bold", BRIXTON_FN);

    const IRECT b = pGraphics->GetBounds().GetPadded(-20.f).GetReducedFromTop(20);
    pGraphics->AttachControl(new IVKeyboardControl(IRECT()), kCtrlTagKeyboard);

    pGraphics->AttachControl(new MP::Section(pGraphics->GetBounds().GetFromTop(40), "C O L L I D E R"));

    auto grid = SA_Grid(b.GetPadded(-30), 3, 7);

    int c = 0;
    int ct = -30; // center translation
    auto row1 = grid.getRowCells(0);
    MP::ColorScheme blowParamColors = { MP::pallete.ACC_GOLD, MP::pallete.ACC_GOLD.WithOpacity(0.5), MP::pallete.BG_KNOB };
    MP::ColorScheme strikeParamColors = { MP::pallete.ACC_BLUE, MP::pallete.ACC_BLUE.WithOpacity(0.5), MP::pallete.BG_KNOB };

pGraphics->AttachControl(new MP::KnobControl(row1[0], kStrike_param, "Strike", strikeParamColors));
pGraphics->AttachControl(new MP::KnobControl(row1[1].GetTranslated(0, ct), kContour_param, "Contour"));
pGraphics->AttachControl(new MP::KnobControl(row1[2], kBlow_param, "Blow", blowParamColors));

auto row2 = grid.getRowCells(1);
pGraphics->AttachControl(new MP::KnobControl(row2[0], kMallet_param, "Mallet", strikeParamColors));
pGraphics->AttachControl(new MP::KnobControl(row2[1].GetTranslated(0, ct), kBow_param, "Bow"));
pGraphics->AttachControl(new MP::KnobControl(row2[2], kFlow_param, "Flow", blowParamColors));

auto row3 = grid.getRowCells(2);
pGraphics->AttachControl(new MP::KnobControl(row3[0], kStrike_timbre_param, "Strike_timbre", strikeParamColors));
pGraphics->AttachControl(new MP::KnobControl(row3[1].GetTranslated(0, ct), kBow_timbre_param, "Bow_timbre"));
pGraphics->AttachControl(new MP::KnobControl(row3[2], kBlow_timbre_param, "Blow_timbre", blowParamColors));



//pGraphics->AttachControl(new MP::KnobControl(g[c++], kCoarse_param, "Coarse"));
//
//pGraphics->AttachControl(new MP::KnobControl(g[c++], kFine_param, "Fine"));
//
//pGraphics->AttachControl(new MP::KnobControl(g[c++], kFm_param, "Fm"));
pGraphics->AttachControl(new MP::KnobControl(grid.getRowCells(3)[1].GetTranslated(0, 0).GetScaledAboutCentre(1.3), kGeometry_param, "Geometry"));

auto row5 = grid.getRowCells(4);

pGraphics->AttachControl(new MP::KnobControl(row5[0], kDamping_param, "Damping"));
pGraphics->AttachControl(new MP::KnobControl(row5[1].GetTranslated(0, -ct), kPosition_param, "Position"));
pGraphics->AttachControl(new MP::KnobControl(row5[2], kBrightness_param, "Brightness"));

pGraphics->AttachControl(new MP::KnobControl(grid.getRowCells(5)[1].GetTranslated(0, -ct), kSpace_param, "Space"))->SetActionFunction([&](IControl* c) {
  
});

auto lastRow = grid.getRowCells(6);
pGraphics->AttachControl(new MP::KnobControl(lastRow[0].GetScaledAboutCentre(0.7), kNoTag, "Model"))->SetActionFunction([&](IControl* c) {
  int model = rack::math::rescale((int)(c->GetValue() * 3), 0, 3, 0, 2);
  mCollider.setModel(model);

  DBGMSG(std::to_string(model).c_str());
});
pGraphics->AttachControl(new MP::KnobControl(lastRow[2].GetScaledAboutCentre(0.7), kParamGain, "ParamGain"));



    pGraphics->AttachControl(new IVMeterControl<2>(b.GetFromRight(10), "out", DEFAULT_STYLE, EDirection::Vertical), kCtrlTagMeter);
    
    
    pGraphics->SetQwertyMidiKeyHandlerFunc([pGraphics](const IMidiMsg& msg) {
                                              pGraphics->GetControlWithTag(kCtrlTagKeyboard)->As<IVKeyboardControl>()->SetNoteFromMidi(msg.NoteNumber(), msg.StatusMsg() == IMidiMsg::kNoteOn);
                                           });
  };
#endif
}

#if IPLUG_DSP
void Collider::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kParamGain)->Value() / 100.;
  const int nChans = NOutChansConnected();

  auto p = mCollider.getParams();
  p->exciter_envelope_shape = GetParam(kContour_param)->Value();
  p->exciter_bow_level = GetParam(kBow_param)->Value();
  p->exciter_blow_level = GetParam(kBlow_param)->Value();
  p->exciter_strike_level = GetParam(kStrike_param)->Value();

  p->exciter_bow_timbre = GetParam(kBlow_timbre_param)->Value();
  p->exciter_blow_meta = GetParam(kFlow_param)->Value();
  p->exciter_blow_timbre = GetParam(kBlow_timbre_param)->Value();
  p->exciter_strike_meta = GetParam(kMallet_param)->Value();
  p->exciter_strike_timbre = GetParam(kStrike_timbre_param)->Value();

  p->resonator_geometry = GetParam(kGeometry_param)->Value();
  p->resonator_brightness = GetParam(kBrightness_param)->Value();
  p->resonator_damping = GetParam(kDamping_param)->Value();
  p->resonator_position = GetParam(kPosition_param)->Value();

  mCollider.getParams()->space = GetParam(kSpace_param)->Value();
  auto pr = mCollider.getPerformanceState();
  pr->modulation = 0.5; //fm??/

  mCollider.process(outputs, nFrames);

  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = outputs[c][s] * gain;
    }
  }
  mMeterSender.ProcessBlock(outputs, nFrames, kCtrlTagMeter);
}

void Collider::OnIdle()
{
  mMeterSender.TransmitData(*this);
}

void Collider::OnReset()
{
}

void Collider::ProcessMidiMsg(const IMidiMsg& msg)
{
  TRACE;
  
  int status = msg.StatusMsg();
  
  switch (status)
  {
    case IMidiMsg::kNoteOn:
      mCollider.getPerformanceState()->gate = 1.0f;
      mCollider.getPerformanceState()->note = msg.NoteNumber()-32;
      mCollider.getPerformanceState()->strength = SA::rescale((float)msg.Velocity(), 0.f, 127.f, 0.f, 1.f);
      break;
    case IMidiMsg::kNoteOff:
      mCollider.getPerformanceState()->gate = 0.0f;
      break;
    case IMidiMsg::kPolyAftertouch:
    case IMidiMsg::kControlChange:
    case IMidiMsg::kProgramChange:
    case IMidiMsg::kChannelAftertouch:
    case IMidiMsg::kPitchWheel:
    {
      
    }
    default:
      return;
  }
  
}

void Collider::OnParamChange(int paramIdx)
{
}

bool Collider::OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData)
{
  return true;
}
#endif
