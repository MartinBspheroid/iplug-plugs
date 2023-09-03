#include "Smoke.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "SA_Grid.h"
#include "metropolis/MP_Knob.h"
#include "metropolis/MP_Section.h"
#include "metropolis/MP_Button_Gate.h"
#include "SmokeSwitch.h"
#include "SA_Meter.h"

Smoke::Smoke(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPresets))
{

 
  GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");
  GetParam(kOutputGain)->InitDouble("OutputGain", 50., 0., 100.0, 0.01, "%");
  GetParam(kSmoke_position)->InitDouble("position", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_size)->InitDouble("size", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_pitch)->InitDouble("pitch", 0.5, 0., 1.0, 0.001, "");
  GetParam(kSmoke_density)->InitDouble("density", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_texture)->InitDouble("texture", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_reverb)->InitDouble("reverb", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_feedback)->InitDouble("feedback", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_stereo_spread)->InitDouble("stereo_spread", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_overlap)->InitDouble("overlap", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_window_shape)->InitDouble("window_frame", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_quantization)->InitDouble("quantization", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_refresh_rate)->InitDouble("refresh_rate", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_phase_randomization)->InitDouble("phase_randomization", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_warp)->InitDouble("warp", 0., 0., 1.0, 0.001, "");
  GetParam(kSmoke_dry_wet)->InitDouble("dry_wet", 0.5, 0., 1.0, 0.001, "");
  GetParam(kSmoke_trigger)->InitBool("trigger", false);
  GetParam(kSmoke_playbackMode)->InitDouble("playBackMode", 0.0, 0.f, 1.f, 0.25);

#ifdef DEBUG
  dbg::loadDebugPlayer();
#endif // DEBUG


  smokeProcessor.setSampleRate(GetSampleRate());
#if IPLUG_EDITOR // ========================================================================================
  mMakeGraphicsFunc = [&]() {
    //ZoneScoped
    
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    //ZoneScoped
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(MP::pallete.BG_DARK);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    pGraphics->LoadFont("Dense-Regular", DENSE_FN);
    pGraphics->LoadFont("Jost-Italic", JOST_FN);
    pGraphics->LoadFont("Brixton-Bold", BRIXTON_FN);



    const IRECT b = pGraphics->GetBounds();

    const float labelH = 35;
    auto labelBounds = b.GetFromTop(labelH).GetPadded(-3);


    pGraphics->AttachControl(new MP::Section(labelBounds, "S  M  O  K  E  2.12 "));
    auto grid = SA_Grid(b.GetReducedFromTop(labelH).GetReducedFromRight(40), 5, 4);
      //auto g = grid.getAllCells(0.8);
    int c = 0;
    float sf = 0.8; // scale factor for resizing cells

    auto scaleCells = [&](std::vector<IRECT> &cells) {
      for (auto& c : cells) {
        c.ScaleAboutCentre(sf);
      }
      
    };


    // 1st row - global parameters
    auto cells1 = grid.getRowCells(0);
    scaleCells(cells1);
    pGraphics->AttachControl(new MP::KnobControl(cells1[0], kSmoke_position, "position"));
    pGraphics->AttachControl(new MP::KnobControl(cells1[1], kSmoke_size, "size"));
    pGraphics->AttachControl(new MP::KnobControl(cells1[2], kSmoke_pitch, "pitch"));
    pGraphics->AttachControl(new MP::KnobControl(cells1[3], kSmoke_density, "density"));
    pGraphics->AttachControl(new MP::KnobControl(cells1[4], kSmoke_texture, "texture"));

    
    pGraphics->AttachControl(new MP::Switch(grid.getRowAsArea(1, 1, 1), kSmoke_playbackMode, [&](IControl *c) {
      auto ShowHideGroup = [&](char* name, bool visible) {  GetUI()->ForControlInGroup(name, [&](IControl& c) {
        c.Hide(visible);
      }); };
      auto HideAllGroups = [&]() {
        ShowHideGroup("spectralParam", true);
        ShowHideGroup("granularParam", true);
      };
      clouds::PlaybackMode mode = (clouds::PlaybackMode)(c->GetValue() * 4);
      DBGMSG(std::to_string(mode).c_str());
      switch (mode)
      {
      default:
        break;
      case clouds::PLAYBACK_MODE_GRANULAR:
        HideAllGroups();
        ShowHideGroup("granularParam", false);
        break;
      case clouds::PLAYBACK_MODE_STRETCH:
        HideAllGroups();

        break;
      case clouds::PLAYBACK_MODE_LOOPING_DELAY:
        HideAllGroups();

        break;
      case clouds::PLAYBACK_MODE_SPECTRAL:
        HideAllGroups();
        ShowHideGroup("spectralParam", false);
        break;
      
      }

    }));

   pGraphics->AttachControl(new MP::KnobControl(grid.getRowCells(1)[0].GetScaledAboutCentre(sf), kSmoke_reverb, "reverb"));
    pGraphics->AttachControl(new MP::KnobControl(grid.getRowCells(1)[4].GetScaledAboutCentre(sf), kSmoke_feedback, "feedback"));


    /// 3rd row - parameter specific controls

    auto paramArea = grid.getRowAsArea(2);
    paramArea.ReduceFromLeft(paramArea.W() / 10.0);
    paramArea.ReduceFromRight(paramArea.W() / 10.0);

    auto pcells = SA_Grid(paramArea, 4, 1).getAllCells(sf);
    MP::ColorScheme granularParamColors = { MP::pallete.ACC_RED, MP::pallete.ACC_RED.WithOpacity(0.5), MP::pallete.BG_KNOB };

    pGraphics->AttachControl(new MP::KnobControl(pcells[0], kSmoke_stereo_spread, "spread", granularParamColors))->SetGroup("granularParam");
    pGraphics->AttachControl(new MP::KnobControl(pcells[1], kSmoke_overlap, "overlap", granularParamColors))->SetGroup("granularParam");
    pGraphics->AttachControl(new MP::KnobControl(pcells[2], kSmoke_window_shape, "frame shape", granularParamColors))->SetGroup("granularParam");
    pGraphics->AttachControl(new MP::KnobControl(pcells[3], kSmoke_granular_stereo_spread, "grain spread", granularParamColors))->SetGroup("granularParam");

    MP::ColorScheme spectralParamColors = { MP::pallete.ACC_PURPLE, MP::pallete.ACC_PURPLE.WithOpacity(0.5), MP::pallete.BG_KNOB };

    pGraphics->AttachControl(new MP::KnobControl(pcells[0], kSmoke_quantization, "quantization", spectralParamColors))->SetGroup("spectralParam");
    pGraphics->AttachControl(new MP::KnobControl(pcells[1], kSmoke_refresh_rate, "refresh rate", spectralParamColors))->SetGroup("spectralParam");
    pGraphics->AttachControl(new MP::KnobControl(pcells[2], kSmoke_phase_randomization, "phase rand", spectralParamColors))->SetGroup("spectralParam");
    pGraphics->AttachControl(new MP::KnobControl(pcells[3], kSmoke_warp, "warp", spectralParamColors))->SetGroup("spectralParam");
    


    // last bottom row of controls
    auto lastRow = grid.getRowCells(3);
    scaleCells(lastRow);
    pGraphics->AttachControl(new MP::ButtonGate(lastRow[0].GetScaledAboutCentre(0.7), "Freeze", [&]() {SetParameterValue(kSmoke_trigger, true); }, [&]() {SetParameterValue(kSmoke_trigger, false); }));

    pGraphics->AttachControl(new MP::KnobControl(lastRow[1].GetScaledAboutCentre(0.7), kGain));
    pGraphics->AttachControl(new MP::KnobControl(lastRow[2].GetScaledAboutCentre(1.1), kSmoke_dry_wet, "dry/wet"));
    pGraphics->AttachControl(new MP::KnobControl(lastRow[3].GetScaledAboutCentre(0.7), kOutputGain));

    pGraphics->AttachControl(new MP::ButtonGate(lastRow[4].GetScaledAboutCentre(0.7), "Trig", [&]() {SetParameterValue(kSmoke_trigger, true); }, [&]() {SetParameterValue(kSmoke_trigger, false); }));

    auto meterBounds = b.GetFromRight(20).GetReducedFromBottom(PLUG_HEIGHT * 0.05).GetReducedFromTop(labelH);
    pGraphics->AttachControl(new IVMeterControl<2>(meterBounds.GetTranslated(-20, 0).GetPadded(-3), ""), kCtrlTagMeterIN);
    pGraphics->AttachControl(new IVMeterControl<2>(meterBounds.GetPadded(-3), ""), kCtrlTagMeterOUT);
    IVColorSpec envMeterColors;
    envMeterColors.mColors[kFG] = MP::pallete.ACC_GOLD;
    pGraphics->AttachControl(new SA::Meter(meterBounds.GetTranslated(-40, 0).GetPadded(-6, 0,3, 0 )), kCtrlTagMeterEnv);

    auto envControls = SA_Grid(labelBounds.GetFromLeft(200).GetOffset(0,0, 0, 20), 4, 1).getAllCells(0.9);
    pGraphics->AttachControl(new MP::KnobControl(envControls[0], kNoTag, "th"))->SetActionFunction([&](IControl* c) {
      smokeProcessor.envThreshold = c->GetValue();
    });
    pGraphics->AttachControl(new MP::KnobControl(envControls[1], kNoTag, "att"))->SetActionFunction([&](IControl* c) {
      smokeProcessor.envFollower.SetAttack(c->GetValue() * 200);

    });
    pGraphics->AttachControl(new MP::KnobControl(envControls[2], kNoTag, "release"))->SetActionFunction([&](IControl* c) {
      smokeProcessor.envFollower.SetRelease(c->GetValue() * 1000);

    });
    pGraphics->AttachControl(new MP::KnobControl(envControls[3], kNoTag, "gain"))->SetActionFunction([&](IControl* c) {
      smokeProcessor.envFollower.SetGain(c->GetValue()*2);
    });

    // init paramControls visibility
    auto ShowHideGroup = [&](char* name, bool visible) {  GetUI()->ForControlInGroup(name, [&](IControl& c) {
      c.Hide(visible);
    }); };
    auto HideAllGroups = [&]() {
      ShowHideGroup("spectralParam", true);
      ShowHideGroup("granularParam", true);
    };
    HideAllGroups();
    ShowHideGroup("granularParam", false);

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
#endif
}

#if IPLUG_DSP
void Smoke::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  
  if (!smokeProcessor.readyToPlay) {
    inputs = outputs;
    return;
  }

  const double gain = GetParam(kGain)->Value() / 100.;
  const double gainOutput = GetParam(kOutputGain)->Value() / 100.;
  const int nChans = 2;
  for (int s = 0; s < nFrames; s++) {
    
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s] * gain;
    }
  }
