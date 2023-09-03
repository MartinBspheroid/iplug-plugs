#include "IPlugEffect.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "SA_Grid.h"



IPlugEffect::IPlugEffect(const InstanceInfo& info)
: Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kGain)->InitDouble("Gain", 10.f, 0., 100.0, 0.01, "%");
  //powerSag params
  GetParam(kPowerSag_Range)->InitDouble("Range", 0., 0., 1, 0.001, "");
  GetParam(kPowerSag_Dry_Inv)->InitDouble("Dry/Wet", 0.5, 0., 1, 0.001, "%");
  //diode params
  GetParam(kDiode_Triode)->InitDouble("Triode", 0., 0., 1, 0.001, "");
  GetParam(kDiode_ClassAB)->InitDouble("ClassAB", 0., 0., 1, 0.001, "");
  GetParam(kDiode_ClassB)->InitDouble("ClassB", 0., 0., 1, 0.001, "");
  GetParam(kDiode_DryWet)->InitDouble("DryWet", 0., 0., 1, 0.001, "%");
  // transistor params 
  GetParam(kTransistorVCA_Gain)->InitDouble("Gain", 0.5, 0., 1, 0.001, "%");
  // coils params
  GetParam(kCoils_Saturation)->InitDouble("Saturation", 0.0f, 0.0f, 1.0f, 0.01f);
  GetParam(kCoils_DC)->InitDouble("CoreDC", 0.5f, 0.0f, 1.0f, 0.01f);
  GetParam(kCoils_DrWet)->InitDouble("Dry/Wet", 1.0f, 0.0f, 1.0f, 0.01f);
  //tape params
  GetParam(kTape_InputGain)->InitDouble("InputGain", 0.5, 0., 1, 0.001, "");
  GetParam(kTape_Soften)->InitDouble("Soften", 0.5, 0., 1, 0.001, "");
  GetParam(kTape_HeadBump)->InitDouble("HeadBump", 0.5, 0., 1, 0.001, "");
  GetParam(kTape_Flutter)->InitDouble("Flutter", 0.0, 0., 1, 0.001, "");
  GetParam(kTape_OutputGain)->InitDouble("OutputGain", 0.5, 0., 1, 0.001, "");
  GetParam(kTape_DryWet)->InitDouble("DryWet", 0., 0.5, 1, 0.001, "");
//capacitor
  GetParam(kCapacitor_Lowpass)->InitDouble("LowPass", 0., 0., 1, 0.001, "");
  GetParam(kCapacitor_Highpass)->InitDouble("HighPass", 1.0f, 0., 1.f, 0.001, "");
  GetParam(kCapacitor_NonLin)->InitDouble("NonLin", 0., 0., 1, 0.001, "");
  GetParam(kCapacitor_DryWet)->InitDouble("DryWet", 0., 0., 1, 0.001, "");
//vinyl
  GetParam(kVinyl_MidHP)->InitDouble("MidHP", 0., 0., 1, 0.001, "");
  GetParam(kVinyl_SideHP)->InitDouble("SideHP", 0., 0., 1, 0.001, "");
  GetParam(kVinyl_HighLimit)->InitDouble("HighLimit", 0., 0., 1, 0.001, "");
  GetParam(kVinyl_GrooveWear)->InitDouble("GrooveWear", 0., 0., 1, 0.001, "");

  
  eq.pushEffect([&](sample** b, int channels, int frames) {
    power_Sag2.processReplacing(b, channels, frames);
  });

  eq.pushEffect([&](sample** b, int channels, int frames) {
    diode.processReplacing(b, channels, frames);
  });

  eq.pushEffect([&](sample** b, int channels, int frames) {
    transistor.processReplacing(b, channels, frames);
  });


  eq.pushEffect([&](sample** b, int channels, int frames) {
    coils.processReplacing(b, channels, frames);
  });

  eq.pushEffect([&](sample** b, int c, int f) {
    tape.processReplacing(b, c, f);
  });
  eq.pushEffect([&](sample** b, int c, int f) {
    capacitor.processReplacing(b, c, f);
  });
  eq.pushEffect([&](sample** b, int c, int f) {
    vinyl.processReplacing(b, c, f);
  });



