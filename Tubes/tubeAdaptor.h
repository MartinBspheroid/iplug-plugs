#pragma once


/* link with  */
#include <math.h>
#include "IPlugConstants.h"
#include "tubesAll.h"

class TubeAdaptor {
  
private:
  class SIG0 {
  private:
    int fSampleRate;
    static float 	fWave0[2001];
    int 	idxfWave0;
  public:
    int getNumInputs() { return 0; }
    int getNumOutputs() { return 1; }
    void init(int sample_rate) {
      fSampleRate = sample_rate;
      idxfWave0 = 0;
    }
    void fill(int count, float output[]) {
      for (int i = 0; i < count; i++) {
        output[i] = fWave0[idxfWave0];
        // post processing
        idxfWave0 = (idxfWave0 + 1) % 2001;
      }
    }
    TubeModel model;
  };


  float 	fConst0;
  float 	fConst1;
  float 	fConst2;
  float 	fConst3;
  float 	fConst4;
  static float 	ftbl0[2001];
  float 	fConst5;
  float 	fConst6;
  float 	fConst7;
  float 	fRec2[2];
  float 	fRec1[3];
  float 	fConst8;
  float 	fConst9;
  float 	fRec0[2];
  int fSampleRate;
  SIG0 sig0;

public:
   void init(int sample_rate) {
    sig0.init(sample_rate);
    sig0.fill(2001, ftbl0);
    fSampleRate = sample_rate;
    fConst0 = std::min(192000.0f, std::max(1.0f, (float)fSampleRate));
    fConst1 = tanf((97.389372261283583f / fConst0));
    fConst2 = (1.0f / fConst1);
    fConst3 = (fConst2 + 1.0f);
    fConst4 = ((1.0f - fConst2) / fConst3);

    fConst5 = (1.0f / tanf((sig0.model.num1 / fConst0)));
    fConst6 = (1.0f - fConst5);
    fConst7 = (1.0f / (fConst5 + 1.0f));
    fConst8 = (1.0f / (fConst1 * fConst3));
    fConst9 = (0 - fConst8);
    for (int i = 0; i < 2; i++) fRec2[i] = 0;
    for (int i = 0; i < 3; i++) fRec1[i] = 0;
    for (int i = 0; i < 2; i++) fRec0[i] = 0;
  }
 

  virtual void compute(int count, iplug::sample* input, iplug::sample* output) {
    //zone1
    //zone2
    //zone2b
    //zone3
    
    iplug::sample* input0 = input;
    iplug::sample* output0 = output;
    //LoopGraphScalar
    for (int i = 0; i < count; i++) {
      float 	fTemp0 = (float)input0[i];
      fRec2[0] = (fConst7 * ((sig0.model.num2 * (fRec1[1] + fRec1[2])) - (fConst6 * fRec2[1])));


      /// add this line to models VVVVVVVV
      float 	fTemp1 = (200.0f * ((fTemp0 + fRec2[0]) + sig0.model.num5));


      float 	fTemp2 = std::max((float)0, std::min((float)1999, floorf(fTemp1)));


      float 	fTemp3 = ((int((0.0f < fTemp2))) ? 0.0f : ((int((fTemp2 < 1999.0f))) ? (fTemp1 - fTemp2) : 1999.0f));
      fRec1[0] = (((ftbl0[int(fTemp2)] * (1.0f - fTemp3)) + (fTemp3 * ftbl0[int((fTemp2 + 1.0f))])) + sig0.model.num3);
      fRec0[0] = ((sig0.model.num4 * ((fConst8 * fRec1[0]) + (fConst9 * fRec1[1]))) - (fConst4 * fRec0[1]));
      output0[i] = (iplug::sample)fRec0[0];
      // post processing
      fRec0[1] = fRec0[0];
      fRec1[2] = fRec1[1]; fRec1[1] = fRec1[0];
      fRec2[1] = fRec2[0];
    }
  }
};



float 	TubeAdaptor::ftbl0[2001];

