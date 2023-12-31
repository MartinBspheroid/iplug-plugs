#pragma once 
#include <set>
#include <string>
#include <cmath>
#include "SA_Processor.h"

#define PI 3.141592653589793


class SA_AW_Vinyl
{
public:


  float MidHP;
  float SideHP;
  float HighLimit;
  float GrooveWear;

  /*case kParamA: vst_strncpy(text, "Mid HiP", kVstMaxParamStrLen); break;
  case kParamB: vst_strncpy(text, "SideHiP", kVstMaxParamStrLen); break;
  case kParamC: vst_strncpy(text, "H Limit", kVstMaxParamStrLen); break;
  case kParamD: vst_strncpy(text, "Gv Wear", kVstMaxParamStrLen); break;*/

  SA_AW_Vinyl()
  {
    for (int count = 0; count < 11; count++) {
      aMid[count] = 0.0;
      bMid[count] = 0.0;
      fMid[count] = 0.0;
      aSide[count] = 0.0;
      bSide[count] = 0.0;
      fSide[count] = 0.0;
    }
    aMidPrev = 0.0; aSidePrev = 0.0;
    bMidPrev = 0.0; bSidePrev = 0.0;
    ataLastOutL = ataLastOutR = 0.0;
    midSampleA = 0.0;
    midSampleB = 0.0;
    midSampleC = 0.0;
    midSampleD = 0.0;
    midSampleE = 0.0;
    midSampleF = 0.0;
    midSampleG = 0.0;
    midSampleH = 0.0;
    midSampleI = 0.0;
    midSampleJ = 0.0;
    midSampleK = 0.0;
    midSampleL = 0.0;
    midSampleM = 0.0;
    midSampleN = 0.0;
    midSampleO = 0.0;
    midSampleP = 0.0;
    midSampleQ = 0.0;
    midSampleR = 0.0;
    midSampleS = 0.0;
    midSampleT = 0.0;
    midSampleU = 0.0;
    midSampleV = 0.0;
    midSampleW = 0.0;
    midSampleX = 0.0;
    midSampleY = 0.0;
    midSampleZ = 0.0;

    sideSampleA = 0.0;
    sideSampleB = 0.0;
    sideSampleC = 0.0;
    sideSampleD = 0.0;
    sideSampleE = 0.0;
    sideSampleF = 0.0;
    sideSampleG = 0.0;
    sideSampleH = 0.0;
    sideSampleI = 0.0;
    sideSampleJ = 0.0;
    sideSampleK = 0.0;
    sideSampleL = 0.0;
    sideSampleM = 0.0;
    sideSampleN = 0.0;
    sideSampleO = 0.0;
    sideSampleP = 0.0;
    sideSampleQ = 0.0;
    sideSampleR = 0.0;
    sideSampleS = 0.0;
    sideSampleT = 0.0;
    sideSampleU = 0.0;
    sideSampleV = 0.0;
    sideSampleW = 0.0;
    sideSampleX = 0.0;
    sideSampleY = 0.0;
    sideSampleZ = 0.0;
    s1L = s2L = s3L = 0.0;
    o1L = o2L = o3L = 0.0;
    m1L = m2L = desL = 0.0;
    s1R = s2R = s3R = 0.0;
    o1R = o2R = o3R = 0.0;
    m1R = m2R = desR = 0.0;

    MidHP = 0.203419;  //22.0 hz = ((A*A)*290)+10  (A*A)*290 = 12   (A*A) = 0.0413793  sqrt() = 0.203419
    SideHP = 0.3424051; //44.0 hz = ((B*B)*290)+10  (B*B)*290 = 34   (B*B) = 0.1172413  sqrt() = 0.3424051
    HighLimit = 0.32;
    GrooveWear = 0.064;
    fpNShapeL = 0.0;
    fpNShapeR = 0.0;
  }

