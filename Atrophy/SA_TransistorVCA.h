#pragma once

#include <set>
#include <string>
#include <math.h>
#include "SA_Processor.h"


class SA_TransistorVCA
{
public:
  SA_TransistorVCA()
  {
    fpNShapeL = 0.0;
    fpNShapeR = 0.0;
  }

  ~SA_TransistorVCA()
  {
  }
  void processReplacing(iplug::sample** buffer, int channels, int nFrames) {

    auto* in1 = buffer[0];
    auto* in2 = buffer[1];
    int frames = nFrames;
    while (--frames >= 0)
    {
      long double inputSampleL = *in1;
      long double inputSampleR = *in2;

      inputSampleL = TransistorVCA(inputSampleL, gain);
      inputSampleR = TransistorVCA(inputSampleR, gain);

      inputSampleL = FastTanh(inputSampleL);
      inputSampleR = FastTanh(inputSampleR);

      //stereo 32 bit dither, made small and tidy.
      int expon; frexpf((float)inputSampleL, &expon);
      long double dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
      inputSampleL += (dither - fpNShapeL); fpNShapeL = dither;
      frexpf((float)inputSampleR, &expon);
      dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
      inputSampleR += (dither - fpNShapeR); fpNShapeR = dither;
      //end 32 bit dither

      *in1 = inputSampleL;
      *in2 = inputSampleR;

      *in1++;
      *in2++;
    }
  }

  float gain;
private:

  long double fpNShapeL;
  long double fpNShapeR;
  inline long double TransistorVCA(long double s, float gain) {
    s = (s - 0.6f) * gain;
    return 3.0f * s / (2.0f + fabsf(s)) + gain * 0.3f;
  }
  inline long double FastTanh(long double x) {
    auto x2 = x * x;
    auto numerator = x * (135135 + x2 * (17325 + x2 * (378 + x2)));
    auto denominator = 135135 + x2 * (62370 + x2 * (3150 + 28 * x2));
    return numerator / denominator;
  }
};
