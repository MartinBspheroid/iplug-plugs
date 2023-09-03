#pragma once
//#include "tracy/Tracy.hpp"
#include "clouds/dsp/granular_processor.h"
#include "rack/dsp/ringbuffer.hpp"
#include "rack/dsp/common.hpp"
#include "rack/dsp/digital.hpp"
#include "rack/math.hpp"
#include "IPlugConstants.h"
#include "SA_DSP.h"
//#include "stmlib/dsp/sample_rate_converter.h"
//#include "resample.h"

namespace SA {


  using namespace rack::math;
  template <typename T, size_t S>
  struct EzBuffer {

  public:
    void init() {
      for (size_t i = 0; i < S; i++)
      {
        dataOut[i].samples[0] = 0;
        dataOut[i].samples[1] = 0;
      }
    }
    
    void reset() { iter = 0; }
    T read() { return dataOut[iter++]; }
    void write(T t) { dataIn[iter] = t; }
    bool full() { return iter == S; }
    T dataIn[S];
    T dataOut[S];
    const int size = S;
  private:

     
    int iter = 0;
  };


  EzBuffer<rack::dsp::Frame<2>, 32> buffer;
  

  class Smoke
  {
  public:


    
    Smoke()
    {
      const int memLen = 118784;
      const int ccmLen = 65536 - 128;
      processor.Init(block_mem, memLen, block_ccm, ccmLen, 48000);
      buffer.init();
      inputBuffer.clear();
      outputBuffer.clear();
      

    }
    ~Smoke()
    {
      
    }

    void setSampleRate(float sr) {
      processor.sample_rate_actual = sr;
      envFollower.Setup(sr, 10, 50);
      readyToPlay = true;
    }
    auto getParams() {
      return processor.mutable_parameters();
    }
    void process(iplug::sample** inputs, iplug::sample** outputs, int nFrames) {
      //ZoneScoped;
      
      for (size_t n = 0; n < nFrames; n++)
      {

        // process env follower
        envFollower.Process(inputs[0][n], inputs[1][n]);

        //process here!!
        if (buffer.full()) {
        
          processClouds();
        }
        // Get input
        if (!buffer.full()) {
          rack::dsp::Frame<2> inputFrame;
          inputFrame.samples[0] = inputs[0][n];
          inputFrame.samples[1] = inputs[1][n];
          buffer.write(inputFrame);

        }


        // Render frames


        // Set output


        rack::dsp::Frame<2> outputFrame = buffer.read();
        outputs[0][n] = outputFrame.samples[0];
        outputs[1][n] = outputFrame.samples[1];

      }
      
    };
    void processClouds() {
      //ZoneScoped;
        clouds::FloatFrame input[32] = {};
       
     
          // We might not fill all of the input buffer if there is a deficiency, but this cannot be avoided due to imprecisions between the input and output SRC.
          for (int i = 0; i < buffer.size; i++) {
            rack::dsp::Frame<2> frame = buffer.dataIn[i];
            
            input[i].l = frame.samples[0];
            input[i].r = frame.samples[1];

        }

        // Set up processor
        
        processor.set_num_channels(mono ? 1 : 2);
        processor.set_low_fidelity(lofi);
        
        processor.set_playback_mode(playbackmode);
        //set up env follower
        bool envTrigger = envFollower.getEnvelopeMono() > envThreshold ? true : false;
        processor.mutable_parameters()->trigger = envTrigger;
        processor.mutable_parameters()->gate = envTrigger;

        processor.Prepare();

        clouds::FloatFrame output[32];
        processor.Process(input, output, 32);

        // Convert output buffer
        
          rack::dsp::Frame<2> outputFrames[32];
          for (int i = 0; i < 32; i++) {


            rack::dsp::Frame<2> fram;
            fram.samples[0] = output[i].l;
            fram.samples[1] = output[i].r;
            buffer.dataOut[i] = fram;

          }
        
        buffer.reset();
      
    }
    clouds::PlaybackMode playbackmode = clouds::PLAYBACK_MODE_GRANULAR;
    float gain = 0.0;
    
    
    SA::EnvelopeFollower<2> envFollower;
    bool readyToPlay = false;
    float envThreshold = 1.0;
  private:
    int buffersize = 256;
    int currentbuffersize = 1;
    bool lofi = false;
    bool mono = false;
    bool freeze = false;
    
    float block_mem[118784];
    float block_ccm[65536 - 128];

    //uint8_t block_mem[118784];
    //uint8_t block_ccm[65536 - 128] __attribute__((section(".ccmdata")));

    clouds::GranularProcessor processor;
    rack::dsp::DoubleRingBuffer<rack::dsp::Frame<2>, 256> inputBuffer;
    rack::dsp::DoubleRingBuffer<rack::dsp::Frame<2>, 256> outputBuffer;


  
    
  };

}