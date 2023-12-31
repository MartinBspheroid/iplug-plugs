#pragma once


#include <set>
#include <string>
#include <math.h>
#include "SA_Processor.h"

class SA_AW_Diode 
{
public:
  SA_AW_Diode() {
    Triode = 0.0;
    ClassAB = 0.0;
    ClassB = 0.0;
    DryWet = 0.0;

    postsine = sin(0.5);

    fpNShapeL = 0.0;
    fpNShapeR = 0.0;

      //case kParamA: vst_strncpy(text, "Triode", kVstMaxParamStrLen); break;
      //case kParamB: vst_strncpy(text, "Clas AB", kVstMaxParamStrLen); break;
      //case kParamC: vst_strncpy(text, "Clas B", kVstMaxParamStrLen); break;
      //case kParamD: vst_strncpy(text, "Dry/Wet", kVstMaxParamStrLen); break;

  };
  ~SA_AW_Diode() {};

  void processReplacing(iplug::sample** buffer, int channels, int nFrames)
  {
    auto* in1 = buffer[0];
    auto* in2 = buffer[1];
    //float* out1 = outputs[0];
    //float* out2 = outputs[1];
    double intensity = pow(Triode, 2) * 8.0;
    double triode = intensity;
    intensity += 0.001;
    double softcrossover = pow(ClassAB, 3) / 8.0;
    double hardcrossover = pow(ClassB, 7) / 8.0;
    double wet = DryWet;
    double dry = 1.0 - wet;
    int frames = nFrames;
    while (--frames>= 0)
    {
      long double inputSampleL = *in1;
      long double inputSampleR = *in2;
      if (inputSampleL < 1.2e-38 && -inputSampleL < 1.2e-38) {
        static int noisesource = 0;
        //this declares a variable before anything else is compiled. It won't keep assigning
        //it to 0 for every sample, it's as if the declaration doesn't exist in this context,
        //but it lets me add this denormalization fix in a single place rather than updating
        //it in three different locations. The variable isn't thread-safe but this is only
        //a random seed and we can share it with whatever.
        noisesource = noisesource % 1700021; noisesource++;
        int residue = noisesource * noisesource;
        residue = residue % 170003; residue *= residue;
        residue = residue % 17011; residue *= residue;
        residue = residue % 1709; residue *= residue;
        residue = residue % 173; residue *= residue;
        residue = residue % 17;
        double applyresidue = residue;
        applyresidue *= 0.00000001;
        applyresidue *= 0.00000001;
        inputSampleL = applyresidue;
      }
      if (inputSampleR < 1.2e-38 && -inputSampleR < 1.2e-38) {
        static int noisesource = 0;
        noisesource = noisesource % 1700021; noisesource++;
        int residue = noisesource * noisesource;
        residue = residue % 170003; residue *= residue;
        residue = residue % 17011; residue *= residue;
        residue = residue % 1709; residue *= residue;
        residue = residue % 173; residue *= residue;
        residue = residue % 17;
        double applyresidue = residue;
        applyresidue *= 0.00000001;
        applyresidue *= 0.00000001;
        inputSampleR = applyresidue;
        //this denormalization routine produces a white noise at -300 dB which the noise
        //shaping will interact with to produce a bipolar output, but the noise is actually
        //all positive. That should stop any variables from going denormal, and the routine
        //only kicks in if digital black is input. As a final touch, if you save to 24-bit
        //the silence will return to being digital black again.
      }
      double drySampleL = inputSampleL;
      double drySampleR = inputSampleR;

      if (triode > 0.0)
      {
        inputSampleL *= intensity;
        inputSampleR *= intensity;
        inputSampleL -= 0.5;
        inputSampleR -= 0.5;

        long double bridgerectifier = fabs(inputSampleL);
        if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
        bridgerectifier = sin(bridgerectifier);
        if (inputSampleL > 0) inputSampleL = bridgerectifier;
        else inputSampleL = -bridgerectifier;

        bridgerectifier = fabs(inputSampleR);
        if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
        bridgerectifier = sin(bridgerectifier);
        if (inputSampleR > 0) inputSampleR = bridgerectifier;
        else inputSampleR = -bridgerectifier;

        inputSampleL += postsine;
        inputSampleR += postsine;
        inputSampleL /= intensity;
        inputSampleR /= intensity;
      }

      if (softcrossover > 0.0)
      {
        long double bridgerectifier = fabs(inputSampleL);
        if (bridgerectifier > 0.0) bridgerectifier -= (softcrossover * (bridgerectifier + sqrt(bridgerectifier)));
        if (bridgerectifier < 0.0) bridgerectifier = 0;
        if (inputSampleL > 0.0) inputSampleL = bridgerectifier;
        else inputSampleL = -bridgerectifier;

        bridgerectifier = fabs(inputSampleR);
        if (bridgerectifier > 0.0) bridgerectifier -= (softcrossover * (bridgerectifier + sqrt(bridgerectifier)));
        if (bridgerectifier < 0.0) bridgerectifier = 0;
        if (inputSampleR > 0.0) inputSampleR = bridgerectifier;
        else inputSampleR = -bridgerectifier;
      }


      if (hardcrossover > 0.0)
      {
        long double bridgerectifier = fabs(inputSampleL);
        bridgerectifier -= hardcrossover;
        if (bridgerectifier < 0.0) bridgerectifier = 0.0;
        if (inputSampleL > 0.0) inputSampleL = bridgerectifier;
        else inputSampleL = -bridgerectifier;

        bridgerectifier = fabs(inputSampleR);
        bridgerectifier -= hardcrossover;
        if (bridgerectifier < 0.0) bridgerectifier = 0.0;
        if (inputSampleR > 0.0) inputSampleR = bridgerectifier;
        else inputSampleR = -bridgerectifier;
      }

      if (wet != 1.0) {
        inputSampleL = (inputSampleL * wet) + (drySampleL * dry);
        inputSampleR = (inputSampleR * wet) + (drySampleR * dry);
      }

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
      /**out1++;
      *out2++;*/
    }
  }

