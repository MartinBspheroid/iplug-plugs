#pragma once
#include <functional>
#include <vector>
#include "IPlugPlatform.h"
#include "IPlugConstants.h"
#include <memory>
#include <cassert>


namespace SA {


  class EffectQueue
  {
  public:
    EffectQueue()
    {
    }

    ~EffectQueue()
    {
    }
    void pushEffect(std::function<void(iplug::sample** buffer, int channels, int nFrames)> func) {
      queue.push_back(func);
    }
    void runQueue(iplug::sample** outputs, int channels, const int nFrames) {
      
      if (order.size() == 0) { return; }
      assert(order.size() == queue.size());
      for (size_t i = 0; i < queue.size() || order.size() == 0; i++)
      {
        
        queue.at(order.at(i))(outputs, channels, nFrames);
      }
    }
    
    std::vector<int> getOrder() const { return order; }
    void setOrder(std::vector<int> val) { order = val; }
  private:
    std::vector<int> order;
    std::vector<std::function<void(iplug::sample** buffer, int channels, int nFrames)>> queue;
  };

}
        // end SA namespace

