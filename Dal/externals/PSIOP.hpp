// Title Font - Jaapokki
// https://mikkonuuttila.com/jaapokki/
// Main font - Jost
// https://indestructibletype.com/Jost.html
#pragma once
//#include "plugin.hpp"
#include "tables.h"
#include "ffCommon.hpp"
#include "ffFilters.hpp"
#include "SA_Math.h"
#include "IGraphicsImGui.h"
#include <cmath>
//#include <math>
/** Turns HIGH when value reaches 1.f, turns LOW when value reaches 0.f. */

template <typename T = float>
struct TSchmittTrigger {
  T state;
  TSchmittTrigger() {
    reset();
  }
  void reset() {
    state = T::mask();
  }
  T process(T in) {
    T on = (in >= 1.f);
    T off = (in <= 0.f);
    T triggered = ~state & on;
    state = on | (state & ~off);
    return triggered;
  }
};


template <>
struct TSchmittTrigger<float> {
  bool state = true;

  void reset() {
    state = true;
  }

  /** Updates the state of the Schmitt Trigger given a value.
  Returns true if triggered, i.e. the value increases from 0 to 1.
  If different trigger thresholds are needed, use
    process(rescale(in, low, high, 0.f, 1.f))
  for example.
  */
  bool process(float in) {
    if (state) {
      // HIGH to LOW
      if (in <= 0.f) {
        state = false;
      }
    }
    else {
      // LOW to HIGH
      if (in >= 1.f) {
        state = true;
        return true;
      }
    }
    return false;
  }

  bool isHigh() {
    return state;
  }
};

typedef TSchmittTrigger<> SchmittTrigger;

struct PSIOP
{


  Operator operators[4];
  Ramp ramps[3];

  DCBlock dcBlock;
  bool blocking = true;
  bool looping = false;

  SchmittTrigger trigger;
  SchmittTrigger choke;
  SchmittTrigger accent;



  /// <summary>
  /// Goes from -4 to 4
  /// </summary>
  float startPitch = 0;
  /// <summary>
/// Goes from -4 to 4
/// </summary>
  float endPitch = 0;
  float finePitch = 0;
  float rates[3] = { 0.5, 0.5, 0.5 }; //release rates 

  /// <summary>
  /// Index of the algorithim
  /// Goes from 0 - 5.
  /// </summary>
  int algo = 0;
  /// <summary>
  /// ---- RANGE: 0 - 31 
  /// </summary>
  int ratioIndex = 0;
  /// <summary>
  /// Cotrols feedback
  /// ---RANGE  0 - 1
  /// </summary>
  float feedback = 0;
  /// <summary>
  /// // Get the wavetable index
  /// ---- RANGE 0 - 63
  /// </summary>
  int table = 0;
  float index = 0.6f; // Global modulation index
  float level = 1.0f;

  PSIOP()
  {
    /*config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    configParam(START_PARAM, -4.f, 4.f, 0.f, "Start Freq", "Hz", dsp::FREQ_SEMITONE, dsp::FREQ_C4);
    configParam(FINE_PARAM, -0.2f, 0.2f, 0.f, "Start Fine Freq");
    configParam(END_PARAM, -4.f, 4.f, 0.f, "End Freq", "Hz", dsp::FREQ_SEMITONE, dsp::FREQ_C4);
    configParam(RATIO_PARAM, 0.f, 31.f, 0.f, "FM Ratios");
    configParam(WAVE_PARAM, 0.f, 63.f, 0.f, "Wave Combination");
    configParam(ALGO_PARAM, 0.f, 5.f, 0.f, "FM Algorithm");
    configParam(FB_PARAM, 0.f, 1.f, 0.f, "OP 1 Feedback");
    configParam(RATE1_PARAM, 0.f, 1.f, 0.5f, "Operator 1 & 3 Release Envelope");
    configParam(RATE2_PARAM, 0.f, 1.f, 0.5f, "Operator 2 & 4 Release Envelope");
    configParam(SPEED_PARAM, 0.f, 1.f, 0.f, "Pitch Envelope Speed");
    configParam(RATE2ATTEN_PARAM, -1.f, 1.f, 0.f, "Rate 2 Attenuverter");
    configParam(WAVEATTEN_PARAM, -1.f, 1.f, 0.f, "Wave Attenuverter");
    configParam(RATIOATTEN_PARAM, -1.f, 1.f, 0.f, "Ratio Attenuverter");*/
  }
  void setSampletime(const int& SampleRate) {
    sampleTime = 1.0f / SampleRate;
  }
  float fakeTrigger = 0;
  float fakeAccent = 0;
  float fakeChokeTrigger = 0;
  float sampleTime;
  float ampEnvDecay1 = 0.5;
  float ampEnvDecay2 = 0.5;
  float pitchEnvDecay = 0.5;
  float Attack1 = 0;
    float Attack2= 0;
    float Attack3= 0;
    float Shape1= 0;
    float Shape2= 0;
    float PitchEnvDecayShape= 0.3;

  