#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_BLACK);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    pGraphics->LoadFont("Poppins-LightItalic", POPPY_FN);
    const IRECT b = pGraphics->GetBounds();

    std::vector<ISVG> icons;
    icons.push_back(GetUI()->LoadSVG(TRANS));
    icons.push_back(GetUI()->LoadSVG(DIODE));
    icons.push_back(GetUI()->LoadSVG(COIL));
    icons.push_back(GetUI()->LoadSVG(VINYL));
    icons.push_back(GetUI()->LoadSVG(TAPE));
    icons.push_back(GetUI()->LoadSVG(CAP));
    icons.push_back(GetUI()->LoadSVG(POWER));

    SA_Grid gridMain(b, 1, { 0.1, 0.7, 0.2 });
    IText style = DEFAULT_TEXT.WithFGColor(COLOR_WHITE)
      .WithFont("Roboto-Regular")
      .WithSize(gridMain.getRowAsArea(0).H() * 0.95)
      .WithVAlign(EVAlign::Middle)
      .WithAlign(EAlign::Near);


    IVStyle knobsStyle = DEFAULT_STYLE;

    knobsStyle.valueText = knobsStyle.valueText.WithFGColor(COLOR_WHITE).WithFont("Poppins-LightItalic");
    
    knobsStyle.labelText = knobsStyle.valueText;
    
    auto reorderFunc = [&](std::vector<int> order) {
      eq.setOrder(order);
    };
    pGraphics->AttachControl(new IVLabelControl(gridMain.getRowAsArea(0).GetTranslated(50, 0), "Vargant", DEFAULT_STYLE.WithValueText(style).WithDrawFrame(false) ));
    pGraphics->AttachControl(new SA::ReorderItems(gridMain.getRowAsArea(2), icons, reorderFunc));

    SA_Grid controls(gridMain.getRowAsArea(1), 9, 3);
    int cp = 0;
    auto cells = controls.getAllCells();
    //power Sag
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kPowerSag_Range, "RANGE", knobsStyle), kNoTag, "powerSagGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kPowerSag_Dry_Inv, "INV/DRY", knobsStyle), kNoTag, "powerSagGroup");
    //Diode
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kDiode_Triode, "Triode", knobsStyle), kNoTag, "diodeGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kDiode_ClassAB, "Class AB", knobsStyle), kNoTag, "diodeGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kDiode_ClassB, "Class B", knobsStyle), kNoTag, "diodeGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kDiode_DryWet, "Dry/Wet", knobsStyle), kNoTag, "diodeGroup");
    //Transistor
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kTransistorVCA_Gain, "Gain", knobsStyle), kNoTag, "transistorGroup");
    // Coils
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kCoils_Saturation, "Saturation", knobsStyle), kNoTag, "coilsGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kCoils_DC, "Core DC", knobsStyle), kNoTag, "coilsGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kCoils_DrWet, "Dry/Wet", knobsStyle), kNoTag, "coilsGroup");
    //Tape
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kTape_InputGain, "InputGain", knobsStyle), kNoTag, "tapeGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kTape_Soften, "Soften", knobsStyle), kNoTag, "tapeGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kTape_HeadBump, "HeadBump", knobsStyle), kNoTag, "tapeGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kTape_Flutter, "Flutter", knobsStyle), kNoTag, "tapeGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kTape_OutputGain, "OutputGain", knobsStyle), kNoTag, "tapeGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kTape_DryWet, "DryWet", knobsStyle), kNoTag, "tapeGroup");
    // Capacitor
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kCapacitor_Lowpass, "Lowpass", knobsStyle), kNoTag, "capacitorGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kCapacitor_Highpass, "Highpass", knobsStyle), kNoTag, "capacitorGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kCapacitor_NonLin, "NonLin", knobsStyle), kNoTag, "capacitorGroup");
    pGraphics->AttachControl(new IVKnobControl(cells.at(cp++).GetScaledAboutCentre(0.75), kCapacitor_DryWet, "DryWet", knobsStyle), kNoTag, "capacitorGroup");
    //Vinyl
    pGraphics->AttachControl(new IVKnobControl(controls.getAllCells().at(cp++).GetScaledAboutCentre(0.75), kVinyl_MidHP, "MidHP", knobsStyle), kNoTag, "vinylGroup");
    pGraphics->AttachControl(new IVKnobControl(controls.getAllCells().at(cp++).GetScaledAboutCentre(0.75), kVinyl_SideHP, "SideHP", knobsStyle), kNoTag, "vinylGroup");
    pGraphics->AttachControl(new IVKnobControl(controls.getAllCells().at(cp++).GetScaledAboutCentre(0.75), kVinyl_HighLimit, "HighLimit", knobsStyle), kNoTag, "vinylGroup");
    pGraphics->AttachControl(new IVKnobControl(controls.getAllCells().at(cp++).GetScaledAboutCentre(0.75), kVinyl_GrooveWear, "GrooveWear", knobsStyle), kNoTag, "vinylGroup");

    // sine gain

    pGraphics->AttachControl(new IVKnobControl(controls.getAllCells().at(controls.size()-1).GetScaledAboutCentre(0.6), kGain, "gain", knobsStyle), kNoTag);
    
   
 
  };
