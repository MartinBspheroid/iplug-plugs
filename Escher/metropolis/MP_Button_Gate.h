#pragma once
#include "IPlugConstants.h"
#include "MP_Colors.h"
#include "IControl.h"
//#include "SA_Math.h"

namespace MP {
  using namespace iplug;
  using namespace iplug::igraphics;
  class ButtonGate : public IControl, IVectorBase
  {
  public:
    ButtonGate(const IRECT& bounds, const char* title, std::function<void()> OnDown, std::function<void()> OnUp) : IControl(bounds), IVectorBase(DEFAULT_STYLE)
    {
      labelRect = bounds;
      mStyle.labelText = mStyle.labelText.WithFont("Brixton-Bold");
      mStyle.labelText.mAlign = EAlign::Center;
      mStyle.labelText.mSize = labelRect.W() * 0.2;
      mStyle.labelText.mVAlign = EVAlign::Middle;
      mStyle.labelText.mFGColor = MP::pallete.LIGHT_GRAY;
      strcpy(mTitle, title);
      currentColor = MP::pallete.BG_LABEL_DARK;

      onUpAction = OnUp;
      onDownAction = OnDown;

    }

    ~ButtonGate()
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
    std::function<void()> onDownAction, onUpAction;
    iplug::igraphics::IColor currentColor;
    bool state = false;
  };

  void ButtonGate::Draw(IGraphics& g)
  {

    g.FillRect(currentColor, labelRect);
    g.DrawText(mStyle.labelText.WithFGColor(state ? MP::pallete.BG_DARK : MP::pallete.LIGHT_GRAY), mTitle, labelRect.GetOffset(20, 0, -20, 0), 0);
    g.DrawRect(MP::pallete.LIGHT_GRAY, labelRect, 0, 1);
    
    //g.DrawRect(COLOR_WHITE, mRECT);
  }

  void ButtonGate::OnMouseDown(float x, float y, const IMouseMod& mod)
  {
    state = true;
    onDownAction();
    currentColor = MP::pallete.LIGHT_GRAY;
    SetDirty(true);
  }


  void ButtonGate::OnMouseUp(float x, float y, const IMouseMod& mod)
  {
    state = false;
    onUpAction();
    currentColor = MP::pallete.BG_LABEL_DARK;
    SetDirty(true);

  }


  void ButtonGate::OnMouseOver(float x, float y, const IMouseMod& mod)
  {

  }


  void ButtonGate::OnMouseOut()
  {

  }

}