#pragma once
#include <set>
#include <string>
#include <math.h>
#include "SA_Processor.h"

#define PI 3.141592653589793

class SA_AW_Tape6
{
public:


  float InputGain;
  float Soften;
  float HeadBump;
  float Flutter;
  float OutputGain;
  float DryWet;
  SA_AW_Tape6() {

    //case kParamA: vst_strncpy(text, "Input", kVstMaxParamStrLen); break;
    //case kParamB: vst_strncpy(text, "Soften", kVstMaxParamStrLen); break;
    //case kParamC: vst_strncpy(text, "Head B", kVstMaxParamStrLen); break;
    //case kParamD: vst_strncpy(text, "Flutter", kVstMaxParamStrLen); break;
    //case kParamE: vst_strncpy(text, "Output", kVstMaxParamStrLen); break;
    //case kParamF: vst_strncpy(text, "Dry/Wet", kVstMaxParamStrLen); break;


    //case kParamA: float2string((A - 0.5) * 24.0, text, kVstMaxParamStrLen); break;
    //case kParamB: float2string(B, text, kVstMaxParamStrLen); break;
    //case kParamC: float2string(C, text, kVstMaxParamStrLen); break;
    //case kParamD: float2string(D, text, kVstMaxParamStrLen); break;
    //case kParamE: float2string((E - 0.5) * 24.0, text, kVstMaxParamStrLen); break;
    //case kParamF: float2string(F, text, kVstMaxParamStrLen); break;
    InputGain = 0.5;
    Soften = 0.5;
    HeadBump = 0.5;
    Flutter = 0.5;
    OutputGain = 0.5;
    DryWet = 1.0;
    iirMidRollerAL = 0.0;
    iirMidRollerBL = 0.0;
    iirHeadBumpAL = 0.0;
    iirHeadBumpBL = 0.0;

    iirMidRollerAR = 0.0;
    iirMidRollerBR = 0.0;
    iirHeadBumpAR = 0.0;
    iirHeadBumpBR = 0.0;

    for (int x = 0; x < 9; x++) {
      biquadAL[x] = 0.0; biquadBL[x] = 0.0; biquadCL[x] = 0.0; biquadDL[x] = 0.0;
      biquadAR[x] = 0.0; biquadBR[x] = 0.0; biquadCR[x] = 0.0; biquadDR[x] = 0.0;
    }
    flip = false;
    for (int temp = 0; temp < 501; temp++) { dL[temp] = 0.0; dR[temp] = 0.0; }

    gcount = 0;
    rateof = 0.5;
    sweep = PI;
    nextmax = 0.5;
    lastSampleL = 0.0;
    lastSampleR = 0.0;
    flip = 0;

    fpd = 17;
  };
  ~SA_AW_Tape6() {};

  void setSampleRate(const int _sampleRate) { sampleRate = _sampleRate; }

