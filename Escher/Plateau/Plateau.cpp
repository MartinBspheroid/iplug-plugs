#include "Plateau.hpp"

Plateau::Plateau() {
	/*config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(Plateau::DRY_PARAM, 0.0f, 1.f, 1.f, "Dry Level");
    configParam(Plateau::WET_PARAM, 0.0f, 1.f, 0.5f, "Wet Level");
    configParam(Plateau::PRE_DELAY_PARAM, 0.f, 0.500f, 0.f, "Pre-delay");
    configParam(Plateau::INPUT_LOW_DAMP_PARAM, 0.f, 10.f, 10.f, "Input Low Cut");
    configParam(Plateau::INPUT_HIGH_DAMP_PARAM, 0.f, 10.f, 10.f, "Input High Cut");
    configParam(Plateau::SIZE_PARAM, 0.f, 1.f, 0.5f, "Size");
    configParam(Plateau::DIFFUSION_PARAM, 0.f, 10.f, 10.f, "Diffusion");
    configParam(Plateau::DECAY_PARAM, 0.1f, 0.9999f, 0.54995f, "Decay");
    configParam(Plateau::REVERB_LOW_DAMP_PARAM, 0.0f, 10.f, 10.f, "Reverb Low Cut");
    configParam(Plateau::REVERB_HIGH_DAMP_PARAM, 0.0f, 10.f, 10.f, "Reverb High Cut");
    configParam(Plateau::MOD_SPEED_PARAM, 0.f, 1.f, 0.f, "Modulation Rate");
    configParam(Plateau::MOD_DEPTH_PARAM, 0.f, 16.f, 0.5f, "Modulation Depth");
    configParam(Plateau::MOD_SHAPE_PARAM, 0.f, 1.f, 0.5f, "Modulation Shape");


    configParam(Plateau::DRY_CV_PARAM, -1.f, 1.f, 0.f, "Dry CV Depth");
    configParam(Plateau::WET_CV_PARAM, -1.f, 1.f, 0.f, "Wet CV Depth");
    configParam(Plateau::INPUT_LOW_DAMP_CV_PARAM, -1.f, 1.f, 0.f, "Input Low Cut CV");
    configParam(Plateau::INPUT_HIGH_DAMP_CV_PARAM, -1.f, 1.f, 0.f, "Input High Cut CV");
    configParam(Plateau::SIZE_CV_PARAM, -1.f, 1.f, 0.f, "Size CV");
    configParam(Plateau::DIFFUSION_CV_PARAM, -1.f, 1.f, 0.f, "Diffusion CV");
    configParam(Plateau::DECAY_CV_PARAM, -1.f, 1.f, 0.f, "Decay CV");
    configParam(Plateau::REVERB_LOW_DAMP_CV_PARAM, -1.f, 1.f, 0.f, "Reverb Low Cut CV");
    configParam(Plateau::REVERB_HIGH_DAMP_CV_PARAM, -1.f, 1.f, 0.f, "Reverb High Cut CV");
    configParam(Plateau::MOD_SPEED_CV_PARAM, -1.f, 1.f, 0.f, "Mod Speed CV");
    configParam(Plateau::MOD_SHAPE_CV_PARAM, -1.f, 1.f, 0.f, "Mod Shape CV");
    configParam(Plateau::MOD_DEPTH_CV_PARAM, -1.f, 1.f, 0.f, "Mod Depth CV");
    configParam(Plateau::FREEZE_PARAM, 0.f, 1.f, 0.f, "Freeze");
    configParam(Plateau::FREEZE_TOGGLE_PARAM, 0.f, 1.f, 0.f, "Freeze Toggle");
    configParam(Plateau::CLEAR_PARAM, 0.f, 1.f, 0.f, "Clear");
    configParam(Plateau::TUNED_MODE_PARAM, 0.f, 1.f, 0.f, "Tuned Mode");
    configParam(Plateau::DIFFUSE_INPUT_PARAM, 0.f, 1.f, 1.f, "Diffuse Input");*/

  //Initialize();

}


