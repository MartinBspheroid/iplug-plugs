#pragma once
#include "MP_Colors.h"
#include "IControl.h"
//#include "SA_Math.h"

namespace MP {
  class Section : public IControl, IVectorBase
  {
  public:
    Section(const IRECT& bounds, const char* title = "") : IControl(bounds), IVectorBase(DEFAULT_STYLE)
    {
      labelRect = bounds;
      mStyle.labelText = mStyle.labelText.WithFont("Brixton-Bold");
      mStyle.labelText.mAlign = EAlign::Near;
      mStyle.labelText.mSize = labelRect.H() * 0.8;
      mStyle.labelText.mVAlign = EVAlign::Bottom;
      mStyle.labelText.mFGColor = MP::pallete.LIGHT_GRAY;
      strcpy(mTitle, title);
      
    }

    ~Section()
    {
    }
    void Draw(IGraphics& g) override;

  private:
    void OnMouseDown(float x, float y, const IMouseMod& mod) override;
    void OnMouseUp(float x, float y, const IMouseMod& mod) override;
    void OnMouseOver(float x, float y, const IMouseMod& mod) override;
    void OnMouseOut() override;
    char mTitle[64]; 
    IRECT labelRect;
  };

  void Section::Draw(IGraphics& g)
  {
    g.FillRect(MP::pallete.BG_LABEL_DARK, labelRect);
    g.DrawText(mStyle.labelText, mTitle, labelRect.GetOffset(20, 0, -20, 0), 0);
    //g.DrawRect(COLOR_WHITE, labelRect.GetAltered(20, 0, -20, 0), 0, 0.5);
    
    //g.DrawRect(COLOR_WHITE, mRECT);
  }

  void Section::OnMouseDown(float x, float y, const IMouseMod& mod)
  {

  }


  void Section::OnMouseUp(float x, float y, const IMouseMod& mod)
  {

  }


  void Section::OnMouseOver(float x, float y, const IMouseMod& mod)
  {

  }


  void Section::OnMouseOut()
  {

  }

}