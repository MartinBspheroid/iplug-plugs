#pragma once
#include "IPlugConstants.h"
namespace SA{


	template <typename Tn>
void to_mono (int samples, Tn* dest, Tn const* left, Tn const* right)
{

  while (samples-- > 0)
    *dest++ = (*left++ + *right++) * Tn(0.70710678118654752440084436210485);


}


//=======================================================

// Tracks the peaks in the signal stream using the attack and release parameters
template <int Channels=2, typename Value=float>
class EnvelopeFollower
{
public:
  EnvelopeFollower()
  {
    for (int i = 0; i < Channels; i++)
      m_env[i]=0;
  }

  Value operator[] (int channel) const
  {
    return m_env[channel];
  }

  void Setup (int SR, const double &attackMs, const double &releaseMs)
  {
    sampleRate = SR;
    m_a = pow (0.01, 1.0 / (attackMs * sampleRate * 0.001));
    m_r = pow (0.01, 1.0 / (releaseMs * sampleRate * 0.001));
  }
  void SetAttack(const double &attackMs)
  {
    m_a = pow(0.01, 1.0 / (attackMs * sampleRate * 0.001)); 
  }
  void SetRelease(const double &releaseMs)
  {
    m_r = pow(0.01, 1.0 / (releaseMs* sampleRate * 0.001));
  }
  void Process(const float& left, const float &right) {
    //left
    double e = m_env[0];
    double v = std::abs(left);
    if (v > e)
      e = m_a * (e - v) + v;
    else
      e = m_r * (e - v) + v;
    m_env[0] = e;

    // right
     e = m_env[1];
     v = std::abs(right);
    if (v > e)
      e = m_a * (e - v) + v;
    else
      e = m_r * (e - v) + v;
    m_env[1] = e;
  }
  void Process (const iplug::sample** inputs, size_t nFrames)
  {

    for (size_t n = 0; n < nFrames; n++)
    {
      double e = m_env[i];

      for (size_t c = 0; c < Channels; c++)
      {
        double v = std::abs(inputs[c][n]);
        if (v > e)
          e = m_a * (e - v) + v;
        else
          e = m_r * (e - v) + v;
        
      }
      m_env[i] = e;
    }
    /*for( int i=0; i<Channels; i++ )
    {
      const Value* cur = src[i];

      double e = m_env[i];
      for (int n = nFrames; n; n--)
      {
        double v = std::abs(*cur++);
        if (v > e)
          e = m_a * (e - v) + v;
        else
          e = m_r * (e - v) + v;
      }
      m_env[i]=e;
    }*/
  }
  double getEnvelopeMono() {
    return ((m_env[0] + m_env[1]) / 2)*gain;
  }
  double m_env[Channels];

  void SetGain(double val) { gain = val; }
protected:
  double m_a = 0;
  double m_r = 0;
  double gain = 1.0;
  int sampleRate = 0;
};


//=================================================================

// Helpful for discovering discontinuities in buffers
template <int Channels=2, typename Value=float>
class SlopeDetector
{
public:
  SlopeDetector () : m_firstTime(true)
  {
	for (int i = 0; i < Channels; ++i)
	  m_slope [i] = 0;
  }

  Value getSlope (int channel) const
  {
	return m_slope [channel];
  }

  void process (size_t numSamples, const Value** input)
  {
	for (int i = 0; i < Channels; ++i)
	{
	  const Value* src = input [i];
	  int n = numSamples;

	  if (m_firstTime)
	  {
		m_prev[i] = *src++;
		--n;
	  }

	  while (n > 0)
	  {
		n--;
		Value cur = *src++;
		Value diff = std::abs (cur - m_prev[i]);
		m_slope [i] = std::max (diff, m_slope[i]);
		m_prev [i] = cur;
	  }
	}

	m_firstTime = false;
  }

private:
  bool m_firstTime;
  Value m_slope [Channels];
  Value m_prev [Channels];
};
}