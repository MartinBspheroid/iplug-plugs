#pragma once
#include "elements/dsp/part.h"
#include "rack/dsp/ringbuffer.hpp"
#include "rack/dsp/common.hpp"
#include "rack/dsp/digital.hpp"
#include "rack/math.hpp"
#include "IPlugConstants.h"

namespace SA {
  using namespace rack::math;

  
  class SA_Collider
  {
  public:
    SA_Collider()
    {

      part = new elements::Part();
      // In the Mutable Instruments code, Part doesn't initialize itself, so zero it here.
      memset(part, 0, sizeof(*part));
      part->Init(reverb_buffer);
      // Just some random numbers
      uint32_t seed[3] = { 1, 2, 3 };
      part->Seed(seed, 3);
      performanceState.gate = false;
      performanceState.modulation = 0.f;
      performanceState.note = 0;
      performanceState.strength = 0.5;
      part->mutable_patch()->modulation_frequency = 0.5;
      part->mutable_patch()->resonator_modulation_frequency = 0.2;
      part->mutable_patch()->resonator_modulation_offset = 0.1;
    }
    elements::Patch* getParams() { return part->mutable_patch(); }
    elements::PerformanceState* getPerformanceState() { return &performanceState; }
    void process(iplug::sample** outputs, int nFrames) {

      for (size_t n = 0; n < nFrames; n++)
      {
        //process here!!>	Collider.exe!SA::SA_Collider::SA_Collider() Line 51	C++

        if (outputBuffer.empty()) {
          // Render frames
          processElements();
        }
        // Get input
       
//         Set output
//

        if (!outputBuffer.empty()) {
          rack::dsp::Frame<2> outputFrame = outputBuffer.shift();
          outputs[0][n] = outputFrame.samples[0];
          outputs[1][n] = outputFrame.samples[1];
        }
      }

    }
    ~SA_Collider()
    {
      delete part;
    }


    void setModel(const int &model)
    {
      if (model < 0) {
        part->set_easter_egg(true);
      }
      else {
        part->set_easter_egg(false);
        part->set_resonator_model((elements::ResonatorModel)model);
      }
    }


  private:
    
    uint16_t reverb_buffer[32768*2] = {};
    elements::Part* part;
    elements::PerformanceState performanceState;
    rack::dsp::DoubleRingBuffer<rack::dsp::Frame<2>, 256> outputBuffer;



    void processElements()
    {
      float main[16];
      float aux[16];
      float blow[16] = {0.f};
      float strike[16] = {0.f};
      part->Process(performanceState, blow, strike, main, aux, elements::kMaxBlockSize);
      //float main[16];
      //float aux[16];
      rack::dsp::Frame<2> output[elements::kMaxBlockSize];
      for (int i = 0; i < elements::kMaxBlockSize; i++) {
        rack::dsp::Frame<2> frame;
        frame.samples[0] = main[i];
        frame.samples[1] = aux[i];

        outputBuffer.push(frame);

      }

    }


  };

}