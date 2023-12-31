#pragma once 
#include <set>
#include <string>
#include <cmath>
#include "SA_Processor.h"

#define PI 3.141592653589793


class SA_AW_Coils 
 
{
public:
  SA_AW_Coils() {
    Saturation = 0.0;
    Core_DC = 0.5;
    DryWet = 1.0;
    for (int x = 0; x < 9; x++) { figureL[x] = 0.0; figureR[x] = 0.0; }
    fpd = 17;
  };
  ~SA_AW_Coils() {};
  int sampleRate = 44100;
  int getSampleRate() { return sampleRate; };
  void setSampleRate(const int _sampleRate) { sampleRate = _sampleRate; }
  void SA_AW_Coils::processReplacing(iplug::sample** buffer, int channels, int nFrames)
  {
    auto* in1 = buffer[0];
    auto* in2 = buffer[1];
    /*float* out1 = outputs[0];
    float* out2 = outputs[1];*/

    //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
    //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
    double boost = 1.0 - pow(Saturation, 2);
    if (boost < 0.001) boost = 0.001; //there's a divide, we can't have this be zero
    figureL[0] = figureR[0] = 600.0 / getSampleRate(); //fixed frequency, 600hz
    figureL[1] = figureR[1] = 0.023; //resonance
    double offset = (Core_DC * 2.0) - 1.0;
    double sinOffset = sin(offset); //we can cache this, it's expensive
    double wet = DryWet;
    double K = tan(PI * figureR[0]);
    double norm = 1.0 / (1.0 + K / figureR[1] + K * K);
    figureL[2] = figureR[2] = K / figureR[1] * norm;
    figureL[4] = figureR[4] = -figureR[2];
    figureL[5] = figureR[5] = 2.0 * (K * K - 1.0) * norm;
    figureL[6] = figureR[6] = (1.0 - K / figureR[1] + K * K) * norm;
    int frames = nFrames;
    while (--frames >= 0)
    {
      long double inputSampleL = *in1;
      long double inputSampleR = *in2;
      if (fabs(inputSampleL) < 1.18e-37) inputSampleL = fpd * 1.18e-37;
      if (fabs(inputSampleR) < 1.18e-37) inputSampleR = fpd * 1.18e-37;
      long double drySampleL = inputSampleL;
      long double drySampleR = inputSampleR;

      //long double tempSample = (inputSample * figure[2]) + figure[7];
      //figure[7] = -(tempSample * figure[5]) + figure[8];
      //figure[8] = (inputSample * figure[4]) - (tempSample * figure[6]);
      //inputSample = tempSample + sin(drySample-tempSample);
      //or
      //inputSample = tempSample + ((sin(((drySample-tempSample)/boost)+offset)-sinOffset)*boost);
      //
      //given a band limited inputSample, freq 600hz and Q of 0.023, this restores a lot of
      //the full frequencies but distorts like a real transformer. Purest case, and since
      //we are not using a high Q we can remove the extra sin/asin on the bi quad.


      long double tempSample = (inputSampleL * figureL[2]) + figureL[7];
      figureL[7] = -(tempSample * figureL[5]) + figureL[8];
      figureL[8] = (inputSampleL * figureL[4]) - (tempSample * figureL[6]);
      inputSampleL = tempSample + ((sin(((drySampleL - tempSample) / boost) + offset) - sinOffset) * boost);
      //given a band limited inputSample, freq 600hz and Q of 0.023, this restores a lot of
      //the full frequencies but distorts like a real transformer. Since
      //we are not using a high Q we can remove the extra sin/asin on the bi quad.

      tempSample = (inputSampleR * figureR[2]) + figureR[7];
      figureR[7] = -(tempSample * figureR[5]) + figureR[8];
      figureR[8] = (inputSampleR * figureR[4]) - (tempSample * figureR[6]);
      inputSampleR = tempSample + ((sin(((drySampleR - tempSample) / boost) + offset) - sinOffset) * boost);
      //given a band limited inputSample, freq 600hz and Q of 0.023, this restores a lot of
      //the full frequencies but distorts like a real transformer. Since
      //we are not using a high Q we can remove the extra sin/asin on the bi quad.

      if (wet != 1.0) {
        inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
        inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
      }

      //begin 32 bit stereo floating point dither
      int expon; frexpf((float)inputSampleL, &expon);
      fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
      inputSampleL += ((double(fpd) - uint32_t(0x7fffffff)) * 5.5e-36l * pow(2, expon + 62));
      frexpf((float)inputSampleR, &expon);
      fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
      inputSampleR += ((double(fpd) - uint32_t(0x7fffffff)) * 5.5e-36l * pow(2, expon + 62));
      //end 32 bit stereo floating point dither

      *in1 = inputSampleL;
      *in2 = inputSampleR;

      *in1++;
      *in2++;
 /*     *out1++;
      *out2++;*/
    }
  }

