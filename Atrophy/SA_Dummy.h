#pragma once

class SA_DUMMY
{
public:
  SA_DUMMY()
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

      inputSampleL *= gain;
      inputSampleR *= gain;

      *in1 = inputSampleL;
      *in2 = inputSampleR;

      *in1++;
      *in2++;

    }
  }
  ~SA_DUMMY()
  {
  }
  float gain = 0.5;
private:

};
