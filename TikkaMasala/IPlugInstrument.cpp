#include "IPlugInstrument.h"
#include "IPlug_include_in_plug_src.h"
#include "IGraphicsFlexBox.h"
#include "Tikka_Presets.h"
#include "SA_Config.h"


IPlugInstrument::IPlugInstrument(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPresets))
{

  GetParam(kParamGain)->InitDouble("Gain", 100., 0., 100.0, 0.01, "%");
  

  for (size_t i = 0; i < 4; i++)
  {
    tracks.push_back(EuqSeq());
    tracks.at(i).init();
    tracks.at(i).sendMidi = [&](IMidiMsg m) {
      return SendMidiMsg(m);
    };
    tracks.at(i).note += i;
  }
#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_HEIGHT));
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    auto  DrumPresets = Tikka_Presets();
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(IColor(255, 11, 32, 39).WithOpacity(0.5));
    pGraphics->EnableMouseOver(true);
    pGraphics->EnableMultiTouch(true);

    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds().GetPadded(-5.f);
    auto topLabelRect = IRECT(0, 0, b.MW(), 50);
    pGraphics->AttachControl(new ITextControl(topLabelRect, "TIKKA", IText().WithAlign(EAlign::Near).WithSize(40).WithFGColor(colors[3]), DEFAULT_BGCOLOR));
    auto generalTrackContainer = b.GetFromTLHC(b.W(), 100).GetScaledAboutCentre(0.98).GetTranslated(0,50);

    auto darkCol = IColor(255, 11, 32, 39);
    int cOff = 1; // color offset;
    for (size_t i = 0; i < tracks.size(); i++)
    {
      IColor col = colors[i + 1];
      auto trackContainer = generalTrackContainer.GetTranslated(0, i * 110);
     
      
      
      // ---- attach sequencer --------
      auto seqRect = trackContainer.GetFromRight(trackContainer.W() * 0.65).GetScaledAboutCentre(0.95);

      
      auto bRect = seqRect.GetScaledAboutCentre(0.25); /// rect for buttons in overlay menu
      pGraphics->AttachControl(new Tikka_Seq(seqRect, colors[i + cOff], tracks.at(i)));


      Tikka_Layout patternsLayout;
      patternsLayout.wrap = YGWrap::YGWrapWrap;
      patternsLayout.justify= YGJustify::YGJustifySpaceEvenly;
      patternsLayout.calc_FromText = true;

      auto PresetMenu_rect = IRECT().MakeXYWH(seqRect.L, topLabelRect.B, seqRect.W(), pGraphics->GetBounds().B - 10);
      auto patternsMenu = new Tikka_OverlayMenu(PresetMenu_rect,patternsLayout, col, NULL);

      
      for (size_t p = 0; p < DrumPresets.presets.size(); p++)
      {
        auto preset = DrumPresets.presets.at(p);
        patternsMenu->AddButton(Tikka_Button(bRect, [=]() {
          tracks.at(i).length = preset.lenght;
          tracks.at(i).beats = preset.pulses;
          tracks.at(i).recalc();
        }, col, preset.name, false, YGAlign::YGAlignAuto));

      }
      
      patternsMenu->recalcItemsPosition();
      
      pGraphics->AttachControl(patternsMenu);
      Tikka_Layout speedLayout;
      speedLayout.margin = 10;
      speedLayout.wrap = YGWrap::YGWrapWrap;
      auto speedMenu = new Tikka_OverlayMenu(seqRect, speedLayout, col, NULL);
      pGraphics->AttachControl(speedMenu);

      speedMenu->AddButton(Tikka_Button(bRect, [=]() {
        tracks.at(i).division = EuqSeq::Divisions::One_Sixteen;
      }, col, "1/16"));
      speedMenu->AddButton(Tikka_Button(bRect, [=]() {
        tracks.at(i).division = EuqSeq::Divisions::One_Eitght;
      }, col, "1/8"));
      speedMenu->AddButton(Tikka_Button(bRect, [=]() {
        tracks.at(i).division = EuqSeq::Divisions::One_Four;
      }, col, "1/4"));
      speedMenu->AddButton(Tikka_Button(bRect, [=]() {
        tracks.at(i).division = EuqSeq::Divisions::One_Half;
      }, col, "1/2"));
      speedMenu->AddButton(Tikka_Button(bRect, [=]() {
        tracks.at(i).division = EuqSeq::Divisions::One_Bar;
      }, col, "1/1"));
      speedMenu->AddButton(Tikka_Button(bRect, [=]() {
        tracks.at(i).division = EuqSeq::Divisions::Two_Bars;
      }, col, "2/1"));
      speedMenu->AddButton(Tikka_Button(bRect, [=]() {
        tracks.at(i).dot == 1 ? tracks.at(i).dot = 1.5 : tracks.at(i).dot = 1;
      }, col, "DOT", true, YGAlign::YGAlignStretch));

      speedMenu->recalcItemsPosition();

      
      auto vMenu = seqRect.GetHSliced(seqRect.W() * 0.1);
      /*float vMenuWidth = vMenu.W();
      vMenu.R = seqRect.L;
      vMenu.L = vMenu.R - vMenuWidth;*/
      //vMenu.Translate(seqRect.R , 0);
      vMenu = vMenu.GetVAlignedTo(seqRect, EVAlign::Middle);
      vMenu.Translate(-vMenu.W(), 0);

      auto vMenuItemRect = vMenu.GetFromTop(seqRect.H() / 5);
      float vh = vMenuItemRect.H();
      std::vector<Tikka_MenuItem> items;
      std::vector<std::string> menuTxt = { "M", "S", "X", "Y", "Z" };
      auto closeAll = [=]() {
        speedMenu->collapse();
        patternsMenu->collapse();
      };
      
      int j = 0;
      pGraphics->AttachControl(new Tikka_MenuItem(vMenuItemRect.GetTranslated(0, (j++) * vh), col, "M", [=]() {
        tracks.at(i).muted = !tracks.at(i).muted;
      }));
      pGraphics->AttachControl(new Tikka_MenuItem(vMenuItemRect.GetTranslated(0, (j++) * vh), col, "S", [=]() {}));
      pGraphics->AttachControl(new Tikka_MenuItem(vMenuItemRect.GetTranslated(0, (j++) * vh), col, "I",
        [=]() { closeAll(); speedMenu->isCollapsed ? speedMenu->expand() : speedMenu->collapse(); }));
      pGraphics->AttachControl(new Tikka_MenuItem(vMenuItemRect.GetTranslated(0, (j++) * vh), col, "X",
        [=]() { closeAll(); patternsMenu->isCollapsed ? patternsMenu->expand() : patternsMenu->collapse(); }));
      
      pGraphics->AttachControl(new Tikka_MenuItem(vMenuItemRect.GetTranslated(0, (j++) * vh), col, "Z", [=]() {}));


      auto recalc = [=]() {
        tracks.at(i).recalc();

      };

      auto controls = trackContainer.GetFromLeft(trackContainer.W() * 0.25); // controls
      float h = controls.H();
      float w = controls.W();
      IFlexBox fb;
      fb.Init(controls, YGFlexDirectionRow, YGJustifyFlexStart, YGWrapWrap);
      for (size_t i = 0; i < 6; i++)
      {
        fb.AddItem(controls.W()*.32, controls.H() * .5, YGAlign::YGAlignStretch, 1);
      }
      fb.CalcLayout();
      auto knobBounds = fb.GetItemBounds();
      


      pGraphics->AttachControl(new Tikka_NumberKnobInt(knobBounds[0], colors[i + cOff],
        "LENGHT", tracks.at(i).length, 1, 32, recalc));
      pGraphics->AttachControl(new Tikka_NumberKnobInt(knobBounds[1], colors[i + cOff],
        "STEPS", tracks.at(i).beats, 1, 32, recalc));
      pGraphics->AttachControl(new Tikka_NumberKnobInt(knobBounds[2], colors[i + cOff],
        "ACCENTS", tracks.at(i).accents, 1, 32, recalc));
      pGraphics->AttachControl(new Tikka_NumberKnobInt(knobBounds[3], colors[i + cOff],
        "NOTE", tracks.at(i).note, 1, 114, recalc));
      pGraphics->AttachControl(new Tikka_NumberKnobInt(knobBounds[4], colors[i + cOff],
        "VELOCITY", tracks.at(i).note, 1, 114, recalc));
      pGraphics->AttachControl(new Tikka_NumberKnobInt(knobBounds[5], colors[i + cOff],
        "ACCOFF", tracks.at(i).note, 1, 114, recalc));
    }
    
    ppqTxt = new ITextControl(pGraphics->GetBounds(), "", IText().WithFGColor(COLOR_WHITE));
    pGraphics->AttachControl(ppqTxt);
    
#endif
  };
}

