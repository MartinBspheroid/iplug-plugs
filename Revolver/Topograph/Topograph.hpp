//
// Topograph.cpp
// Author: Dale Johnson
// Contact: valley.audio.soft@gmail.com
// Date: 5/12/2017
//
// Topograph, a port of "Mutable Instruments Grids" for VCV Rack
// Original author: Emilie Gillet (emilie.o.gillet@gmail.com)
// https://github.com/pichenettes/eurorack/tree/master/grids
// Copyright 2012 Emilie Gillet.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once
//#include "../Valley.hpp"
//#include "../ValleyComponents.hpp"
#include "rack/dsp/digital.hpp"
#include "Oneshot.hpp"
#include "TopographPatternGenerator.hpp"
#include "IPlugMidi.h"
#include "IPlugEditorDelegate.h"
#include <iomanip> // setprecision
#include <sstream> // stringstream
#include <functional>
struct Topograph  {
   enum ParamIds {
       RESET_BUTTON_PARAM,
       RUN_BUTTON_PARAM,
       TEMPO_PARAM,
       MAPX_PARAM,
       MAPY_PARAM,
       CHAOS_PARAM,
       BD_DENS_PARAM,
       SN_DENS_PARAM,
       HH_DENS_PARAM,
       SWING_PARAM,
       NUM_PARAMS
   };

   enum InputIds {
       CLOCK_INPUT,
       RESET_INPUT,
       MAPX_CV,
       MAPY_CV,
       CHAOS_CV,
       BD_FILL_CV,
       SN_FILL_CV,
       HH_FILL_CV,
       SWING_CV,
       RUN_INPUT,
       NUM_INPUTS
   };

   enum OutputIds {
       BD_OUTPUT,
       SN_OUTPUT,
       HH_OUTPUT,
       BD_ACC_OUTPUT,
       SN_ACC_OUTPUT,
       HH_ACC_OUTPUT,
       NUM_OUTPUTS
   };

   enum LightIds {
       RUNNING_LIGHT,
       RESET_LIGHT,
       BD_LIGHT,
       SN_LIGHT,
       HH_LIGHT,
       NUM_LIGHTS
   };

   
   PatternGenerator grids;
   uint8_t numTicks;
   
   //bool initExtReset = true;
   int running = 0;
   //bool externalClockConnected = false;
   //bool inExternalClockMode = false;
   bool advStep = false;
   //long seqStep = 0;
   //float swing = 0.5;
   //float swingHighTempo = 0.0;
   //float swingLowTempo = 0.0;
   //long elapsedTicks = 0;

   float tempoParam = 0.0;
   float tempo = 120.0;
   float mapX = 0.0;
   float mapY = 0.0;
   float chaos = 0.0;
   float BDFill = 0.0;
   float SNFill = 0.0;
   float HHFill = 0.0;

   uint8_t state = 0;

   // LED Triggers
   Oneshot drumLED[3];
   const LightIds drumLEDIds[3] = {BD_LIGHT, SN_LIGHT, HH_LIGHT};
   Oneshot BDLed;
   Oneshot SNLed;
   Oneshot HHLed;
   Oneshot resetLed;
   Oneshot runningLed;

   // Drum Triggers
   Oneshot drumTriggers[6];
   bool gateState[6];
   bool outputs[6];
   const OutputIds outIDs[6] = {BD_OUTPUT, SN_OUTPUT, HH_OUTPUT,
                                BD_ACC_OUTPUT, SN_ACC_OUTPUT, HH_ACC_OUTPUT};
   const int OutMidiNoteNumber[3] = { 24, 26, 30 };
   enum SequencerMode {
       HENRI,
       ORIGINAL,
       EUCLIDEAN
   };
   SequencerMode sequencerMode = HENRI;
   int inEuclideanMode = 0;

   enum TriggerOutputMode {
       PULSE,
       GATE
   };
   TriggerOutputMode triggerOutputMode = PULSE;

   enum AccOutputMode {
       INDIVIDUAL_ACCENTS,
       ACC_CLK_RST
   };
   AccOutputMode accOutputMode = INDIVIDUAL_ACCENTS;

   enum ExtClockResolution {
       EXTCLOCK_RES_4_PPQN,
       EXTCLOCK_RES_8_PPQN,
       EXTCLOCK_RES_24_PPQN,
   };
   ExtClockResolution extClockResolution = EXTCLOCK_RES_24_PPQN;

   enum ChaosKnobMode {
       CHAOS,
       SWING
   };
   ChaosKnobMode chaosKnobMode = CHAOS;

   enum RunMode {
       TOGGLE,
       MOMENTARY
   };
   RunMode runMode = TOGGLE;

   int panelStyle;
   int textVisible = 1;

   Topograph();

   void Initialize(float sampleRate = 44100)
   {
     grids.reset();
     
     numTicks = ticks_granularity[2];
     srand(time(NULL));
     BDLed = Oneshot(0.1, sampleRate);
     SNLed = Oneshot(0.1, sampleRate);
     HHLed = Oneshot(0.1, sampleRate);
     resetLed = Oneshot(0.1, sampleRate);

     for (int i = 0; i < 6; ++i) {
       drumTriggers[i] = Oneshot(0.001, sampleRate);
       gateState[i] = false;
     }
     for (int i = 0; i < 3; ++i) {
       drumLED[i] = Oneshot(0.1, sampleRate);
     }
     panelStyle = 0;
     grids.setPatternMode(PATTERN_ORIGINAL);
     
     
   }

   //json_t* dataToJson() override;
   //void dataFromJson(json_t *rootJ) override;
   int current_step = -1;
   void process(/*const ProcessArgs &args*/);
   void onSampleRateChange(float sampleRate);
   void onReset();

   inline void updateOutputs()
   {
     if (triggerOutputMode == PULSE) {
       for (int i = 0; i < 6; ++i) {
         drumTriggers[i].process();
         if (drumTriggers[i].getState()) {
           outputs[outIDs[i]] = true;
         }
         else {
           outputs[outIDs[i]] = false;
         }
       }
     }
    
   }

   void setTempo(double BPM);
   void update(double mPPQPos, int nFrames, bool mTransportIsRunning);
   std::function<bool(iplug::IMidiMsg)> sendMidiMsg;
private:
  void stop();
  enum Divisions {
    One_Sixteen = 8,
    One_Eitght = 4,
    One_Four = 2,
    One_Half = 1,
    One_Bar = 2,
    Two_Bars = 1,

  };
  int division = Divisions::One_Sixteen*3;
};

