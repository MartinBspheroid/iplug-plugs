//
// Topograph.cpp
// Author: Dale Johnson
// Contact: valley.audio.soft@gmail.com
// Date: 5/12/2017
//

#include "Topograph.hpp"

Topograph::Topograph() {
}



void Topograph::onReset() {
  stop();
}


void Topograph::setTempo(double BPM)
{
 
}


void Topograph::update(double mPPQPos, int nFrames, bool isRunning)
{


  if (isRunning) {
    running = true;
    auto step_now = (int)(mPPQPos * division);
    
    if ((step_now) != current_step) {
      advStep = true;
      current_step = step_now;
    }
    
  }
  else {
    if (running) {
      stop();
    }
  }

  
  if (advStep) {
    grids.setMapX((uint8_t)(mapX * 255.0));
    grids.setMapY((uint8_t)(mapY * 255.0));
    grids.setBDDensity((uint8_t)(BDFill * 255.0));
    grids.setSDDensity((uint8_t)(SNFill * 255.0));
    grids.setHHDensity((uint8_t)(HHFill * 255.0));
    grids.setRandomness((uint8_t)(chaos * 255.0));

    grids.setEuclideanLength(0, (uint8_t)(mapX * 255.0));
    grids.setEuclideanLength(1, (uint8_t)(mapY * 255.0));
    grids.setEuclideanLength(2, (uint8_t)(chaos * 255.0));
    grids.setPatternMode(PATTERN_ORIGINAL);
    grids.tick(1);
    for (int i = 0; i < 6; ++i) {
      if (grids.getDrumState(i) && !gateState[i]) {
        iplug::IMidiMsg m;
        int velocity = i < 3 ? 64 : 120;
        m.MakeNoteOnMsg(OutMidiNoteNumber[i % 3], velocity, 0, 0);
        sendMidiMsg(m);
        //drumTriggers[i].trigger();
        
        gateState[i] = true;

        if (i < 3) {
          drumLED[i].trigger();
        }
      }
      if (!grids.getDrumState(i) && gateState[i])
      {
        iplug::IMidiMsg m;
        m.MakeNoteOffMsg(OutMidiNoteNumber[i % 3], 0, 0);
        sendMidiMsg(m);
        //drumTriggers[i].trigger();
        gateState[i] = false;
        grids.getBeat();
      }
    }
   
    advStep = false;
  }
  updateOutputs();
  //updateUI();
}


void Topograph::stop()
{
  running = false;
  advStep = false;
  current_step = -1;
  //send stop all channels
  for (size_t i = 0; i < 6; i++)
  {
    iplug::IMidiMsg m;
    m.MakeNoteOffMsg(OutMidiNoteNumber[i % 3], 0, 0);
    sendMidiMsg(m);
  }
  grids.reset();

}

void Topograph::onSampleRateChange(float sampleRate) {
  
  for (int i = 0; i < 3; ++i) {
    drumLED[i].setSampleRate(sampleRate);
  }
  resetLed.setSampleRate(sampleRate);
  for (int i = 0; i < 6; ++i) {
    drumTriggers[i].setSampleRate(sampleRate);
  }
}