#ifdef DEBUG
  dbg::ProcessBlock(outputs, outputs, nFrames);
#endif
  mMeterSenderInput.ProcessBlock(outputs, nFrames, kCtrlTagMeterIN);

  auto p = smokeProcessor.getParams();
  p->position = GetParam(kSmoke_position)->Value();
  p->size = GetParam(kSmoke_size)->Value();
  p->pitch = SA::math::rescale(GetParam(kSmoke_pitch)->Value(), 0.0f, 1.0f, -48.0f, 48.0f);
  p->density = GetParam(kSmoke_density)->Value();
  p->texture = GetParam(kSmoke_texture)->Value();
  p->reverb = GetParam(kSmoke_reverb)->Value();
  p->feedback = GetParam(kSmoke_feedback)->Value();
  p->stereo_spread = GetParam(kSmoke_stereo_spread)->Value();
  p->granular.overlap = GetParam(kSmoke_overlap)->Value();
  p->granular.stereo_spread = GetParam(kSmoke_granular_stereo_spread)->Value();
  p->granular.window_shape = GetParam(kSmoke_window_shape)->Value();
  p->spectral.quantization = GetParam(kSmoke_quantization)->Value();
  p->spectral.refresh_rate = GetParam(kSmoke_refresh_rate)->Value();
  p->spectral.phase_randomization = GetParam(kSmoke_phase_randomization)->Value();
  p->spectral.warp = GetParam(kSmoke_warp)->Value();
  p->dry_wet = GetParam(kSmoke_dry_wet)->Value();
  //p->trigger = GetParam(kSmoke_trigger)->Value();
  //p->gate = GetParam(kSmoke_trigger)->Value();
  smokeProcessor.playbackmode = (clouds::PlaybackMode)(GetParam(kSmoke_playbackMode)->Value() * 4);

  smokeProcessor.process(outputs, outputs, nFrames);
  


  for (size_t x = 0; x < nFrames; x++)
  {
    for (size_t y = 0; y < nChans; y++)
    {
      outputs[y][x] = clouds::SoftLimit(outputs[y][x] * gainOutput);
    }
  }
  mMeterSenderOutput.ProcessBlock(outputs, nFrames, kCtrlTagMeterOUT);
  ISenderData<1> envFollowerValue(kCtrlTagMeterEnv, 1, 0);
  envFollowerValue.vals[0] = smokeProcessor.envFollower.getEnvelopeMono();
  mEnvFollowerSender.PushData(envFollowerValue);
}


void Smoke::OnIdle()
{
  mMeterSenderInput.TransmitData(*this);
  mMeterSenderOutput.TransmitData(*this);
  mEnvFollowerSender.TransmitData(*this);
}

void Smoke::OnReset()
{
  smokeProcessor.setSampleRate(GetSampleRate());
}
#endif