void Plateau::Initialize(float const& SampleRate)
{
  reverb.setSampleRate(SampleRate);
  envelope.setSampleRate(SampleRate);
  envelope.setTime(0.004f);
  envelope._value = 1.f;

  wet = 0.5f;
  dry = 1.f;
  preDelay = 0.f;
  preDelayCVSens = preDelayNormSens;
  size = 1.f;
  diffusion = 1.f;
  decay = 0.f;
  inputDampLow = 0.f;
  inputDampHigh = 10.f;
  reverbDampLow = 0.f;
  reverbDampHigh = 10.f;
  modSpeed = 0.1f;
  modShape = 0.5f;
  modDepth = 0.0f;

  freezeButtonState = false;
  freezeToggle = false;
  freezeToggleButtonState = false;
  freeze = false;
  frozen = false;
  tunedButtonState = false;
  diffuseButtonState = false;
  preDelayCVSensState = 0;
  inputSensitivityState = 0;
  outputSaturationState = 1;

  clear = false;
  cleared = true;
  fadeOut = false;
  fadeIn = false;
  tuned = 0;
  diffuseInput = 1;

  leftInput = 0.f;
  rightInput = 0.f;
}

void Plateau::process(iplug::sample* inputs, iplug::sample* outputs) {
    //Freeze
    /*freezeToggle = params[FREEZE_TOGGLE_PARAM].getValue() > 0.5f ? true : false;
    lights[FREEZE_TOGGLE_LIGHT].value = freezeToggle ? 10.f : 0.f;

    if((params[FREEZE_PARAM].getValue() > 0.5f || inputs[FREEZE_CV_INPUT].getVoltage() > 0.5f)
    && !freezeButtonState) {
        freeze = freezeToggle ? !freeze : true;
        freezeButtonState = true;
    }
    if(params[FREEZE_PARAM].getValue() <= 0.5f && inputs[FREEZE_CV_INPUT].getVoltage() <= 0.5f
    && freezeButtonState) {
        freeze = freezeToggle ? freeze : false;
        freezeButtonState = false;
    }*/

    if(freeze && !frozen) {
        frozen = true;
        reverb.freeze();
    }
    else if(!freeze && frozen){
        frozen = false;
        reverb.unFreeze();
    }
    //lights[FREEZE_LIGHT].value = freeze ? 10.f : 0.f;

    /*tuned = params[TUNED_MODE_PARAM].getValue() > 0.5f ? 1 : 0;
    lights[TUNED_MODE_LIGHT].value = tuned ? 10.f : 0.f;*/

    /*diffuseInput = params[DIFFUSE_INPUT_PARAM].getValue();
    lights[DIFFUSE_INPUT_LIGHT].value = diffuseInput ? 10.f : 0.f;*/

    // Clear
    //if((params[CLEAR_PARAM].getValue() > 0.5f || inputs[CLEAR_CV_INPUT].getVoltage() > 0.5f) && !clear && cleared) {
    //    cleared = false;
    //    clear = true;
    //    //clear = 1;
    //}
    //else if((params[CLEAR_PARAM].getValue() < 0.5f && inputs[CLEAR_CV_INPUT].getVoltage() < 0.5f) && cleared) {
    //    clear = false;
    //}
    //
    if(clear) {
        if(!cleared && !fadeOut && !fadeIn) {
            fadeOut = true;
            envelope.setStartEndPoints(1.f, 0.f);
            envelope.trigger();
            //lights[CLEAR_LIGHT].value = 10.f;
        }
        if(fadeOut && envelope._justFinished) {
            reverb.clear();
            fadeOut = false;
            fadeIn = true;
            envelope.setStartEndPoints(0.f, 1.f);
            envelope.trigger();
        }
        if(fadeIn && envelope._justFinished) {
            fadeIn = false;
            cleared = true;
            //lights[CLEAR_LIGHT].value = 0.f;
            envelope._value = 1.f;
        }
    }
    envelope.process();

    // CV
    switch(preDelayCVSensState) {
        case 0: preDelayCVSens = preDelayNormSens; break;
        case 1: preDelayCVSens = preDelayLowSens;
    }
    /*preDelay = params[PRE_DELAY_PARAM].getValue();*/
    //preDelay += 0.5f * (powf(2.f, inputs[PRE_DELAY_CV_INPUT].getVoltage() * preDelayCVSens) - 1.f);
    // TODO -> change params
    reverb.setPreDelay(preDelay);

    //size = inputs[SIZE_CV_INPUT].getVoltage() * params[SIZE_CV_PARAM].getValue() * 0.1f;
    //size += params[SIZE_PARAM].getValue();
    if(tuned) {
        size = sizeMin * powf(2.f, size * 5.f);
        size = rack::math::clamp(size, sizeMin, 2.5f);
    }
    else {
      size *= size;
      size = rack::math::rescale(size, 0.f, 1.f, 0.01f, sizeMax);
      size = rack::math::clamp(size, 0.01f, sizeMax);
    }
    reverb.setTimeScale(size);

    /*diffusion = inputs[DIFFUSION_CV_INPUT].getVoltage() * params[DIFFUSION_CV_PARAM].getValue();
    diffusion += params[DIFFUSION_PARAM].getValue();
    diffusion = rack::math::clamp(diffusion, 0.f, 10.f);*/
    reverb.plateDiffusion1 = rack::math::rescale(diffusion, 0.f, 1.0, 0.f, 0.7f);
    reverb.plateDiffusion2 = rack::math::rescale(diffusion, 0.f, 1., 0.f, 0.5f);

    /*decay = rack::math::rescale(inputs[DECAY_CV_INPUT].getVoltage() * params[DECAY_CV_PARAM].getValue(), 0.f, 10.f, 0.1f, 0.999f);
    decay += params[DECAY_PARAM].getValue();*/
    decay = rack::math::clamp(decay, 0.1f, decayMax);
    decay = 1.f - decay;
    decay = 1.f - decay * decay;

    /*inputDampLow = inputs[INPUT_LOW_DAMP_CV_INPUT].getVoltage() * params[INPUT_LOW_DAMP_CV_PARAM].getValue();
    inputDampLow += params[INPUT_LOW_DAMP_PARAM].value;*/
    inputDampLow = rack::math::clamp(inputDampLow, 0.f, 10.f);
    inputDampLow = 10.f - inputDampLow;

    /*inputDampHigh = inputs[INPUT_HIGH_DAMP_CV_INPUT].getVoltage() * params[INPUT_HIGH_DAMP_CV_PARAM].getValue();
    inputDampHigh += params[INPUT_HIGH_DAMP_PARAM].getValue();*/
    inputDampHigh = rack::math::clamp(inputDampHigh, 0.f, 10.f);

    /*reverbDampLow = inputs[REVERB_LOW_DAMP_CV_INPUT].getVoltage() * params[REVERB_LOW_DAMP_CV_PARAM].getValue();
    reverbDampLow += params[REVERB_LOW_DAMP_PARAM].getValue();*/
    reverbDampLow = rack::math::clamp(reverbDampLow, 0.f, 10.f);
    reverbDampLow = 10.f - reverbDampLow;

    /*reverbDampHigh = inputs[REVERB_HIGH_DAMP_CV_INPUT].getVoltage() * params[REVERB_HIGH_DAMP_CV_PARAM].getValue();
    reverbDampHigh += params[REVERB_HIGH_DAMP_PARAM].getValue();*/
    reverbDampHigh = rack::math::clamp(reverbDampHigh, 0.f, 10.f);

    reverb.diffuseInput = (double)diffuseInput;

    reverb.decay = decay;
    reverb.inputLowCut = 440.f * powf(2.f, inputDampLow - 5.f);
    reverb.inputHighCut = 440.f * powf(2.f, inputDampHigh - 5.f);
    reverb.reverbLowCut = 440.f * powf(2.f, reverbDampLow - 5.f);
    reverb.reverbHighCut = 440.f * powf(2.f, reverbDampHigh - 5.f);

    /*modSpeed = inputs[MOD_SPEED_CV_INPUT].getVoltage() * params[MOD_SPEED_CV_PARAM].getValue() * 0.1f;
    modSpeed += params[MOD_SPEED_PARAM].getValue();*/
    modSpeed = rack::math::clamp(modSpeed, modSpeedMin, modSpeedMax);
    modSpeed *= modSpeed;
    modSpeed = modSpeed * 99.f + 1.f;

    /*modShape = inputs[MOD_SHAPE_CV_INPUT].getVoltage() * params[MOD_SHAPE_CV_PARAM].getValue() * 0.1f;
    modShape += params[MOD_SHAPE_PARAM].getValue();*/
    modShape = rack::math::rescale(modShape, 0.f, 1.f, modShapeMin, modShapeMax);
    modShape = rack::math::clamp(modShape, modShapeMin, modShapeMax);

    /*modDepth = inputs[MOD_DEPTH_CV_INPUT].getVoltage() * params[MOD_DEPTH_CV_PARAM].getValue();
    modDepth = rack::math::rescale(modDepth, 0.f, 10.f, modDepthMin, modDepthMax);*/
    //modDepth += params[MOD_DEPTH_PARAM].getValue();
    modDepth = rack::math::clamp(modDepth, modDepthMin, modDepthMax);

    reverb.modSpeed = modSpeed;
    reverb.modDepth = modDepth;
    reverb.setModShape(modShape);

    /*leftInput = inputs[LEFT_INPUT].getVoltage();
    rightInput = inputs[RIGHT_INPUT].getVoltage();
    if (inputs[LEFT_INPUT].isConnected() == false && inputs[RIGHT_INPUT].isConnected() == true) {
      leftInput = inputs[RIGHT_INPUT].getVoltage();
    }
    else if (inputs[LEFT_INPUT].isConnected() == true && inputs[RIGHT_INPUT].isConnected() == false) {
      rightInput = inputs[LEFT_INPUT].getVoltage();
    }*/

    //leftInput = rack::math::clamp(leftInput, -10.0, 10.0);
    //rightInput = rack::math::clamp(rightInput, -10.0, 10.0);

    //leftInput = rack::math::clamp(leftInput, -10.0, 10.0);
    //rightInput = rack::math::clamp(rightInput, -10.0, 10.0);

    inputSensitivity = inputSensitivityState ? 0.125893f : 1.f;
    leftInput = inputs[0] *inputSensitivity * envelope._value;
    rightInput = inputs[1] * inputSensitivity * envelope._value;
    reverb.process(leftInput, rightInput);
     

   /* dry = inputs[DRY_CV_INPUT].getVoltage() * params[DRY_CV_PARAM].getValue();
    dry += params[DRY_PARAM].getValue();*/
    dry = rack::math::clamp(dry, 0.f, 1.f);

   /* wet = inputs[WET_CV_INPUT].getVoltage() * params[WET_CV_PARAM].getValue();
    wet += params[WET_PARAM].getValue();*/
    wet = rack::math::clamp(wet, 0.f, 1.f);

    outputs[0] = (leftInput * dry + reverb.leftOut * wet * envelope._value);
    outputs[1] = (rightInput * dry + reverb.rightOut * wet * envelope._value);

    if(outputSaturationState) {
        outputs[0] = (tanhDriveSignal(outputs[0] * 0.111f, 0.95f) *0.99);
        outputs[1] = (tanhDriveSignal(outputs[1] * 0.111f, 0.95f) * 0.99);
    }
}

void Plateau::onSampleRateChange(const float& sampleRate) {
    reverb.setSampleRate(sampleRate);
    envelope.setSampleRate(sampleRate);
}

