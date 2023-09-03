#pragma once
#include <vector>
#include <functional>
#include "IPlugMidi.h"

#include <stdexcept>

class  EuqSeq {
public:

  struct Step {
    /// <summary>
    /// does this step do custom parameter lock??? 
    /// </summary>
    bool customParam = false;

    /// <summary>
    /// number of repetitions to trigger custom paramater
    /// </summary>
    int every = 1;

    /// <summary>
    /// number of bars to restart the counting
    /// </summary>
    int bars = 2; 

    /// <summary>
    /// Skip this step (true) or play only on this :every: bar (false) / default = false;
    /// </summary>
    bool skip = false;
    /// <summary>
    /// velocity of this step
    /// </summary>
    float vel = 0;
  };
  EuqSeq() {};

  std::function<void(iplug::IMidiMsg)> sendMidi;
  int length = 16;
  int beats = 4;
  std::vector<Step> steps;
  void init() {
    for (size_t i = 0; i < 32; i++)
    {
      auto s = Step();
      steps.push_back(s);
    }
    recalc();
  }
  int current_step = 0;
  int offset = 0;
  int accents = 2;
  float vel = 0.75;
  float acc_ratio = 0.5;
  int note = 36;
  bool newStep = false;
  bool stopped = true;
  bool muted = false;
  bool solo = false;
  float dot = 1;

  void recalc() {
    auto s = bjorklund(length, beats, vel);

    if (offset != 0) {
      if (offset > 0) {
        std::rotate(s.begin(), s.end() - (offset % length), s.end());
      }
      else {
        std::rotate(s.begin(), s.begin() + (abs(offset) % length), s.end());

      }
    }
    if (accents > 0 && accents < beats) {
      // do accents calc
      auto a = bjorklund(beats, accents, 1);
      int acc_iter = 0;
      for (size_t i = 0; i < length; i++)
      {
        if (s.at(i) > 0) {
          a.at(acc_iter) > 0 ? s.at(i) *= acc_ratio : 0;
          acc_iter++;
        }
      }

    }
    for (size_t i = 0; i < s.size(); i++)
    {
      steps.at(i).vel = s.at(i);
    }
  }

  void update(double& step, int frames, bool playing)
  {
    if (frames <= 0 || (!playing && stopped)) {
      return;
    }
    if (!playing && !stopped) {
      iplug::IMidiMsg noteOff;
      noteOff.MakeNoteOffMsg(note, 0);
      sendMidi(noteOff);
      stopped = true;
      return;
    }
    assert(steps.size() > 0);
    int step_now = (int)(step*division*dot);
    if ((step_now % length) != current_step) {
      newStep = true;
      if (steps.at(current_step).vel > 0) {
        iplug::IMidiMsg noteOff;
        noteOff.MakeNoteOffMsg(note, 0);
        sendMidi(noteOff);
      }
      current_step = step_now % length;
      if (steps.at(current_step).vel > 0) {
        iplug::IMidiMsg noteOn;
        int vel = (int)(steps.at(current_step).vel * 127);
        noteOn.MakeNoteOnMsg(note, vel, 0);
        sendMidi(noteOn);
      }
      
    }
    stopped = false;
  }
  std::vector<float> bjorklund(int step, int pulse, float val) {
    int pauses = step - pulse;
    int remainder = pauses % pulse;
    int per_pulse = (int)floor(pauses / pulse);
    int noskip = (remainder == 0) ? 0 : (int)floor(pulse / remainder);
    int skipXTime = (noskip == 0) ? 0 : (int)floor((pulse - remainder) / noskip);
    std::vector<float> rhythm;
    int count = 0;
    int skipper = 0;
    for (int i = 1; i <= step; i++) {
      if (count == 0) {
        rhythm.push_back(val);
        count = per_pulse;
        if (remainder > 0 && skipper == 0) {
          count++;
          remainder--;
          skipper = (skipXTime > 0) ? noskip : 0;
          skipXTime--;
        }
        else {
          skipper--;
        }
      }
      else {
        rhythm.push_back(0);
        count--;
      }
    }
    return rhythm;
  }
  enum Divisions {
    One_Sixteen = 8,
    One_Eitght = 4,
    One_Four = 2,
    One_Half = 1,
    One_Bar = 2,
    Two_Bars = 1,

  };
  int division = Divisions::One_Sixteen;
};
