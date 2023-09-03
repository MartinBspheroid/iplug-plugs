#pragma once
#include <set>
#include <string>
#include <math.h>
#include "SA_Processor.h"

class SA_AW_PowerSag2 
{
public:
  SA_AW_PowerSag2() {
    for (int count = 0; count < 16385; count++) { dL[count] = 0; dR[count] = 0; }
    controlL = 0;
    controlR = 0;
    gcount = 0;

    Range = 0.3;
    Inv_Wet = 0.5;
    fpd = 17;
  };
  ~SA_AW_PowerSag2() {};
  void processReplacing(iplug::sample** buffer, int channels, int nFrames) 
  {
    auto* in1 = buffer[0];
    auto* in2 = buffer[1];
    /*auto* out1 = outputs[0];
    auto* out2 = outputs[1];*/

    double depth = pow(Range, 4);
    int offset = (int)(depth * 16383) + 1;
    double wet = (Inv_Wet * 2.0) - 1.0;
    int frames = nFrames;
    while (--frames>= 0)
    {
      long double inputSampleL = *in1;
      long double inputSampleR = *in2;
      if (fabs(inputSampleL) < 1.18e-37) inputSampleL = fpd * 1.18e-37;
      if (fabs(inputSampleR) < 1.18e-37) inputSampleR = fpd * 1.18e-37;
      long double drySampleL = inputSampleL;
      long double drySampleR = inputSampleR;

      if (gcount < 0 || gcount > 16384) { gcount = 16384; }
      dL[gcount] = fabs(inputSampleL);
      dR[gcount] = fabs(inputSampleR);
      controlL += dL[gcount];
      controlR += dR[gcount];
      controlL -= dL[gcount + offset - ((gcount + offset > 16384) ? 16384 : 0)];
      controlR -= dR[gcount + offset - ((gcount + offset > 16384) ? 16384 : 0)];
      gcount--;

      if (controlL > offset) controlL = offset; if (controlL < 0.0) controlL = 0.0;
      if (controlR > offset) controlR = offset; if (controlR < 0.0) controlR = 0.0;

      double burst = inputSampleL * (controlL / sqrt(offset));
      double clamp = inputSampleL / ((burst == 0.0) ? 1.0 : burst);

      if (clamp > 1.0) clamp = 1.0; if (clamp < 0.0) clamp = 0.0;
      inputSampleL *= clamp;
      double difference = drySampleL - inputSampleL;
      if (wet < 0.0) drySampleL *= (wet + 1.0);
      inputSampleL = drySampleL - (difference * wet);

      burst = inputSampleR * (controlR / sqrt(offset));
      clamp = inputSampleR / ((burst == 0.0) ? 1.0 : burst);

      if (clamp > 1.0) clamp = 1.0; if (clamp < 0.0) clamp = 0.0;
      inputSampleR *= clamp;
      difference = drySampleR - inputSampleR;
      if (wet < 0.0) drySampleR *= (wet + 1.0);
      inputSampleR = drySampleR - (difference * wet);

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
      //*out1++;
      //*out2++;
    }
  }

  void processDoubleReplacing(double** inputs, double** outputs, const int  sampleFrames)
  {
    double* in1 = inputs[0];
    double* in2 = inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];

    double depth = pow(Range, 4);
    int offset = (int)(depth * 16383) + 1;
    double wet = (Inv_Wet * 2.0) - 1.0;
    int frames = sampleFrames;
    while (--frames>= 0)
    {
      long double inputSampleL = *in1;
      long double inputSampleR = *in2;
      if (fabs(inputSampleL) < 1.18e-43) inputSampleL = fpd * 1.18e-43;
      if (fabs(inputSampleR) < 1.18e-43) inputSampleR = fpd * 1.18e-43;
      long double drySampleL = inputSampleL;
      long double drySampleR = inputSampleR;

      if (gcount < 0 || gcount > 16384) { gcount = 16384; }
      dL[gcount] = fabs(inputSampleL);
      dR[gcount] = fabs(inputSampleR);
      controlL += dL[gcount];
      controlR += dR[gcount];
      controlL -= dL[gcount + offset - ((gcount + offset > 16384) ? 16384 : 0)];
      controlR -= dR[gcount + offset - ((gcount + offset > 16384) ? 16384 : 0)];
      gcount--;

      if (controlL > offset) controlL = offset; if (controlL < 0.0) controlL = 0.0;
      if (controlR > offset) controlR = offset; if (controlR < 0.0) controlR = 0.0;

      double burst = inputSampleL * (controlL / sqrt(offset));
      double clamp = inputSampleL / ((burst == 0.0) ? 1.0 : burst);

      if (clamp > 1.0) clamp = 1.0; if (clamp < 0.0) clamp = 0.0;
      inputSampleL *= clamp;
      double difference = drySampleL - inputSampleL;
      if (wet < 0.0) drySampleL *= (wet + 1.0);
      inputSampleL = drySampleL - (difference * wet);

      burst = inputSampleR * (controlR / sqrt(offset));
      clamp = inputSampleR / ((burst == 0.0) ? 1.0 : burst);

      if (clamp > 1.0) clamp = 1.0; if (clamp < 0.0) clamp = 0.0;
      inputSampleR *= clamp;
      difference = drySampleR - inputSampleR;
      if (wet < 0.0) drySampleR *= (wet + 1.0);
      inputSampleR = drySampleR - (difference * wet);

      //begin 64 bit stereo floating point dither
      int expon; frexp((double)inputSampleL, &expon);
      fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
      inputSampleL += ((double(fpd) - uint32_t(0x7fffffff)) * 1.1e-44l * pow(2, expon + 62));
      frexp((double)inputSampleR, &expon);
      fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
      inputSampleR += ((double(fpd) - uint32_t(0x7fffffff)) * 1.1e-44l * pow(2, expon + 62));
      //end 64 bit stereo floating point dither

      *out1 = inputSampleL;
      *out2 = inputSampleR;

      *in1++;
      *in2++;
      *out1++;
      *out2++;
    }
  }
  float Range;
  float Inv_Wet;
private:

  double dL[16386];
  double dR[16386];
  double controlL;
  double controlR;
  int gcount;

  uint32_t fpd;
  //default stuff



  //case kParamA: vst_strncpy(text, "Range", kVstMaxParamStrLen); break;
  //case kParamB: vst_strncpy(text, "Inv/Wet", kVstMaxParamStrLen); break;
};