#endif
}

void IPlugEffect::OnActivate(bool enable)
{
  
}

#if IPLUG_DSP
void IPlugEffect::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
 
  const double gain = GetParam(kGain)->Value() / 100.;
  const int nChans = NOutChansConnected();
  for (int frames = 0; frames < nFrames; frames++) {
    for (int c = 0; c < nChans; c++) {
      
      outputs[c][frames] = inputs[c][frames]* gain;
    }
  }
  
  //ns.process(outputs, nChans, nFrames);
  //PQ.runFunctions(outputs, nChans, nFrames);
  power_Sag2.Range = GetParam(kPowerSag_Range)->Value();
  power_Sag2.Inv_Wet = GetParam(kPowerSag_Dry_Inv)->Value();
  //diode
  diode.Triode = GetParam(kDiode_Triode)->Value();
  diode.ClassAB= GetParam(kDiode_ClassAB)->Value();
  diode.ClassB = GetParam(kDiode_ClassB)->Value();
  diode.DryWet= GetParam(kDiode_DryWet)->Value();
  //transistor
transistor.gain= GetParam(kTransistorVCA_Gain)->Value();
//coils
coils.Saturation = GetParam(kCoils_Saturation)->Value();
coils.Core_DC = GetParam(kCoils_DC)->Value();
coils.DryWet = GetParam(kCoils_DrWet)->Value();
//tape
tape.InputGain = GetParam(kTape_InputGain)->Value();
tape.Soften = GetParam(kTape_Soften)->Value();
tape.HeadBump = GetParam(kTape_HeadBump)->Value();
tape.Flutter = GetParam(kTape_Flutter)->Value();
tape.OutputGain = GetParam(kTape_OutputGain)->Value();
tape.DryWet = GetParam(kTape_DryWet)->Value();
//capacitor
capacitor.Lowpass = GetParam(kCapacitor_Lowpass)->Value();
capacitor.Highpass = GetParam(kCapacitor_Highpass)->Value();
capacitor.NonLin = GetParam(kCapacitor_NonLin)->Value();
capacitor.DryWet = GetParam(kCapacitor_DryWet)->Value();
// vinyl
vinyl.MidHP = GetParam(kVinyl_MidHP)->Value();
vinyl.SideHP = GetParam(kVinyl_SideHP)->Value();
vinyl.HighLimit = GetParam(kVinyl_HighLimit)->Value();
vinyl.GrooveWear = GetParam(kVinyl_GrooveWear)->Value();

  eq.runQueue(outputs, nChans, nFrames);

  
  //interstage.processReplacing((float**)inputs, (float**)outputs, nFrames);

  
}
#endif