  void process(iplug::sample** inputs, iplug::sample** outputs, int channels, int nFrames) 
  {
    double* in1 = inputs[0];
    double* in2 = inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];

    //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
    //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
    double boost = 1.0 - pow(Saturation, 2);
    if (boost < 0.001) boost = 0.001; //there's a divide, we can't have this be zero
    figureL[0] = figureR[0] = 600.0 / getSampleRate(); //fixed frequency, 600hz
    figureL[1] = figureR[1] = 0.023; //resonance
    double offset = (Core_DC * 2.0) - 1.0;
    double sinOffset = sin(offset); //we can cache this, it's expensive
    double wet = DryWet;
    double K = tan(PI * figureR[0]);
    double norm = 1.0 / (1.0 + K / figureR[1] + K * K);
    figureL[2] = figureR[2] = K / figureR[1] * norm;
    figureL[4] = figureR[4] = -figureR[2];
    figureL[5] = figureR[5] = 2.0 * (K * K - 1.0) * norm;
    figureL[6] = figureR[6] = (1.0 - K / figureR[1] + K * K) * norm;

    while (--nFrames >= 0)
    {
      long double inputSampleL = *in1;
      long double inputSampleR = *in2;
      if (fabs(inputSampleL) < 1.18e-43) inputSampleL = fpd * 1.18e-43;
      if (fabs(inputSampleR) < 1.18e-43) inputSampleR = fpd * 1.18e-43;
      long double drySampleL = inputSampleL;
      long double drySampleR = inputSampleR;

      //long double tempSample = (inputSample * figure[2]) + figure[7];
      //figure[7] = -(tempSample * figure[5]) + figure[8];
      //figure[8] = (inputSample * figure[4]) - (tempSample * figure[6]);
      //inputSample = tempSample + sin(drySample-tempSample);
      //or
      //inputSample = tempSample + ((sin(((drySample-tempSample)/boost)+offset)-sinOffset)*boost);
      //
      //given a bandlimited inputSample, freq 600hz and Q of 0.023, this restores a lot of
      //the full frequencies but distorts like a real transformer. Purest case, and since
      //we are not using a high Q we can remove the extra sin/asin on the biquad.


      long double tempSample = (inputSampleL * figureL[2]) + figureL[7];
      figureL[7] = -(tempSample * figureL[5]) + figureL[8];
      figureL[8] = (inputSampleL * figureL[4]) - (tempSample * figureL[6]);
      inputSampleL = tempSample + ((sin(((drySampleL - tempSample) / boost) + offset) - sinOffset) * boost);
      //given a bandlimited inputSample, freq 600hz and Q of 0.023, this restores a lot of
      //the full frequencies but distorts like a real transformer. Since
      //we are not using a high Q we can remove the extra sin/asin on the biquad.

      tempSample = (inputSampleR * figureR[2]) + figureR[7];
      figureR[7] = -(tempSample * figureR[5]) + figureR[8];
      figureR[8] = (inputSampleR * figureR[4]) - (tempSample * figureR[6]);
      inputSampleR = tempSample + ((sin(((drySampleR - tempSample) / boost) + offset) - sinOffset) * boost);
      //given a bandlimited inputSample, freq 600hz and Q of 0.023, this restores a lot of
      //the full frequencies but distorts like a real transformer. Since
      //we are not using a high Q we can remove the extra sin/asin on the biquad.

      if (wet != 1.0) {
        inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
        inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
      }

      //begin 64 bit stereo floating point dither
      int expon; frexp((double)inputSampleL, &expon);
      fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
      inputSampleL += ((double(fpd) - uint32_t(0x7fffffff)) * 1.1e-44l * pow(2, expon + 62));
      frexp((double)inputSampleR, &expon);
      fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
      inputSampleR += ((double(fpd) - uint32_t(0x7fffffff)) * 1.1e-44l * pow(2, expon + 62));
      //end 64 bit stereo floating point dither

      *in1 = inputSampleL;
      *in2 = inputSampleR;

      *in1++;
      *in2++;
      //*out1++;
      //*out2++;
    }
  } 


  float Saturation;
  float Core_DC;
  float DryWet;
private:
  

  long double figureL[9];
  long double figureR[9];
  uint32_t fpd;
  //default stuff


};