#if IPLUG_DSP
void IPlugInstrument::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{

#ifdef VST2_API


  for (size_t i = 0; i < tracks.size(); i++)
  {
    tracks.at(i).update(mTimeInfo.mPPQPos, nFrames, mTimeInfo.mTransportIsRunning);

  }



#endif

#ifdef APP_API
  time += 0.001;

  for (size_t i = 0; i < tracks.size(); i++)
  {
    tracks.at(i).update(time, nFrames, true);

  }
#endif
}

void IPlugInstrument::OnIdle()
{
  std::string info;

#ifdef APP_API
  info += "PPQ: " + std::to_string(time) + '\n';
#else
  info += "PPQ: " + std::to_string(mTimeInfo.mPPQPos) + '\n';
  info += "Step: " + std::to_string((int)(mTimeInfo.mPPQPos * 4)) + ' ';
#endif
  info += "Tempo " + std::to_string(mTimeInfo.mTempo) + "\n";
  info += "Running " + std::to_string(mTimeInfo.mTransportIsRunning) + "\n";
  info += "Looping  " + std::to_string(mTimeInfo.mTransportLoopEnabled) + "\n";
  info += "Denom " + std::to_string(mTimeInfo.mDenominator) + "\n";
  info += "Numer " + std::to_string(mTimeInfo.mNumerator) + "\n";
  //ppqTxt->SetStr(info.c_str());
  ppqTxt->SetDirty(true);

}

void IPlugInstrument::OnReset()
{

}

void IPlugInstrument::ProcessMidiMsg(const IMidiMsg& msg)
{
  TRACE;

  int status = msg.StatusMsg();

  switch (status)
  {
  case IMidiMsg::kNoteOn:
  case IMidiMsg::kNoteOff:
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

  SendMidiMsg(msg);
}

void IPlugInstrument::OnParamChange(int paramIdx)
{
  /*mDSP.SetParam(paramIdx, GetParam(paramIdx)->Value());*/
}

bool IPlugInstrument::OnMessage(int msgTag, int ctrlTag, int dataSize, const void* pData)
{
  //if (msgTag == kMsgTagSliderChanged)
  //{
  //  auto* pConverted = reinterpret_cast<const ParamTuple*>(pData);

  //  //mSteps[pConverted->idx] = pConverted->value;

  //  return true;
  //}
  return false;


}

#endif