  float process(/*const ProcessArgs &args*/)
  {

    // Look for input on the trigger
    // All parameters are held on trigger input
    if (trigger.process(fakeTrigger))
    {
      // Look for accent trigger
      if (accent.process(fakeAccent))
      {
        index = 1.f;
        level = 1.8f;
      }
      else
      {
        index = 0.6f;
        level = 1.f;
      }

      // Compute the start and end pitches

      startPitch += finePitch;
      //startPitch = math::clamp(startPitch, -6.f, 4.f);



      //endPitch = math::clamp(endPitch, -8.f, 4.f);

      // Get the index for the ratio matrix
      /*ratioIndex = (int)params[RATIO_PARAM].getValue();
      ratioIndex += (int)round(inputs[RATIO_INPUT].getVoltage() * params[RATIOATTEN_PARAM].getValue());*/
      ratioIndex = math::clamp(ratioIndex, 0, 31);

      // Get the wavetable index
      /*table = (int)params[WAVE_PARAM].getValue();
      table += (int)round(inputs[WAVE_INPUT].getVoltage() * params[WAVEATTEN_PARAM].getValue());*/
      table = math::clamp(table, 0, 63);

      // Get the algorithim

      /*algo = (int)params[ALGO_PARAM].getValue();
      algo += (int)round(inputs[ALGO_INPUT].getVoltage());*/
      algo = math::clamp(algo, 0, 5);

      // Get the OP1 feedback amount
      /*feedback = params[FB_PARAM].getValue();
      feedback += 0.2f * inputs[FB_INPUT].getVoltage();*/
      feedback = math::clamp(feedback, 0.f, 1.f);

      // Get the rates for the volume and pitch envelopes
      //for (int i = 0; i < 3; i++)
      //{
      //    rates[i] = rate1;
      //    // Special case to factor in rate 2 attenuator
      //    rates[i] += i == 1 ? 0.2 * rate2 : 0.2 * rate1;
      //    rates[i] = math::clamp(rates[i], 0.f, 1.f);
      //}
      rates[0] = ampEnvDecay1;
      rates[1] = ampEnvDecay2;
      rates[2] = pitchEnvDecay;

      // Trigger
      for (int i = 0; i < 3; i++)
      {
        // Set the gate for the ramps to active
        ramps[i].gate = true;
      }
      for (size_t i = 0; i < 4; i++)
      {
        operators[i].phase = 0;
      }
    }

    // Look for Choke trigger
    if (choke.process(fakeChokeTrigger))
    {
      for (int i = 0; i < 3; i++)
      {
        // Set the gate for the ramps to off
        ramps[i].gate = false;
        ramps[i].out = 0.f;
      }
    }

    // Process amplitude ramps

    ramps[0].process(Shape1, Attack1, ampEnvDecay1, sampleTime, false);
    ramps[1].process(Shape2, Attack2, ampEnvDecay2, sampleTime, false);

    // Compute current pitch as a function of pitchStart, pitchEnd and the pitch speed envelope
    float pitch = startPitch;
    if (pitchEnvDecay > 0.2)
    {
      ramps[2].process(PitchEnvDecayShape, Attack3, 1 - pitchEnvDecay, sampleTime, looping);

      // Crossfade from start pitch to end pitch
      float xf = ramps[2].out;
      pitch = math::crossfade(endPitch, startPitch, xf);
    }

    // Process operators
    float output = 0.f;

    for (int i = 0; i < 4; i++)
    {
      // Set initial pitch for each operator
      operators[i].setPitch(pitch);

      // Actual per operator ratio to be used is taken from the LUT of magic ratios
      float ratio = fm_frequency_ratios[ratioMatrix[ratioIndex][i]];
      operators[i].applyRatio(ratio);

      float fmMod = 0;

      // Determine how much operator i is modulated by other modulators j++
      for (int j = 0; j < 4; j++)
      {
        fmMod += operators[j].out * index * modMatrix[algo][j][i];
      }

      // Accumulate phase, apply FM modulation, apply appropriate amp modulation
      // Feedback is applied for OP1 only
      // Ramp 1 affects OP1 & OP3 VCA, ramp 2 affects OP2 & OP4
      if (i == 0)
      {
        operators[i].process(sampleTime, ramps[0].out, fmMod, feedback, tableMatrix[table][i]);
      }
      else if (i == 2)
      {
        operators[i].process(sampleTime, ramps[0].out, fmMod, 0, tableMatrix[table][i]);
      }
      else
      {
        operators[i].process(sampleTime, ramps[1].out, fmMod, 0, tableMatrix[table][i]);
      }

      // Send to output as dependent on Algorithim
      output += operators[i].out * modMatrix[algo][i][4];
    }
    // Filter DC content from output
    if (blocking)
    {
      output = dcBlock.process(output);
    }
    // Check for NaN
    output = std::isfinite(output) ? output : 0.f;
    // Send output signal to output jack
    return (output * .8 * level);
  }

  void onReset()
  {
    blocking = true;
    looping = false;
  }
};