  void processDoubleReplacing(double** inputs, double** outputs, const int sampleFrames)
  {
    double* in1 = inputs[0];
    double* in2 = inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];
    double intensity = pow(Triode, 2) * 8.0;
    double triode = intensity;
    intensity += 0.001;
    double softcrossover = pow(ClassAB, 3) / 8.0;
    double hardcrossover = pow(ClassB, 7) / 8.0;
    double wet = DryWet;
    double dry = 1.0 - wet;

    int frames = sampleFrames;
    while (--frames >= 0)
    {
      long double inputSampleL = *in1;
      long double inputSampleR = *in2;
      if (inputSampleL < 1.2e-38 && -inputSampleL < 1.2e-38) {
        static int noisesource = 0;
        //this declares a variable before anything else is compiled. It won't keep assigning
        //it to 0 for every sample, it's as if the declaration doesn't exist in this context,
        //but it lets me add this denormalization fix in a single place rather than updating
        //it in three different locations. The variable isn't thread-safe but this is only
        //a random seed and we can share it with whatever.
        noisesource = noisesource % 1700021; noisesource++;
        int residue = noisesource * noisesource;
        residue = residue % 170003; residue *= residue;
        residue = residue % 17011; residue *= residue;
        residue = residue % 1709; residue *= residue;
        residue = residue % 173; residue *= residue;
        residue = residue % 17;
        double applyresidue = residue;
        applyresidue *= 0.00000001;
        applyresidue *= 0.00000001;
        inputSampleL = applyresidue;
      }
      if (inputSampleR < 1.2e-38 && -inputSampleR < 1.2e-38) {
        static int noisesource = 0;
        noisesource = noisesource % 1700021; noisesource++;
        int residue = noisesource * noisesource;
        residue = residue % 170003; residue *= residue;
        residue = residue % 17011; residue *= residue;
        residue = residue % 1709; residue *= residue;
        residue = residue % 173; residue *= residue;
        residue = residue % 17;
        double applyresidue = residue;
        applyresidue *= 0.00000001;
        applyresidue *= 0.00000001;
        inputSampleR = applyresidue;
        //this denormalization routine produces a white noise at -300 dB which the noise
        //shaping will interact with to produce a bipolar output, but the noise is actually
        //all positive. That should stop any variables from going denormal, and the routine
        //only kicks in if digital black is input. As a final touch, if you save to 24-bit
        //the silence will return to being digital black again.
      }
      double drySampleL = inputSampleL;
      double drySampleR = inputSampleR;

      if (triode > 0.0)
      {
        inputSampleL *= intensity;
        inputSampleR *= intensity;
        inputSampleL -= 0.5;
        inputSampleR -= 0.5;

        long double bridgerectifier = fabs(inputSampleL);
        if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
        bridgerectifier = sin(bridgerectifier);
        if (inputSampleL > 0) inputSampleL = bridgerectifier;
        else inputSampleL = -bridgerectifier;

        bridgerectifier = fabs(inputSampleR);
        if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
        bridgerectifier = sin(bridgerectifier);
        if (inputSampleR > 0) inputSampleR = bridgerectifier;
        else inputSampleR = -bridgerectifier;

        inputSampleL += postsine;
        inputSampleR += postsine;
        inputSampleL /= intensity;
        inputSampleR /= intensity;
      }

      if (softcrossover > 0.0)
      {
        long double bridgerectifier = fabs(inputSampleL);
        if (bridgerectifier > 0.0) bridgerectifier -= (softcrossover * (bridgerectifier + sqrt(bridgerectifier)));
        if (bridgerectifier < 0.0) bridgerectifier = 0;
        if (inputSampleL > 0.0) inputSampleL = bridgerectifier;
        else inputSampleL = -bridgerectifier;

        bridgerectifier = fabs(inputSampleR);
        if (bridgerectifier > 0.0) bridgerectifier -= (softcrossover * (bridgerectifier + sqrt(bridgerectifier)));
        if (bridgerectifier < 0.0) bridgerectifier = 0;
        if (inputSampleR > 0.0) inputSampleR = bridgerectifier;
        else inputSampleR = -bridgerectifier;
      }


      if (hardcrossover > 0.0)
      {
        long double bridgerectifier = fabs(inputSampleL);
        bridgerectifier -= hardcrossover;
        if (bridgerectifier < 0.0) bridgerectifier = 0.0;
        if (inputSampleL > 0.0) inputSampleL = bridgerectifier;
        else inputSampleL = -bridgerectifier;

        bridgerectifier = fabs(inputSampleR);
        bridgerectifier -= hardcrossover;
        if (bridgerectifier < 0.0) bridgerectifier = 0.0;
        if (inputSampleR > 0.0) inputSampleR = bridgerectifier;
        else inputSampleR = -bridgerectifier;
      }

      if (wet != 1.0) {
        inputSampleL = (inputSampleL * wet) + (drySampleL * dry);
        inputSampleR = (inputSampleR * wet) + (drySampleR * dry);
      }

      //stereo 64 bit dither, made small and tidy.
      int expon; frexp((double)inputSampleL, &expon);
      long double dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
      dither /= 536870912.0; //needs this to scale to 64 bit zone
      inputSampleL += (dither - fpNShapeL); fpNShapeL = dither;
      frexp((double)inputSampleR, &expon);
      dither = (rand() / (RAND_MAX * 7.737125245533627e+25)) * pow(2, expon + 62);
      dither /= 536870912.0; //needs this to scale to 64 bit zone
      inputSampleR += (dither - fpNShapeR); fpNShapeR = dither;
      //end 64 bit dither

      *out1 = inputSampleL;
      *out2 = inputSampleR;

      *in1++;
      *in2++;
      *out1++;
      *out2++;
    }
  }
  float Triode;
  float ClassAB;
  float ClassB;
  float DryWet;
private:
  
  long double fpNShapeL;
  long double fpNShapeR;
  //default stuff
  double postsine;


};