  ~SA_AW_Vinyl()
  {
  }
  void setSampleRate(float SR) {
    sampleRate = SR;
  }
  void processReplacing(iplug::sample** buffer, int channels, int nFrames)
  {
    auto* in1 = buffer[0];
    auto* in2 = buffer[1];
    
    double overallscale = 1.0;
    overallscale /= 44100.0;
    overallscale *= sampleRate;

    double fusswithscale = 50000; //corrected
    double cutofffreq = ((MidHP * MidHP) * 290.0) + 10.0;
    double resonance = 0.992;

    double midAmount = cutofffreq / fusswithscale;
    midAmount /= overallscale;

    double midAmountA = midAmount * resonance;
    double midAmountB = midAmountA * resonance;
    double midAmountC = midAmountB * resonance;
    double midAmountD = midAmountC * resonance;
    double midAmountE = midAmountD * resonance;
    double midAmountF = midAmountE * resonance;
    double midAmountG = midAmountF * resonance;
    double midAmountH = midAmountG * resonance;
    double midAmountI = midAmountH * resonance;
    double midAmountJ = midAmountI * resonance;
    double midAmountK = midAmountJ * resonance;
    double midAmountL = midAmountK * resonance;
    double midAmountM = midAmountL * resonance;
    double midAmountN = midAmountM * resonance;
    double midAmountO = midAmountN * resonance;
    double midAmountP = midAmountO * resonance;
    double midAmountQ = midAmountP * resonance;
    double midAmountR = midAmountQ * resonance;
    double midAmountS = midAmountR * resonance;
    double midAmountT = midAmountS * resonance;
    double midAmountU = midAmountT * resonance;
    double midAmountV = midAmountU * resonance;
    double midAmountW = midAmountV * resonance;
    double midAmountX = midAmountW * resonance;
    double midAmountY = midAmountX * resonance;
    double midAmountZ = midAmountY * resonance;


    double midaltAmountA = 1.0 - midAmountA;
    double midaltAmountB = 1.0 - midAmountB;
    double midaltAmountC = 1.0 - midAmountC;
    double midaltAmountD = 1.0 - midAmountD;
    double midaltAmountE = 1.0 - midAmountE;
    double midaltAmountF = 1.0 - midAmountF;
    double midaltAmountG = 1.0 - midAmountG;
    double midaltAmountH = 1.0 - midAmountH;
    double midaltAmountI = 1.0 - midAmountI;
    double midaltAmountJ = 1.0 - midAmountJ;
    double midaltAmountK = 1.0 - midAmountK;
    double midaltAmountL = 1.0 - midAmountL;
    double midaltAmountM = 1.0 - midAmountM;
    double midaltAmountN = 1.0 - midAmountN;
    double midaltAmountO = 1.0 - midAmountO;
    double midaltAmountP = 1.0 - midAmountP;
    double midaltAmountQ = 1.0 - midAmountQ;
    double midaltAmountR = 1.0 - midAmountR;
    double midaltAmountS = 1.0 - midAmountS;
    double midaltAmountT = 1.0 - midAmountT;
    double midaltAmountU = 1.0 - midAmountU;
    double midaltAmountV = 1.0 - midAmountV;
    double midaltAmountW = 1.0 - midAmountW;
    double midaltAmountX = 1.0 - midAmountX;
    double midaltAmountY = 1.0 - midAmountY;
    double midaltAmountZ = 1.0 - midAmountZ;

    cutofffreq = ((SideHP * SideHP) * 290.0) + 10.0;
    double sideAmount = cutofffreq / fusswithscale;
    sideAmount /= overallscale;
    double sideAmountA = sideAmount * resonance;
    double sideAmountB = sideAmountA * resonance;
    double sideAmountC = sideAmountB * resonance;
    double sideAmountD = sideAmountC * resonance;
    double sideAmountE = sideAmountD * resonance;
    double sideAmountF = sideAmountE * resonance;
    double sideAmountG = sideAmountF * resonance;
    double sideAmountH = sideAmountG * resonance;
    double sideAmountI = sideAmountH * resonance;
    double sideAmountJ = sideAmountI * resonance;
    double sideAmountK = sideAmountJ * resonance;
    double sideAmountL = sideAmountK * resonance;
    double sideAmountM = sideAmountL * resonance;
    double sideAmountN = sideAmountM * resonance;
    double sideAmountO = sideAmountN * resonance;
    double sideAmountP = sideAmountO * resonance;
    double sideAmountQ = sideAmountP * resonance;
    double sideAmountR = sideAmountQ * resonance;
    double sideAmountS = sideAmountR * resonance;
    double sideAmountT = sideAmountS * resonance;
    double sideAmountU = sideAmountT * resonance;
    double sideAmountV = sideAmountU * resonance;
    double sideAmountW = sideAmountV * resonance;
    double sideAmountX = sideAmountW * resonance;
    double sideAmountY = sideAmountX * resonance;
    double sideAmountZ = sideAmountY * resonance;


    double sidealtAmountA = 1.0 - sideAmountA;
    double sidealtAmountB = 1.0 - sideAmountB;
    double sidealtAmountC = 1.0 - sideAmountC;
    double sidealtAmountD = 1.0 - sideAmountD;
    double sidealtAmountE = 1.0 - sideAmountE;
    double sidealtAmountF = 1.0 - sideAmountF;
    double sidealtAmountG = 1.0 - sideAmountG;
    double sidealtAmountH = 1.0 - sideAmountH;
    double sidealtAmountI = 1.0 - sideAmountI;
    double sidealtAmountJ = 1.0 - sideAmountJ;
    double sidealtAmountK = 1.0 - sideAmountK;
    double sidealtAmountL = 1.0 - sideAmountL;
    double sidealtAmountM = 1.0 - sideAmountM;
    double sidealtAmountN = 1.0 - sideAmountN;
    double sidealtAmountO = 1.0 - sideAmountO;
    double sidealtAmountP = 1.0 - sideAmountP;
    double sidealtAmountQ = 1.0 - sideAmountQ;
    double sidealtAmountR = 1.0 - sideAmountR;
    double sidealtAmountS = 1.0 - sideAmountS;
    double sidealtAmountT = 1.0 - sideAmountT;
    double sidealtAmountU = 1.0 - sideAmountU;
    double sidealtAmountV = 1.0 - sideAmountV;
    double sidealtAmountW = 1.0 - sideAmountW;
    double sidealtAmountX = 1.0 - sideAmountX;
    double sidealtAmountY = 1.0 - sideAmountY;
    double sidealtAmountZ = 1.0 - sideAmountZ;
    double tempMid;
    double tempSide;

    double intensity = pow(HighLimit, 3) * (32 / overallscale);
    double inputSampleL;
    double inputSampleR;
    double senseL;
    double senseR;
    double smoothL;
    double smoothR;
    double mid;
    double side;

    overallscale = (GrooveWear * 9.0) + 1.0;
    double gain = overallscale;
    //mid groove wear
    if (gain > 1.0) { fMid[0] = 1.0; gain -= 1.0; }
    else { fMid[0] = gain; gain = 0.0; }
    if (gain > 1.0) { fMid[1] = 1.0; gain -= 1.0; }
    else { fMid[1] = gain; gain = 0.0; }
    if (gain > 1.0) { fMid[2] = 1.0; gain -= 1.0; }
    else { fMid[2] = gain; gain = 0.0; }
    if (gain > 1.0) { fMid[3] = 1.0; gain -= 1.0; }
    else { fMid[3] = gain; gain = 0.0; }
    if (gain > 1.0) { fMid[4] = 1.0; gain -= 1.0; }
    else { fMid[4] = gain; gain = 0.0; }
    if (gain > 1.0) { fMid[5] = 1.0; gain -= 1.0; }
    else { fMid[5] = gain; gain = 0.0; }
    if (gain > 1.0) { fMid[6] = 1.0; gain -= 1.0; }
    else { fMid[6] = gain; gain = 0.0; }
    if (gain > 1.0) { fMid[7] = 1.0; gain -= 1.0; }
    else { fMid[7] = gain; gain = 0.0; }
    if (gain > 1.0) { fMid[8] = 1.0; gain -= 1.0; }
    else { fMid[8] = gain; gain = 0.0; }
    if (gain > 1.0) { fMid[9] = 1.0; gain -= 1.0; }
    else { fMid[9] = gain; gain = 0.0; }
    //there, now we have a neat little moving average with remainders

    if (overallscale < 1.0) overallscale = 1.0;
    fMid[0] /= overallscale;
    fMid[1] /= overallscale;
    fMid[2] /= overallscale;
    fMid[3] /= overallscale;
    fMid[4] /= overallscale;
    fMid[5] /= overallscale;
    fMid[6] /= overallscale;
    fMid[7] /= overallscale;
    fMid[8] /= overallscale;
    fMid[9] /= overallscale;
    //and now it's neatly scaled, too

    overallscale = (GrooveWear * 4.5) + 1.0;
    gain = overallscale;
    //side groove wear
    if (gain > 1.0) { fSide[0] = 1.0; gain -= 1.0; }
    else { fSide[0] = gain; gain = 0.0; }
    if (gain > 1.0) { fSide[1] = 1.0; gain -= 1.0; }
    else { fSide[1] = gain; gain = 0.0; }
    if (gain > 1.0) { fSide[2] = 1.0; gain -= 1.0; }
    else { fSide[2] = gain; gain = 0.0; }
    if (gain > 1.0) { fSide[3] = 1.0; gain -= 1.0; }
    else { fSide[3] = gain; gain = 0.0; }
    if (gain > 1.0) { fSide[4] = 1.0; gain -= 1.0; }
    else { fSide[4] = gain; gain = 0.0; }
    if (gain > 1.0) { fSide[5] = 1.0; gain -= 1.0; }
    else { fSide[5] = gain; gain = 0.0; }
    if (gain > 1.0) { fSide[6] = 1.0; gain -= 1.0; }
    else { fSide[6] = gain; gain = 0.0; }
    if (gain > 1.0) { fSide[7] = 1.0; gain -= 1.0; }
    else { fSide[7] = gain; gain = 0.0; }
    if (gain > 1.0) { fSide[8] = 1.0; gain -= 1.0; }
    else { fSide[8] = gain; gain = 0.0; }
    if (gain > 1.0) { fSide[9] = 1.0; gain -= 1.0; }
    else { fSide[9] = gain; gain = 0.0; }
    //there, now we have a neat little moving average with remainders

    if (overallscale < 1.0) overallscale = 1.0;
    fSide[0] /= overallscale;
    fSide[1] /= overallscale;
    fSide[2] /= overallscale;
    fSide[3] /= overallscale;
    fSide[4] /= overallscale;
    fSide[5] /= overallscale;
    fSide[6] /= overallscale;
    fSide[7] /= overallscale;
    fSide[8] /= overallscale;
    fSide[9] /= overallscale;
    //and now it's neatly scaled, too

    double tempSample;
    double accumulatorSample;
    double midCorrection;
    double sideCorrection;
    double correction;

    int sampleFrames = nFrames;
    while (--sampleFrames >= 0)
    {
      inputSampleL = *in1;
      inputSampleR = *in2;
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

      s3L = s2L;
      s2L = s1L;
      s1L = inputSampleL;
      smoothL = (s3L + s2L + s1L) / 3.0;
      m1L = (s1L - s2L) * ((s1L - s2L) / 1.3);
      m2L = (s2L - s3L) * ((s1L - s2L) / 1.3);
      senseL = fabs(m1L - m2L);
      senseL = (intensity * intensity * senseL);
      o3L = o2L;
      o2L = o1L;
      o1L = senseL;
      if (o2L > senseL) senseL = o2L;
      if (o3L > senseL) senseL = o3L;
      //sense on the most intense

      s3R = s2R;
      s2R = s1R;
      s1R = inputSampleR;
      smoothR = (s3R + s2R + s1R) / 3.0;
      m1R = (s1R - s2R) * ((s1R - s2R) / 1.3);
      m2R = (s2R - s3R) * ((s1R - s2R) / 1.3);
      senseR = fabs(m1R - m2R);
      senseR = (intensity * intensity * senseR);
      o3R = o2R;
      o2R = o1R;
      o1R = senseR;
      if (o2R > senseR) senseR = o2R;
      if (o3R > senseR) senseR = o3R;
      //sense on the most intense


      if (senseL > 1.0) senseL = 1.0;
      if (senseR > 1.0) senseR = 1.0;

      inputSampleL *= (1.0 - senseL);
      inputSampleR *= (1.0 - senseR);

      inputSampleL += (smoothL * senseL);
      inputSampleR += (smoothR * senseR);
      //we need to do the de-ess before anything else, and feed the result into the antialiasing-
      //but the trigger runs off just the input samples

      tempMid = mid = inputSampleL + inputSampleR;
      tempSide = side = inputSampleL - inputSampleR;
      //assign mid and side.

      tempSample = mid;
      midSampleA = (midSampleA * midaltAmountA) + (tempSample * midAmountA); tempSample -= midSampleA;
      midSampleB = (midSampleB * midaltAmountB) + (tempSample * midAmountB); tempSample -= midSampleB;
      midSampleC = (midSampleC * midaltAmountC) + (tempSample * midAmountC); tempSample -= midSampleC;
      midSampleD = (midSampleD * midaltAmountD) + (tempSample * midAmountD); tempSample -= midSampleD;
      midSampleE = (midSampleE * midaltAmountE) + (tempSample * midAmountE); tempSample -= midSampleE;
      midSampleF = (midSampleF * midaltAmountF) + (tempSample * midAmountF); tempSample -= midSampleF;
      midSampleG = (midSampleG * midaltAmountG) + (tempSample * midAmountG); tempSample -= midSampleG;
      midSampleH = (midSampleH * midaltAmountH) + (tempSample * midAmountH); tempSample -= midSampleH;
      midSampleI = (midSampleI * midaltAmountI) + (tempSample * midAmountI); tempSample -= midSampleI;
      midSampleJ = (midSampleJ * midaltAmountJ) + (tempSample * midAmountJ); tempSample -= midSampleJ;
      midSampleK = (midSampleK * midaltAmountK) + (tempSample * midAmountK); tempSample -= midSampleK;
      midSampleL = (midSampleL * midaltAmountL) + (tempSample * midAmountL); tempSample -= midSampleL;
      midSampleM = (midSampleM * midaltAmountM) + (tempSample * midAmountM); tempSample -= midSampleM;
      midSampleN = (midSampleN * midaltAmountN) + (tempSample * midAmountN); tempSample -= midSampleN;
      midSampleO = (midSampleO * midaltAmountO) + (tempSample * midAmountO); tempSample -= midSampleO;
      midSampleP = (midSampleP * midaltAmountP) + (tempSample * midAmountP); tempSample -= midSampleP;
      midSampleQ = (midSampleQ * midaltAmountQ) + (tempSample * midAmountQ); tempSample -= midSampleQ;
      midSampleR = (midSampleR * midaltAmountR) + (tempSample * midAmountR); tempSample -= midSampleR;
      midSampleS = (midSampleS * midaltAmountS) + (tempSample * midAmountS); tempSample -= midSampleS;
      midSampleT = (midSampleT * midaltAmountT) + (tempSample * midAmountT); tempSample -= midSampleT;
      midSampleU = (midSampleU * midaltAmountU) + (tempSample * midAmountU); tempSample -= midSampleU;
      midSampleV = (midSampleV * midaltAmountV) + (tempSample * midAmountV); tempSample -= midSampleV;
      midSampleW = (midSampleW * midaltAmountW) + (tempSample * midAmountW); tempSample -= midSampleW;
      midSampleX = (midSampleX * midaltAmountX) + (tempSample * midAmountX); tempSample -= midSampleX;
      midSampleY = (midSampleY * midaltAmountY) + (tempSample * midAmountY); tempSample -= midSampleY;
      midSampleZ = (midSampleZ * midaltAmountZ) + (tempSample * midAmountZ); tempSample -= midSampleZ;
      correction = midCorrection = mid - tempSample;
      mid -= correction;

      tempSample = side;
      sideSampleA = (sideSampleA * sidealtAmountA) + (tempSample * sideAmountA); tempSample -= sideSampleA;
      sideSampleB = (sideSampleB * sidealtAmountB) + (tempSample * sideAmountB); tempSample -= sideSampleB;
      sideSampleC = (sideSampleC * sidealtAmountC) + (tempSample * sideAmountC); tempSample -= sideSampleC;
      sideSampleD = (sideSampleD * sidealtAmountD) + (tempSample * sideAmountD); tempSample -= sideSampleD;
      sideSampleE = (sideSampleE * sidealtAmountE) + (tempSample * sideAmountE); tempSample -= sideSampleE;
      sideSampleF = (sideSampleF * sidealtAmountF) + (tempSample * sideAmountF); tempSample -= sideSampleF;
      sideSampleG = (sideSampleG * sidealtAmountG) + (tempSample * sideAmountG); tempSample -= sideSampleG;
      sideSampleH = (sideSampleH * sidealtAmountH) + (tempSample * sideAmountH); tempSample -= sideSampleH;
      sideSampleI = (sideSampleI * sidealtAmountI) + (tempSample * sideAmountI); tempSample -= sideSampleI;
      sideSampleJ = (sideSampleJ * sidealtAmountJ) + (tempSample * sideAmountJ); tempSample -= sideSampleJ;
      sideSampleK = (sideSampleK * sidealtAmountK) + (tempSample * sideAmountK); tempSample -= sideSampleK;
      sideSampleL = (sideSampleL * sidealtAmountL) + (tempSample * sideAmountL); tempSample -= sideSampleL;
      sideSampleM = (sideSampleM * sidealtAmountM) + (tempSample * sideAmountM); tempSample -= sideSampleM;
      sideSampleN = (sideSampleN * sidealtAmountN) + (tempSample * sideAmountN); tempSample -= sideSampleN;
      sideSampleO = (sideSampleO * sidealtAmountO) + (tempSample * sideAmountO); tempSample -= sideSampleO;
      sideSampleP = (sideSampleP * sidealtAmountP) + (tempSample * sideAmountP); tempSample -= sideSampleP;
      sideSampleQ = (sideSampleQ * sidealtAmountQ) + (tempSample * sideAmountQ); tempSample -= sideSampleQ;
      sideSampleR = (sideSampleR * sidealtAmountR) + (tempSample * sideAmountR); tempSample -= sideSampleR;
      sideSampleS = (sideSampleS * sidealtAmountS) + (tempSample * sideAmountS); tempSample -= sideSampleS;
      sideSampleT = (sideSampleT * sidealtAmountT) + (tempSample * sideAmountT); tempSample -= sideSampleT;
      sideSampleU = (sideSampleU * sidealtAmountU) + (tempSample * sideAmountU); tempSample -= sideSampleU;
      sideSampleV = (sideSampleV * sidealtAmountV) + (tempSample * sideAmountV); tempSample -= sideSampleV;
      sideSampleW = (sideSampleW * sidealtAmountW) + (tempSample * sideAmountW); tempSample -= sideSampleW;
      sideSampleX = (sideSampleX * sidealtAmountX) + (tempSample * sideAmountX); tempSample -= sideSampleX;
      sideSampleY = (sideSampleY * sidealtAmountY) + (tempSample * sideAmountY); tempSample -= sideSampleY;
      sideSampleZ = (sideSampleZ * sidealtAmountZ) + (tempSample * sideAmountZ); tempSample -= sideSampleZ;
      correction = sideCorrection = side - tempSample;
      side -= correction;


      aMid[9] = aMid[8]; aMid[8] = aMid[7]; aMid[7] = aMid[6]; aMid[6] = aMid[5];
      aMid[5] = aMid[4]; aMid[4] = aMid[3]; aMid[3] = aMid[2]; aMid[2] = aMid[1];
      aMid[1] = aMid[0]; aMid[0] = accumulatorSample = (mid - aMidPrev);

      accumulatorSample *= fMid[0];
      accumulatorSample += (aMid[1] * fMid[1]);
      accumulatorSample += (aMid[2] * fMid[2]);
      accumulatorSample += (aMid[3] * fMid[3]);
      accumulatorSample += (aMid[4] * fMid[4]);
      accumulatorSample += (aMid[5] * fMid[5]);
      accumulatorSample += (aMid[6] * fMid[6]);
      accumulatorSample += (aMid[7] * fMid[7]);
      accumulatorSample += (aMid[8] * fMid[8]);
      accumulatorSample += (aMid[9] * fMid[9]);
      //we are doing our repetitive calculations on a separate value
      correction = (mid - aMidPrev) - accumulatorSample;
      midCorrection += correction;
      aMidPrev = mid;
      mid -= correction;

      aSide[9] = aSide[8]; aSide[8] = aSide[7]; aSide[7] = aSide[6]; aSide[6] = aSide[5];
      aSide[5] = aSide[4]; aSide[4] = aSide[3]; aSide[3] = aSide[2]; aSide[2] = aSide[1];
      aSide[1] = aSide[0]; aSide[0] = accumulatorSample = (side - aSidePrev);

      accumulatorSample *= fSide[0];
      accumulatorSample += (aSide[1] * fSide[1]);
      accumulatorSample += (aSide[2] * fSide[2]);
      accumulatorSample += (aSide[3] * fSide[3]);
      accumulatorSample += (aSide[4] * fSide[4]);
      accumulatorSample += (aSide[5] * fSide[5]);
      accumulatorSample += (aSide[6] * fSide[6]);
      accumulatorSample += (aSide[7] * fSide[7]);
      accumulatorSample += (aSide[8] * fSide[8]);
      accumulatorSample += (aSide[9] * fSide[9]);
      //we are doing our repetitive calculations on a separate value		
      correction = (side - aSidePrev) - accumulatorSample;
      sideCorrection += correction;
      aSidePrev = side;
      side -= correction;


      bMid[9] = bMid[8]; bMid[8] = bMid[7]; bMid[7] = bMid[6]; bMid[6] = bMid[5];
      bMid[5] = bMid[4]; bMid[4] = bMid[3]; bMid[3] = bMid[2]; bMid[2] = bMid[1];
      bMid[1] = bMid[0]; bMid[0] = accumulatorSample = (mid - bMidPrev);

      accumulatorSample *= fMid[0];
      accumulatorSample += (bMid[1] * fMid[1]);
      accumulatorSample += (bMid[2] * fMid[2]);
      accumulatorSample += (bMid[3] * fMid[3]);
      accumulatorSample += (bMid[4] * fMid[4]);
      accumulatorSample += (bMid[5] * fMid[5]);
      accumulatorSample += (bMid[6] * fMid[6]);
      accumulatorSample += (bMid[7] * fMid[7]);
      accumulatorSample += (bMid[8] * fMid[8]);
      accumulatorSample += (bMid[9] * fMid[9]);
      //we are doing our repetitive calculations on a separate value
      correction = (mid - bMidPrev) - accumulatorSample;
      midCorrection += correction;
      bMidPrev = mid;

      bSide[9] = bSide[8]; bSide[8] = bSide[7]; bSide[7] = bSide[6]; bSide[6] = bSide[5];
      bSide[5] = bSide[4]; bSide[4] = bSide[3]; bSide[3] = bSide[2]; bSide[2] = bSide[1];
      bSide[1] = bSide[0]; bSide[0] = accumulatorSample = (side - bSidePrev);

      accumulatorSample *= fSide[0];
      accumulatorSample += (bSide[1] * fSide[1]);
      accumulatorSample += (bSide[2] * fSide[2]);
      accumulatorSample += (bSide[3] * fSide[3]);
      accumulatorSample += (bSide[4] * fSide[4]);
      accumulatorSample += (bSide[5] * fSide[5]);
      accumulatorSample += (bSide[6] * fSide[6]);
      accumulatorSample += (bSide[7] * fSide[7]);
      accumulatorSample += (bSide[8] * fSide[8]);
      accumulatorSample += (bSide[9] * fSide[9]);
      //we are doing our repetitive calculations on a separate value		
      correction = (side - bSidePrev) - accumulatorSample;
      sideCorrection += correction;
      bSidePrev = side;


      mid = tempMid - midCorrection;
      side = tempSide - sideCorrection;
      inputSampleL = (mid + side) / 2.0;
      inputSampleR = (mid - side) / 2.0;

      senseL /= 2.0;
      senseR /= 2.0;


      accumulatorSample = (ataLastOutL * senseL) + (inputSampleL * (1.0 - senseL));
      ataLastOutL = inputSampleL;
      inputSampleL = accumulatorSample;

      accumulatorSample = (ataLastOutR * senseR) + (inputSampleR * (1.0 - senseR));
      ataLastOutR = inputSampleR;
      inputSampleR = accumulatorSample;
      //we just re-use accumulatorSample to do this little shuffle

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
private:
  double ataLastOutL;
  double ataLastOutR;

  double s1L;
  double s2L;
  double s3L;
  double o1L;
  double o2L;
  double o3L;
  double m1L;
  double m2L;
  double s1R;
  double s2R;
  double s3R;
  double o1R;
  double o2R;
  double o3R;
  double m1R;
  double m2R;
  double desL;
  double desR;

  double midSampleA;
  double midSampleB;
  double midSampleC;
  double midSampleD;
  double midSampleE;
  double midSampleF;
  double midSampleG;
  double midSampleH;
  double midSampleI;
  double midSampleJ;
  double midSampleK;
  double midSampleL;
  double midSampleM;
  double midSampleN;
  double midSampleO;
  double midSampleP;
  double midSampleQ;
  double midSampleR;
  double midSampleS;
  double midSampleT;
  double midSampleU;
  double midSampleV;
  double midSampleW;
  double midSampleX;
  double midSampleY;
  double midSampleZ;

  double sideSampleA;
  double sideSampleB;
  double sideSampleC;
  double sideSampleD;
  double sideSampleE;
  double sideSampleF;
  double sideSampleG;
  double sideSampleH;
  double sideSampleI;
  double sideSampleJ;
  double sideSampleK;
  double sideSampleL;
  double sideSampleM;
  double sideSampleN;
  double sideSampleO;
  double sideSampleP;
  double sideSampleQ;
  double sideSampleR;
  double sideSampleS;
  double sideSampleT;
  double sideSampleU;
  double sideSampleV;
  double sideSampleW;
  double sideSampleX;
  double sideSampleY;
  double sideSampleZ;

  double aMid[11];
  double bMid[11];
  double fMid[11];
  double aSide[11];
  double bSide[11];
  double fSide[11];
  double aMidPrev;
  double aSidePrev;
  double bMidPrev;
  double bSidePrev;

  long double fpNShapeL;
  long double fpNShapeR;
  //default stuff

  float sampleRate = 44100.0f;
};
