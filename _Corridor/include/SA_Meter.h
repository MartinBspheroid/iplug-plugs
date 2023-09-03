#pragma once

#include "IControl.h"
#include "ISender.h"





namespace SA {
  using namespace iplug;
  using namespace iplug::igraphics;
  class Meter : public IControl,
    public IVectorBase
  {
  public:



    Meter::Meter(const IRECT bounds, const char* label = "", int idx = kNoTag, IActionFunction  aF = [](IControl* c) {})
      : IControl(bounds, idx)
      , IVectorBase(DEFAULT_STYLE, false)

    {



    }
    



    virtual ~Meter() {}


    void OnMouseOut() override {  }


    void Draw(IGraphics& g) override
    {
      g.DrawRect(COLOR_WHITE, mRECT);
      g.FillRect(COLOR_YELLOW, mRECT.GetFromBottom(pos * mRECT.H()));

    }

    void OnMouseDown(float x, float y, const IMouseMod& mod) override
    {
    }

    void OnMouseUp(float x, float y, const IMouseMod& mod) override
    {
      
    }

    void OnMouseOver(float x, float y, const IMouseMod& mod) override
    {
    }

    void OnResize() override
    {
    }
    void OnMsgFromDelegate(int msgTag, int dataSize, const void* pData) override
    {
      if (!IsDisabled() && msgTag == ISender<>::kUpdateMessage)
      {
        IByteStream stream(pData, dataSize);

        int positions = 0;
        positions = stream.Get(&mData, positions);
        pos = mData.vals[0];
        SetDirty(true);
      }
    }

    bool loaded = false;
  protected:
    std::vector<float> waveForm_;
    ISenderData<> mData;
    float pos = 0;

  };
}