  int getSampleRate() { return sampleRate; };
  void processReplacing(iplug::sample** buffer, int channels, int nFrames)
  {
    auto* in1 = buffer[0];
    auto* in2 = buffer[1];
    /*float* out1 = outputs[0];
    float* out2 = outputs[1];*/

    double overallscale = 1.0;
    overallscale /= 44100.0;
    overallscale *= getSampleRate();

    double inputgain = pow(10.0, ((InputGain - 0.5) * 24.0) / 20.0);
    double SoftenControl = pow(Soften, 2);
    double RollAmount = (1.0 - (SoftenControl * 0.45)) / overallscale;
    double HeadBumpControl = HeadBump * 0.25 * inputgain;
    double HeadBumpFreq = 0.12 / overallscale;
    //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
    //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
    biquadAL[0] = biquadBL[0] = biquadAR[0] = biquadBR[0] = 0.007 / overallscale;
    biquadAL[1] = biquadBL[1] = biquadAR[1] = biquadBR[1] = 0.0009;
    double K = tan(PI * biquadBR[0]);
    double norm = 1.0 / (1.0 + K / biquadBR[1] + K * K);
    biquadAL[2] = biquadBL[2] = biquadAR[2] = biquadBR[2] = K / biquadBR[1] * norm;
    biquadAL[4] = biquadBL[4] = biquadAR[4] = biquadBR[4] = -biquadBR[2];
    biquadAL[5] = biquadBL[5] = biquadAR[5] = biquadBR[5] = 2.0 * (K * K - 1.0) * norm;
    biquadAL[6] = biquadBL[6] = biquadAR[6] = biquadBR[6] = (1.0 - K / biquadBR[1] + K * K) * norm;

    biquadCL[0] = biquadDL[0] = biquadCR[0] = biquadDR[0] = 0.032 / overallscale;
    biquadCL[1] = biquadDL[1] = biquadCR[1] = biquadDR[1] = 0.0007;
    K = tan(PI * biquadDR[0]);
    norm = 1.0 / (1.0 + K / biquadDR[1] + K * K);
    biquadCL[2] = biquadDL[2] = biquadCR[2] = biquadDR[2] = K / biquadDR[1] * norm;
    biquadCL[4] = biquadDL[4] = biquadCR[4] = biquadDR[4] = -biquadDR[2];
    biquadCL[5] = biquadDL[5] = biquadCR[5] = biquadDR[5] = 2.0 * (K * K - 1.0) * norm;
    biquadCL[6] = biquadDL[6] = biquadCR[6] = biquadDR[6] = (1.0 - K / biquadDR[1] + K * K) * norm;

    double depth = pow(Flutter, 2) * overallscale * 70;
    double fluttertrim = (0.0024 * pow(Flutter, 2)) / overallscale;
    double outputgain = pow(10.0, ((OutputGain - 0.5) * 24.0) / 20.0);

    double refclip = 0.99;
    double softness = 0.618033988749894848204586;

    double wet = DryWet;

    int frames = nFrames;
    while (--frames >= 0) {

      long double inputSampleL = *in1;
      long double inputSampleR = *in2;
      if (fabs(inputSampleL) < 1.18e-37) inputSampleL = fpd * 1.18e-37;
      if (fabs(inputSampleR) < 1.18e-37) inputSampleR = fpd * 1.18e-37;
      long double drySampleL = inputSampleL;
      long double drySampleR = inputSampleR;

      if (inputgain < 1.0) {
        inputSampleL *= inputgain;
        inputSampleR *= inputgain;
      } //gain cut before plugin		

      double flutterrandy = fpd / (double)UINT32_MAX;
      //now we've got a random flutter, so we're messing with the pitch before tape effects go on
      if (gcount < 0 || gcount > 499) { gcount = 499; }
      dL[gcount] = inputSampleL;
      dR[gcount] = inputSampleR;
      int count = gcount;
      if (depth != 0.0) {

        long double offset = depth + (depth * pow(rateof, 2) * sin(sweep));

        count += (int)floor(offset);
        inputSampleL = (dL[count - ((count > 499) ? 500 : 0)] * (1 - (offset - floor(offset))));
        inputSampleR = (dR[count - ((count > 499) ? 500 : 0)] * (1 - (offset - floor(offset))));
        inputSampleL += (dL[count + 1 - ((count + 1 > 499) ? 500 : 0)] * (offset - floor(offset)));
        inputSampleR += (dR[count + 1 - ((count + 1 > 499) ? 500 : 0)] * (offset - floor(offset)));

        rateof = (rateof * (1.0 - fluttertrim)) + (nextmax * fluttertrim);
        sweep += rateof * fluttertrim;

        if (sweep >= (PI * 2.0)) {
          sweep -= PI;
          nextmax = 0.24 + (flutterrandy * 0.74);
        }
        //apply to input signal only when flutter is present, interpolate samples
      }
      gcount--;

      long double vibDrySampleL = inputSampleL;
      long double vibDrySampleR = inputSampleR;
      long double HighsSampleL = 0.0;
      long double HighsSampleR = 0.0;
      long double NonHighsSampleL = 0.0;
      long double NonHighsSampleR = 0.0;
      long double tempSample;

      if (flip)
      {
        iirMidRollerAL = (iirMidRollerAL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
        iirMidRollerAR = (iirMidRollerAR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
        HighsSampleL = inputSampleL - iirMidRollerAL;
        HighsSampleR = inputSampleR - iirMidRollerAR;
        NonHighsSampleL = iirMidRollerAL;
        NonHighsSampleR = iirMidRollerAR;

        iirHeadBumpAL += (inputSampleL * 0.05);
        iirHeadBumpAR += (inputSampleR * 0.05);
        iirHeadBumpAL -= (iirHeadBumpAL * iirHeadBumpAL * iirHeadBumpAL * HeadBumpFreq);
        iirHeadBumpAR -= (iirHeadBumpAR * iirHeadBumpAR * iirHeadBumpAR * HeadBumpFreq);
        iirHeadBumpAL = sin(iirHeadBumpAL);
        iirHeadBumpAR = sin(iirHeadBumpAR);

        tempSample = (iirHeadBumpAL * biquadAL[2]) + biquadAL[7];
        biquadAL[7] = (iirHeadBumpAL * biquadAL[3]) - (tempSample * biquadAL[5]) + biquadAL[8];
        biquadAL[8] = (iirHeadBumpAL * biquadAL[4]) - (tempSample * biquadAL[6]);
        iirHeadBumpAL = tempSample; //interleaved biquad
        if (iirHeadBumpAL > 1.0) iirHeadBumpAL = 1.0;
        if (iirHeadBumpAL < -1.0) iirHeadBumpAL = -1.0;
        iirHeadBumpAL = asin(iirHeadBumpAL);

        tempSample = (iirHeadBumpAR * biquadAR[2]) + biquadAR[7];
        biquadAR[7] = (iirHeadBumpAR * biquadAR[3]) - (tempSample * biquadAR[5]) + biquadAR[8];
        biquadAR[8] = (iirHeadBumpAR * biquadAR[4]) - (tempSample * biquadAR[6]);
        iirHeadBumpAR = tempSample; //interleaved biquad
        if (iirHeadBumpAR > 1.0) iirHeadBumpAR = 1.0;
        if (iirHeadBumpAR < -1.0) iirHeadBumpAR = -1.0;
        iirHeadBumpAR = asin(iirHeadBumpAR);

        inputSampleL = sin(inputSampleL);
        tempSample = (inputSampleL * biquadCL[2]) + biquadCL[7];
        biquadCL[7] = (inputSampleL * biquadCL[3]) - (tempSample * biquadCL[5]) + biquadCL[8];
        biquadCL[8] = (inputSampleL * biquadCL[4]) - (tempSample * biquadCL[6]);
        inputSampleL = tempSample; //interleaved biquad
        if (inputSampleL > 1.0) inputSampleL = 1.0;
        if (inputSampleL < -1.0) inputSampleL = -1.0;
        inputSampleL = asin(inputSampleL);

        inputSampleR = sin(inputSampleR);
        tempSample = (inputSampleR * biquadCR[2]) + biquadCR[7];
        biquadCR[7] = (inputSampleR * biquadCR[3]) - (tempSample * biquadCR[5]) + biquadCR[8];
        biquadCR[8] = (inputSampleR * biquadCR[4]) - (tempSample * biquadCR[6]);
        inputSampleR = tempSample; //interleaved biquad
        if (inputSampleR > 1.0) inputSampleR = 1.0;
        if (inputSampleR < -1.0) inputSampleR = -1.0;
        inputSampleR = asin(inputSampleR);
      }
      else {
        iirMidRollerBL = (iirMidRollerBL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
        iirMidRollerBR = (iirMidRollerBR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
        HighsSampleL = inputSampleL - iirMidRollerBL;
        HighsSampleR = inputSampleR - iirMidRollerBR;
        NonHighsSampleL = iirMidRollerBL;
        NonHighsSampleR = iirMidRollerBR;

        iirHeadBumpBL += (inputSampleL * 0.05);
        iirHeadBumpBR += (inputSampleR * 0.05);
        iirHeadBumpBL -= (iirHeadBumpBL * iirHeadBumpBL * iirHeadBumpBL * HeadBumpFreq);
        iirHeadBumpBR -= (iirHeadBumpBR * iirHeadBumpBR * iirHeadBumpBR * HeadBumpFreq);
        iirHeadBumpBL = sin(iirHeadBumpBL);
        iirHeadBumpBR = sin(iirHeadBumpBR);

        tempSample = (iirHeadBumpBL * biquadBL[2]) + biquadBL[7];
        biquadBL[7] = (iirHeadBumpBL * biquadBL[3]) - (tempSample * biquadBL[5]) + biquadBL[8];
        biquadBL[8] = (iirHeadBumpBL * biquadBL[4]) - (tempSample * biquadBL[6]);
        iirHeadBumpBL = tempSample; //interleaved biquad
        if (iirHeadBumpBL > 1.0) iirHeadBumpBL = 1.0;
        if (iirHeadBumpBL < -1.0) iirHeadBumpBL = -1.0;
        iirHeadBumpBL = asin(iirHeadBumpBL);

        tempSample = (iirHeadBumpBR * biquadBR[2]) + biquadBR[7];
        biquadBR[7] = (iirHeadBumpBR * biquadBR[3]) - (tempSample * biquadBR[5]) + biquadBR[8];
        biquadBR[8] = (iirHeadBumpBR * biquadBR[4]) - (tempSample * biquadBR[6]);
        iirHeadBumpBR = tempSample; //interleaved biquad
        if (iirHeadBumpBR > 1.0) iirHeadBumpBR = 1.0;
        if (iirHeadBumpBR < -1.0) iirHeadBumpBR = -1.0;
        iirHeadBumpBR = asin(iirHeadBumpBR);

        inputSampleL = sin(inputSampleL);
        tempSample = (inputSampleL * biquadDL[2]) + biquadDL[7];
        biquadDL[7] = (inputSampleL * biquadDL[3]) - (tempSample * biquadDL[5]) + biquadDL[8];
        biquadDL[8] = (inputSampleL * biquadDL[4]) - (tempSample * biquadDL[6]);
        inputSampleL = tempSample; //interleaved biquad
        if (inputSampleL > 1.0) inputSampleL = 1.0;
        if (inputSampleL < -1.0) inputSampleL = -1.0;
        inputSampleL = asin(inputSampleL);

        inputSampleR = sin(inputSampleR);
        tempSample = (inputSampleR * biquadDR[2]) + biquadDR[7];
        biquadDR[7] = (inputSampleR * biquadDR[3]) - (tempSample * biquadDR[5]) + biquadDR[8];
        biquadDR[8] = (inputSampleR * biquadDR[4]) - (tempSample * biquadDR[6]);
        inputSampleR = tempSample; //interleaved biquad
        if (inputSampleR > 1.0) inputSampleR = 1.0;
        if (inputSampleR < -1.0) inputSampleR = -1.0;
        inputSampleR = asin(inputSampleR);
      }
      flip = !flip;

      long double groundSampleL = vibDrySampleL - inputSampleL; //set up UnBox on fluttered audio
      long double groundSampleR = vibDrySampleR - inputSampleR; //set up UnBox on fluttered audio

      if (inputgain > 1.0) {
        inputSampleL *= inputgain;
        inputSampleR *= inputgain;
      }

      long double applySoften = fabs(HighsSampleL) * 1.57079633;
      if (applySoften > 1.57079633) applySoften = 1.57079633;
      applySoften = 1 - cos(applySoften);
      if (HighsSampleL > 0) inputSampleL -= applySoften;
      if (HighsSampleL < 0) inputSampleL += applySoften;
      //apply Soften depending on polarity
      applySoften = fabs(HighsSampleR) * 1.57079633;
      if (applySoften > 1.57079633) applySoften = 1.57079633;
      applySoften = 1 - cos(applySoften);
      if (HighsSampleR > 0) inputSampleR -= applySoften;
      if (HighsSampleR < 0) inputSampleR += applySoften;
      //apply Soften depending on polarity

      double suppress = (1.0 - fabs(inputSampleL)) * 0.00013;
      if (iirHeadBumpAL > suppress) iirHeadBumpAL -= suppress;
      if (iirHeadBumpAL < -suppress) iirHeadBumpAL += suppress;
      if (iirHeadBumpBL > suppress) iirHeadBumpBL -= suppress;
      if (iirHeadBumpBL < -suppress) iirHeadBumpBL += suppress;
      //restrain resonant quality of head bump algorithm
      suppress = (1.0 - fabs(inputSampleR)) * 0.00013;
      if (iirHeadBumpAR > suppress) iirHeadBumpAR -= suppress;
      if (iirHeadBumpAR < -suppress) iirHeadBumpAR += suppress;
      if (iirHeadBumpBR > suppress) iirHeadBumpBR -= suppress;
      if (iirHeadBumpBR < -suppress) iirHeadBumpBR += suppress;
      //restrain resonant quality of head bump algorithm

      inputSampleL += ((iirHeadBumpAL + iirHeadBumpBL) * HeadBumpControl);
      inputSampleR += ((iirHeadBumpAR + iirHeadBumpBR) * HeadBumpControl);
      //apply Fatten.

      if (inputSampleL > 1.0) inputSampleL = 1.0;
      if (inputSampleL < -1.0) inputSampleL = -1.0;
      long double mojo; mojo = pow(fabs(inputSampleL), 0.25);
      if (mojo > 0.0) inputSampleL = (sin(inputSampleL * mojo * PI * 0.5) / mojo);
      //mojo is the one that flattens WAAAAY out very softly before wavefolding		

      if (inputSampleR > 1.0) inputSampleR = 1.0;
      if (inputSampleR < -1.0) inputSampleR = -1.0;
      mojo = pow(fabs(inputSampleR), 0.25);
      if (mojo > 0.0) inputSampleR = (sin(inputSampleR * mojo * PI * 0.5) / mojo);
      //mojo is the one that flattens WAAAAY out very softly before wavefolding		
      inputSampleL += groundSampleL; //apply UnBox processing
      inputSampleR += groundSampleR; //apply UnBox processing

      if (outputgain != 1.0) {
        inputSampleL *= outputgain;
        inputSampleR *= outputgain;
      }

      if (lastSampleL >= refclip)
      {
        if (inputSampleL < refclip) lastSampleL = ((refclip * softness) + (inputSampleL * (1.0 - softness)));
        else lastSampleL = refclip;
      }

      if (lastSampleL <= -refclip)
      {
        if (inputSampleL > -refclip) lastSampleL = ((-refclip * softness) + (inputSampleL * (1.0 - softness)));
        else lastSampleL = -refclip;
      }

      if (inputSampleL > refclip)
      {
        if (lastSampleL < refclip) inputSampleL = ((refclip * softness) + (lastSampleL * (1.0 - softness)));
        else inputSampleL = refclip;
      }

      if (inputSampleL < -refclip)
      {
        if (lastSampleL > -refclip) inputSampleL = ((-refclip * softness) + (lastSampleL * (1.0 - softness)));
        else inputSampleL = -refclip;
      }
      lastSampleL = inputSampleL; //end ADClip L


      if (lastSampleR >= refclip)
      {
        if (inputSampleR < refclip) lastSampleR = ((refclip * softness) + (inputSampleR * (1.0 - softness)));
        else lastSampleR = refclip;
      }

      if (lastSampleR <= -refclip)
      {
        if (inputSampleR > -refclip) lastSampleR = ((-refclip * softness) + (inputSampleR * (1.0 - softness)));
        else lastSampleR = -refclip;
      }

      if (inputSampleR > refclip)
      {
        if (lastSampleR < refclip) inputSampleR = ((refclip * softness) + (lastSampleR * (1.0 - softness)));
        else inputSampleR = refclip;
      }

      if (inputSampleR < -refclip)
      {
        if (lastSampleR > -refclip) inputSampleR = ((-refclip * softness) + (lastSampleR * (1.0 - softness)));
        else inputSampleR = -refclip;
      }
      lastSampleR = inputSampleR; //end ADClip R		

      if (inputSampleL > refclip) inputSampleL = refclip;
      if (inputSampleL < -refclip) inputSampleL = -refclip;
      //final iron bar
      if (inputSampleR > refclip) inputSampleR = refclip;
      if (inputSampleR < -refclip) inputSampleR = -refclip;
      //final iron bar

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
      /**out1++;
      *out2++;*/
    }
  }

  void processDoubleReplacing(double** inputs, double** outputs, const int  sampleFrames)
  {
    double* in1 = inputs[0];
    double* in2 = inputs[1];
    double* out1 = outputs[0];
    double* out2 = outputs[1];

    double overallscale = 1.0;
    overallscale /= 44100.0;
    overallscale *= getSampleRate();

    double inputgain = pow(10.0, ((InputGain - 0.5) * 24.0) / 20.0);
    double SoftenControl = pow(Soften, 2);
    double RollAmount = (1.0 - (SoftenControl * 0.45)) / overallscale;
    double HeadBumpControl = HeadBump * 0.25 * inputgain;
    double HeadBumpFreq = 0.12 / overallscale;
    //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
    //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
    biquadAL[0] = biquadBL[0] = biquadAR[0] = biquadBR[0] = 0.007 / overallscale;
    biquadAL[1] = biquadBL[1] = biquadAR[1] = biquadBR[1] = 0.0009;
    double K = tan(PI * biquadBR[0]);
    double norm = 1.0 / (1.0 + K / biquadBR[1] + K * K);
    biquadAL[2] = biquadBL[2] = biquadAR[2] = biquadBR[2] = K / biquadBR[1] * norm;
    biquadAL[4] = biquadBL[4] = biquadAR[4] = biquadBR[4] = -biquadBR[2];
    biquadAL[5] = biquadBL[5] = biquadAR[5] = biquadBR[5] = 2.0 * (K * K - 1.0) * norm;
    biquadAL[6] = biquadBL[6] = biquadAR[6] = biquadBR[6] = (1.0 - K / biquadBR[1] + K * K) * norm;

    biquadCL[0] = biquadDL[0] = biquadCR[0] = biquadDR[0] = 0.032 / overallscale;
    biquadCL[1] = biquadDL[1] = biquadCR[1] = biquadDR[1] = 0.0007;
    K = tan(PI * biquadDR[0]);
    norm = 1.0 / (1.0 + K / biquadDR[1] + K * K);
    biquadCL[2] = biquadDL[2] = biquadCR[2] = biquadDR[2] = K / biquadDR[1] * norm;
    biquadCL[4] = biquadDL[4] = biquadCR[4] = biquadDR[4] = -biquadDR[2];
    biquadCL[5] = biquadDL[5] = biquadCR[5] = biquadDR[5] = 2.0 * (K * K - 1.0) * norm;
    biquadCL[6] = biquadDL[6] = biquadCR[6] = biquadDR[6] = (1.0 - K / biquadDR[1] + K * K) * norm;

    double depth = pow(Flutter, 2) * overallscale * 70;
    double fluttertrim = (0.0024 * pow(Flutter, 2)) / overallscale;
    double outputgain = pow(10.0, ((OutputGain - 0.5) * 24.0) / 20.0);

    double refclip = 0.99;
    double softness = 0.618033988749894848204586;

    double wet = DryWet;

    int frames = sampleFrames;
    while (--frames >= 0) {

      long double inputSampleL = *in1;
      long double inputSampleR = *in2;
      if (fabs(inputSampleL) < 1.18e-43) inputSampleL = fpd * 1.18e-43;
      if (fabs(inputSampleR) < 1.18e-43) inputSampleR = fpd * 1.18e-43;
      long double drySampleL = inputSampleL;
      long double drySampleR = inputSampleR;

      if (inputgain < 1.0) {
        inputSampleL *= inputgain;
        inputSampleR *= inputgain;
      } //gain cut before plugin		

      double flutterrandy = fpd / (double)UINT32_MAX;
      //now we've got a random flutter, so we're messing with the pitch before tape effects go on
      if (gcount < 0 || gcount > 499) { gcount = 499; }
      dL[gcount] = inputSampleL;
      dR[gcount] = inputSampleR;
      int count = gcount;
      if (depth != 0.0) {

        long double offset = depth + (depth * pow(rateof, 2) * sin(sweep));

        count += (int)floor(offset);
        inputSampleL = (dL[count - ((count > 499) ? 500 : 0)] * (1 - (offset - floor(offset))));
        inputSampleR = (dR[count - ((count > 499) ? 500 : 0)] * (1 - (offset - floor(offset))));
        inputSampleL += (dL[count + 1 - ((count + 1 > 499) ? 500 : 0)] * (offset - floor(offset)));
        inputSampleR += (dR[count + 1 - ((count + 1 > 499) ? 500 : 0)] * (offset - floor(offset)));

        rateof = (rateof * (1.0 - fluttertrim)) + (nextmax * fluttertrim);
        sweep += rateof * fluttertrim;

        if (sweep >= (PI * 2.0)) {
          sweep -= PI;
          nextmax = 0.24 + (flutterrandy * 0.74);
        }
        //apply to input signal only when flutter is present, interpolate samples
      }
      gcount--;

      long double vibDrySampleL = inputSampleL;
      long double vibDrySampleR = inputSampleR;
      long double HighsSampleL = 0.0;
      long double HighsSampleR = 0.0;
      long double NonHighsSampleL = 0.0;
      long double NonHighsSampleR = 0.0;
      long double tempSample;

      if (flip)
      {
        iirMidRollerAL = (iirMidRollerAL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
        iirMidRollerAR = (iirMidRollerAR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
        HighsSampleL = inputSampleL - iirMidRollerAL;
        HighsSampleR = inputSampleR - iirMidRollerAR;
        NonHighsSampleL = iirMidRollerAL;
        NonHighsSampleR = iirMidRollerAR;

        iirHeadBumpAL += (inputSampleL * 0.05);
        iirHeadBumpAR += (inputSampleR * 0.05);
        iirHeadBumpAL -= (iirHeadBumpAL * iirHeadBumpAL * iirHeadBumpAL * HeadBumpFreq);
        iirHeadBumpAR -= (iirHeadBumpAR * iirHeadBumpAR * iirHeadBumpAR * HeadBumpFreq);
        iirHeadBumpAL = sin(iirHeadBumpAL);
        iirHeadBumpAR = sin(iirHeadBumpAR);

        tempSample = (iirHeadBumpAL * biquadAL[2]) + biquadAL[7];
        biquadAL[7] = (iirHeadBumpAL * biquadAL[3]) - (tempSample * biquadAL[5]) + biquadAL[8];
        biquadAL[8] = (iirHeadBumpAL * biquadAL[4]) - (tempSample * biquadAL[6]);
        iirHeadBumpAL = tempSample; //interleaved biquad
        if (iirHeadBumpAL > 1.0) iirHeadBumpAL = 1.0;
        if (iirHeadBumpAL < -1.0) iirHeadBumpAL = -1.0;
        iirHeadBumpAL = asin(iirHeadBumpAL);

        tempSample = (iirHeadBumpAR * biquadAR[2]) + biquadAR[7];
        biquadAR[7] = (iirHeadBumpAR * biquadAR[3]) - (tempSample * biquadAR[5]) + biquadAR[8];
        biquadAR[8] = (iirHeadBumpAR * biquadAR[4]) - (tempSample * biquadAR[6]);
        iirHeadBumpAR = tempSample; //interleaved biquad
        if (iirHeadBumpAR > 1.0) iirHeadBumpAR = 1.0;
        if (iirHeadBumpAR < -1.0) iirHeadBumpAR = -1.0;
        iirHeadBumpAR = asin(iirHeadBumpAR);

        inputSampleL = sin(inputSampleL);
        tempSample = (inputSampleL * biquadCL[2]) + biquadCL[7];
        biquadCL[7] = (inputSampleL * biquadCL[3]) - (tempSample * biquadCL[5]) + biquadCL[8];
        biquadCL[8] = (inputSampleL * biquadCL[4]) - (tempSample * biquadCL[6]);
        inputSampleL = tempSample; //interleaved biquad
        if (inputSampleL > 1.0) inputSampleL = 1.0;
        if (inputSampleL < -1.0) inputSampleL = -1.0;
        inputSampleL = asin(inputSampleL);

        inputSampleR = sin(inputSampleR);
        tempSample = (inputSampleR * biquadCR[2]) + biquadCR[7];
        biquadCR[7] = (inputSampleR * biquadCR[3]) - (tempSample * biquadCR[5]) + biquadCR[8];
        biquadCR[8] = (inputSampleR * biquadCR[4]) - (tempSample * biquadCR[6]);
        inputSampleR = tempSample; //interleaved biquad
        if (inputSampleR > 1.0) inputSampleR = 1.0;
        if (inputSampleR < -1.0) inputSampleR = -1.0;
        inputSampleR = asin(inputSampleR);
      }
      else {
        iirMidRollerBL = (iirMidRollerBL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
        iirMidRollerBR = (iirMidRollerBR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
        HighsSampleL = inputSampleL - iirMidRollerBL;
        HighsSampleR = inputSampleR - iirMidRollerBR;
        NonHighsSampleL = iirMidRollerBL;
        NonHighsSampleR = iirMidRollerBR;

        iirHeadBumpBL += (inputSampleL * 0.05);
        iirHeadBumpBR += (inputSampleR * 0.05);
        iirHeadBumpBL -= (iirHeadBumpBL * iirHeadBumpBL * iirHeadBumpBL * HeadBumpFreq);
        iirHeadBumpBR -= (iirHeadBumpBR * iirHeadBumpBR * iirHeadBumpBR * HeadBumpFreq);
        iirHeadBumpBL = sin(iirHeadBumpBL);
        iirHeadBumpBR = sin(iirHeadBumpBR);

        tempSample = (iirHeadBumpBL * biquadBL[2]) + biquadBL[7];
        biquadBL[7] = (iirHeadBumpBL * biquadBL[3]) - (tempSample * biquadBL[5]) + biquadBL[8];
        biquadBL[8] = (iirHeadBumpBL * biquadBL[4]) - (tempSample * biquadBL[6]);
        iirHeadBumpBL = tempSample; //interleaved biquad
        if (iirHeadBumpBL > 1.0) iirHeadBumpBL = 1.0;
        if (iirHeadBumpBL < -1.0) iirHeadBumpBL = -1.0;
        iirHeadBumpBL = asin(iirHeadBumpBL);

        tempSample = (iirHeadBumpBR * biquadBR[2]) + biquadBR[7];
        biquadBR[7] = (iirHeadBumpBR * biquadBR[3]) - (tempSample * biquadBR[5]) + biquadBR[8];
        biquadBR[8] = (iirHeadBumpBR * biquadBR[4]) - (tempSample * biquadBR[6]);
        iirHeadBumpBR = tempSample; //interleaved biquad
        if (iirHeadBumpBR > 1.0) iirHeadBumpBR = 1.0;
        if (iirHeadBumpBR < -1.0) iirHeadBumpBR = -1.0;
        iirHeadBumpBR = asin(iirHeadBumpBR);

        inputSampleL = sin(inputSampleL);
        tempSample = (inputSampleL * biquadDL[2]) + biquadDL[7];
        biquadDL[7] = (inputSampleL * biquadDL[3]) - (tempSample * biquadDL[5]) + biquadDL[8];
        biquadDL[8] = (inputSampleL * biquadDL[4]) - (tempSample * biquadDL[6]);
        inputSampleL = tempSample; //interleaved biquad
        if (inputSampleL > 1.0) inputSampleL = 1.0;
        if (inputSampleL < -1.0) inputSampleL = -1.0;
        inputSampleL = asin(inputSampleL);

        inputSampleR = sin(inputSampleR);
        tempSample = (inputSampleR * biquadDR[2]) + biquadDR[7];
        biquadDR[7] = (inputSampleR * biquadDR[3]) - (tempSample * biquadDR[5]) + biquadDR[8];
        biquadDR[8] = (inputSampleR * biquadDR[4]) - (tempSample * biquadDR[6]);
        inputSampleR = tempSample; //interleaved biquad
        if (inputSampleR > 1.0) inputSampleR = 1.0;
        if (inputSampleR < -1.0) inputSampleR = -1.0;
        inputSampleR = asin(inputSampleR);
      }
      flip = !flip;

      long double groundSampleL = vibDrySampleL - inputSampleL; //set up UnBox on fluttered audio
      long double groundSampleR = vibDrySampleR - inputSampleR; //set up UnBox on fluttered audio

      if (inputgain > 1.0) {
        inputSampleL *= inputgain;
        inputSampleR *= inputgain;
      }

      long double applySoften = fabs(HighsSampleL) * 1.57079633;
      if (applySoften > 1.57079633) applySoften = 1.57079633;
      applySoften = 1 - cos(applySoften);
      if (HighsSampleL > 0) inputSampleL -= applySoften;
      if (HighsSampleL < 0) inputSampleL += applySoften;
      //apply Soften depending on polarity
      applySoften = fabs(HighsSampleR) * 1.57079633;
      if (applySoften > 1.57079633) applySoften = 1.57079633;
      applySoften = 1 - cos(applySoften);
      if (HighsSampleR > 0) inputSampleR -= applySoften;
      if (HighsSampleR < 0) inputSampleR += applySoften;
      //apply Soften depending on polarity

      double suppress = (1.0 - fabs(inputSampleL)) * 0.00013;
      if (iirHeadBumpAL > suppress) iirHeadBumpAL -= suppress;
      if (iirHeadBumpAL < -suppress) iirHeadBumpAL += suppress;
      if (iirHeadBumpBL > suppress) iirHeadBumpBL -= suppress;
      if (iirHeadBumpBL < -suppress) iirHeadBumpBL += suppress;
      //restrain resonant quality of head bump algorithm
      suppress = (1.0 - fabs(inputSampleR)) * 0.00013;
      if (iirHeadBumpAR > suppress) iirHeadBumpAR -= suppress;
      if (iirHeadBumpAR < -suppress) iirHeadBumpAR += suppress;
      if (iirHeadBumpBR > suppress) iirHeadBumpBR -= suppress;
      if (iirHeadBumpBR < -suppress) iirHeadBumpBR += suppress;
      //restrain resonant quality of head bump algorithm

      inputSampleL += ((iirHeadBumpAL + iirHeadBumpBL) * HeadBumpControl);
      inputSampleR += ((iirHeadBumpAR + iirHeadBumpBR) * HeadBumpControl);
      //apply Fatten.

      if (inputSampleL > 1.0) inputSampleL = 1.0;
      if (inputSampleL < -1.0) inputSampleL = -1.0;
      long double mojo; mojo = pow(fabs(inputSampleL), 0.25);
      if (mojo > 0.0) inputSampleL = (sin(inputSampleL * mojo * PI * 0.5) / mojo);
      //mojo is the one that flattens WAAAAY out very softly before wavefolding		

      if (inputSampleR > 1.0) inputSampleR = 1.0;
      if (inputSampleR < -1.0) inputSampleR = -1.0;
      mojo = pow(fabs(inputSampleR), 0.25);
      if (mojo > 0.0) inputSampleR = (sin(inputSampleR * mojo * PI * 0.5) / mojo);
      //mojo is the one that flattens WAAAAY out very softly before wavefolding		

      inputSampleL += groundSampleL; //apply UnBox processing
      inputSampleR += groundSampleR; //apply UnBox processing

      if (outputgain != 1.0) {
        inputSampleL *= outputgain;
        inputSampleR *= outputgain;
      }

      if (lastSampleL >= refclip)
      {
        if (inputSampleL < refclip) lastSampleL = ((refclip * softness) + (inputSampleL * (1.0 - softness)));
        else lastSampleL = refclip;
      }

      if (lastSampleL <= -refclip)
      {
        if (inputSampleL > -refclip) lastSampleL = ((-refclip * softness) + (inputSampleL * (1.0 - softness)));
        else lastSampleL = -refclip;
      }

      if (inputSampleL > refclip)
      {
        if (lastSampleL < refclip) inputSampleL = ((refclip * softness) + (lastSampleL * (1.0 - softness)));
        else inputSampleL = refclip;
      }

      if (inputSampleL < -refclip)
      {
        if (lastSampleL > -refclip) inputSampleL = ((-refclip * softness) + (lastSampleL * (1.0 - softness)));
        else inputSampleL = -refclip;
      }
      lastSampleL = inputSampleL; //end ADClip L


      if (lastSampleR >= refclip)
      {
        if (inputSampleR < refclip) lastSampleR = ((refclip * softness) + (inputSampleR * (1.0 - softness)));
        else lastSampleR = refclip;
      }

      if (lastSampleR <= -refclip)
      {
        if (inputSampleR > -refclip) lastSampleR = ((-refclip * softness) + (inputSampleR * (1.0 - softness)));
        else lastSampleR = -refclip;
      }

      if (inputSampleR > refclip)
      {
        if (lastSampleR < refclip) inputSampleR = ((refclip * softness) + (lastSampleR * (1.0 - softness)));
        else inputSampleR = refclip;
      }

      if (inputSampleR < -refclip)
      {
        if (lastSampleR > -refclip) inputSampleR = ((-refclip * softness) + (lastSampleR * (1.0 - softness)));
        else inputSampleR = -refclip;
      }
      lastSampleR = inputSampleR; //end ADClip R		

      if (inputSampleL > refclip) inputSampleL = refclip;
      if (inputSampleL < -refclip) inputSampleL = -refclip;
      //final iron bar
      if (inputSampleR > refclip) inputSampleR = refclip;
      if (inputSampleR < -refclip) inputSampleR = -refclip;
      //final iron bar

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

      *out1 = inputSampleL;
      *out2 = inputSampleR;

      *in1++;
      *in2++;
      *out1++;
      *out2++;
    }
  }


private:

  int sampleRate = 44100;

  double dL[502];
  double dR[502];
  int gcount;
  double rateof;
  double sweep;
  double nextmax;

  double iirMidRollerAL;
  double iirMidRollerBL;
  double iirHeadBumpAL;
  double iirHeadBumpBL;

  double iirMidRollerAR;
  double iirMidRollerBR;
  double iirHeadBumpAR;
  double iirHeadBumpBR;
  long double biquadAL[9];
  long double biquadBL[9];
  long double biquadCL[9];
  long double biquadDL[9];

  long double biquadAR[9];
  long double biquadBR[9];
  long double biquadCR[9];
  long double biquadDR[9];
  bool flip;

  long double lastSampleL;
  long double lastSampleR;

  uint32_t fpd;
  //default stuff


};
