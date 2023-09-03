#pragma once

#include "IPlugConstants.h"
#include "SA_FIlePlayer.h"
#include "Oscillator.h"

namespace dbg {
  using namespace  iplug;
  SA::FilePlayer player;
  FastSinOscillator<float> osc{ 0, 333 };

  uint32_t mRandSeed = 0;
  float Rand()
  {
    mRandSeed = mRandSeed * 0x0019660D + 0x3C6EF35F;
    uint32_t temp = ((mRandSeed >> 9) & 0x007FFFFF) | 0x3F800000;
    return (*reinterpret_cast<float*>(&temp)) * 2.f - 3.f;
  }
  enum class TestToneType
  {
    OSC,
    SAMPLE,
    NOISE,
    NOISE_STEREO
  };

  void loadDebugPlayer() {
    player.loadFile("C:\\Audio\\samples\\Breaks\\think.wav");
    player.setLoop(true);

  }
  TestToneType tone = TestToneType::OSC;
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) {
    const int nChans = 2;
    switch (tone)
    {
      case dbg::TestToneType::OSC:
      for (int s = 0; s < nFrames; s++) {
        float oscFrame = osc.Process();
        for (int c = 0; c < nChans; c++) {
          outputs[c][s] = oscFrame * 0.707;
        }
      }
      break;
    case dbg::TestToneType::SAMPLE:
      player.process(outputs, nFrames);
      break;
    case dbg::TestToneType::NOISE:
      for (int s = 0; s < nFrames; s++) {
        float noiseFrame = dbg::Rand();
        for (int c = 0; c < nChans; c++) {
          outputs[c][s] = noiseFrame * 0.707;
        }
      }
      break;
    default:
      break;
    case dbg::TestToneType::NOISE_STEREO:
      for (int s = 0; s < nFrames; s++) {
        for (int c = 0; c < nChans; c++) {
          outputs[c][s] = Rand() * 0.707;
        }
      }
      break;
    }

  }
 

}