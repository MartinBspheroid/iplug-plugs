#pragma once

#include "IControl.h"
#include "IColorPickerControl.h"
#include "IVKeyboardControl.h"
#include "IVMeterControl.h"
#include "IVScopeControl.h"
#include "IVMultiSliderControl.h"
#include "IRTTextControl.h"
#include "IVDisplayControl.h"
#include "ILEDControl.h"
#include "IPopupMenuControl.h"
#include "IControls.h"
#include <functional>
#include <iostream>
#include "Tikka_Seq.h"
#include <memory>
#include <algorithm>
#include "IGraphicsFlexBox.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE


struct Tikka_Layout {
  YGFlexDirection dir = YGFlexDirection::YGFlexDirectionRow;
  YGJustify justify = YGJustify::YGJustifyFlexStart;
  YGWrap wrap = YGWrap::YGWrapNoWrap;
  float pad = 0;
  float margin = 0;
  bool calc_FromText = false;
};

class Tikka_NumberKnobInt : public IControl, public IVectorBase {
public:
  Tikka_NumberKnobInt(IRECT& bounds, IColor _color, char* _name, int& _val, int _min, int _max, std::function<void()> _func = []() {}) : IControl(bounds, NULL), IVectorBase(DEFAULT_STYLE), val(_val)
  {
    name = _name;
    min = _min;
    max = _max;
    color = _color;
    mText = IText(bounds.GetScaled(0.55).H(), _color, nullptr, EAlign::Center, EVAlign::Middle);
    stepSize = 25;
    func = _func;
  };

  void Draw(IGraphics& p) override {
    p.DrawText(mText.WithSize(mRECT.H() * 0.35).WithFGColor(color.WithOpacity(hoverOpacity)), name, mRECT.GetFromBottom(mRECT.H() * 0.35));

    p.DrawText(mText.WithFGColor(color), std::to_string(val).c_str(), mRECT.GetFromTop(mRECT.H() * 0.65));
  };

  void OnMouseDown(float x, float y, const IMouseMod& mod) override {
    if (mRECT.Contains(x, y)) {
      mDown = true;

      offY = 0;

    }
  };
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override {
    if (mDown) {
      offY += dY;

      if (offY < -stepSize) {

        val = clamp(val + 1, min, max);
        offY = 0;
        func();

      }
      if (offY > stepSize) {
        val = clamp(val - 1, min, max);
        offY = 0;
        func();
      }

    }
  };
  void OnMouseUp(float x, float y, const IMouseMod& mod) override {
    if (mDown) {
      mDown = false;

    }

  };
  void OnMouseOver(float x, float y, const IMouseMod& mod)override {
    hoverOpacity <= 1.0f ? hoverOpacity += 0.05 : 0;

  };
  void OnMouseOut()override {
    hoverOpacity = 0.25;

  };
  int clamp(const int& n, const int& lower, const int& upper) {
    return std::max(lower, std::min(n, upper));
  }

private:
  int& val;
  bool mDown = false;
  IText mText;
  float offY;
  float stepSize;
  int min, max;
  char* name;
  IColor color;
  std::function<void()> func;
  float hoverOpacity = 0.25;
};
class Tikka_Seq : public IControl, public IVectorBase {
public:


  Tikka_Seq(IRECT& bounds, IColor _color, EuqSeq& _seq) : IControl(bounds, NULL), IVectorBase(DEFAULT_STYLE), seq(_seq)
  {
    //seq = _seq;
    stepSize = mRECT.W() / 32;
    color = _color;
    char* _name = "EVERY";


  };

  IRECT step(const int i, bool full = false) {

    auto base = mRECT.GetHSliced(mRECT.W() / seq.length);
    auto shifted = base.GetHShifted(i * base.W());
    /*auto squashed = shifted.GetPadded(5);*/
    auto scaled = shifted.GetFromBottom(seq.steps.at(i).vel * base.H());
    if (full) {
      return shifted;
    }
    else {
      return scaled;
    }
  }

  void DrawStepMenu(IGraphics& p) {

  }
  void Draw(IGraphics& p) override {
    //p.FillRect(color.WithOpacity(0.1), mRECT.GetFromLeft(stepSize * seq.length));

    for (size_t i = 0; i < seq.length; i++)
    {
      if (i == seq.current_step) {
        p.FillRoundRect(color.WithOpacity(0.25), step(i, true), 2);
      }
      if (seq.steps.at(i).vel > 0) {
        p.FillRoundRect(color, step(i), 2);
        if (seq.steps.at(i).customParam) {
          p.FillRoundRect(color.WithOpacity(0.5).WithContrast(-1), step(i), 2);
          p.DrawRoundRect(COLOR_WHITE, step(i).GetAltered(-1,-1, 1,1), 2);


        }
      }

      if (i == seq.current_step && seq.steps.at(seq.current_step).vel > 0) {
        p.FillRoundRect(color, step(i, false), 2);

      }
      
      if (i == hovered) {
        p.DrawRoundRect(IColor(255, 11, 32, 39), step(i).GetScaledAboutCentre(0.8), 2);
      }
    }
    p.DrawRoundRect(color, mRECT, 2);

    // cover with translucent rect if muted 
    if (seq.muted) {
      auto darkCol = IColor(255, 11, 32, 39);
      p.FillRoundRect(darkCol.WithOpacity(0.75), mRECT.GetScaledAboutCentre(1.05), 2);
    }
    if (drawStepParam && hovered != -1 && seq.steps.at(hovered).customParam) {
      auto area = step(hovered).GetAltered(50, 0, 50, 0);
      //auto t1 = ;
      p.DrawText(IText(19, EAlign::Near, COLOR_WHITE), std::to_string(seq.steps.at(hovered).every).c_str(), area);
      //auto t2 = ;
      p.DrawText(IText(19, EAlign::Far, COLOR_WHITE), std::to_string(seq.steps.at(hovered).bars).c_str(), area);
    }
  };

  void OnMouseOut() override {
    hovered = -1;
  }

  void OnMouseOver(float x, float y, const IMouseMod& mod)override {


    //if (mDown) return;
    for (size_t i = 0; i < seq.length; i++) {
      if (step(i, true).Contains(x, y) && seq.steps.at(i).vel > 0) {
        hovered = i;
        return;
      }
    }
    hovered = -1;

  }


  void OnMouseDown(float x, float y, const IMouseMod& mod) override {
    if (mRECT.Contains(x, y)) {
      mDown = true;

      lx = 0;
      ly = 0;
      startOffsetX = x;
      startOffsetY = y;
      //return;
    }
    if (hovered != -1) {
      seq.steps.at(hovered).customParam = !seq.steps.at(hovered).customParam;
    }


  };
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override {

    if (mDown) {


      lx += dX * 0.5;
      ly += dY * 0.5;
      /// -------------- Horizontal  dragging --------------
      if (lx > stepSize) {
        if (hovered != -1) {
          seq.steps.at(hovered).every++;
        }
        else {
          seq.offset++;
          seq.recalc();

        }
        lx = 0;
      }
      if (lx < -stepSize) {
        if (hovered != -1) {
          seq.steps.at(hovered).every--;

        }
        else {
          seq.offset--;
          seq.recalc();

        }
        lx = 0;
      }
 /// -------------- Vertical dragging --------------

      if (ly > (stepSize*2)) {
        if (hovered != -1) {
          seq.steps.at(hovered).bars--;
        }
        else {
        }
        ly = 0;

      }
      if (ly < -(stepSize*2)) {
        if (hovered != -1) {
          seq.steps.at(hovered).bars++;

        }
        else {
        }
        ly = 0;
      }

    }
  };
  void OnMouseUp(float x, float y, const IMouseMod& mod) override {
    if (mDown) {
      mDown = false;

    }

  };

private:
  EuqSeq& seq;
  bool mDown = false;
  int mouseActiveSlider = 0;
  float lx, startOffsetX, ly, startOffsetY;
  float stepSize;
  IColor color;
  int hovered = -1;
  int selected = -1;
  bool drawStepParam = true;
  struct StepParamInfo {
    int x;
    int y;
  };

};



class Tikka_MenuItem : public IControl, public IVectorBase {
public:
  Tikka_MenuItem(IRECT& bounds, IColor _color, std::string _txt, std::function<void()> _func) : IControl(bounds, NULL), IVectorBase(DEFAULT_STYLE)
  {
    txt = _txt.c_str();
    func = _func;
    color = _color;
    style = IText(bounds.GetScaled(0.9).H(), _color, nullptr, EAlign::Center, EVAlign::Middle);
  };

  void Draw(IGraphics& p) override {
    p.DrawRect(color.WithOpacity(hoverOpacity - 0.25), mRECT);
    p.DrawText(style.WithFGColor(color.WithOpacity(hoverOpacity)), txt.c_str(), mRECT);
  }
  void OnMouseDown(float x, float y, const IMouseMod& mod) override {

  };
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override {

  };
  void OnMouseUp(float x, float y, const IMouseMod& mod) override {
    func();
  };
  void OnMouseOver(float x, float y, const IMouseMod& mod)override {
    hoverOpacity <= 1.0f ? hoverOpacity += 0.05 : 0;

  };
  void OnMouseOut()override {
    hoverOpacity = 0.25;

  };
  int clamp(const int& n, const int& lower, const int& upper) {
    return std::max(lower, std::min(n, upper));
  }


private:
  std::function<void()> func;
  bool mDown = false;
  IColor color;
  float hoverOpacity = 0.25;
  std::string txt;
  IText style;
};
class Tikka_Button : public IVectorBase
{
public:
  Tikka_Button(const IRECT& _bounds, std::function<void()> aF, IColor _color, std::string _name, bool _isToggle = false, YGAlign _align = YGAlign::YGAlignAuto) : IVectorBase(NULL)

  {
    align = _align;
    color = _color;
    /*mText = style.valueText;*/
    mRECT = _bounds;
    name = _name;
    func = aF;
    darkCol = IColor(255, 11, 32, 39);
    isToggle = _isToggle;
    toggleVal = false;
  }

  void Draw(IGraphics& g)
  {
    if (hover) {
      hoverOpacity <= 1.0f ? hoverOpacity += 0.15 : 0;
    }
    if (!isToggle || (isToggle && !toggleVal)) {
      g.FillRect(color.WithOpacity(0.5 - hoverOpacity), mRECT);
      g.DrawRoundRect(darkCol.WithOpacity(1 - hoverOpacity), mRECT, 3, 0, 2);
      g.DrawText(IText(mRECT.GetScaled(0.55).H(), darkCol.WithOpacity(1 - hoverOpacity), nullptr, EAlign::Center, EVAlign::Middle), name.c_str(), mRECT);
      g.FillRoundRect(darkCol.WithOpacity(hoverOpacity), mRECT, 3);
      g.DrawText(IText(mRECT.GetScaled(0.55).H(), color.WithOpacity(hoverOpacity), nullptr, EAlign::Center, EVAlign::Middle), name.c_str(), mRECT);
    }
    else {
      g.FillRect(darkCol, mRECT);
      g.DrawText(IText(mRECT.GetScaled(0.55).H(), color.WithOpacity(hoverOpacity + 0.5), nullptr, EAlign::Center, EVAlign::Middle), name.c_str(), mRECT);
    }
  }
  void OnMouseOver(float x, float y, const IMouseMod& mod) {
    hover = true;

  };
  void OnMouseOut() {
    hoverOpacity = 0;
    hover = false;


  };

  bool IsHit(float x, float y) const
  {
    return mRECT.Contains(x, y);
  }
  ~Tikka_Button() {};
  IColor color;
  float hoverOpacity = 0;
  IRECT mRECT;
  std::function<void()> func;
  std::string name;
  IColor darkCol;
  bool hover = false;
  YGAlign align;
  bool isToggle, toggleVal;

};

class Tikka_OverlayMenu : public IControl, public IVectorBase {
public:
  Tikka_OverlayMenu(IRECT& bounds, Tikka_Layout _layout, IColor _color, std::function<void()> _func) : IControl(bounds, NULL), IVectorBase(DEFAULT_STYLE)
  {
    layout = _layout;
    func = _func;
    color = _color;
    expandedRect = bounds;
    collapse();

  };
  void Draw(IGraphics& p) override {
    if (!isCollapsed) {
      p.FillRect(color.WithOpacity(0.5), mRECT);
      for (size_t i = 0; i < items.size(); i++)
      {
        items.at(i).Draw(p);
      }
      for (auto& k : knobs) {
        k.Draw(p);
      }

    }

    //p.DrawText(style.WithFGColor(color.WithOpacity(hoverOpacity)), txt.c_str(), mRECT);
  }

  void collapse() {
    isCollapsed = true;
    mRECT = expandedRect.GetFromRight(0);
    mIgnoreMouse = true;
  }
  void expand() {
    isCollapsed = false;
    mRECT = expandedRect;
    mIgnoreMouse = false;

  }
  //void OnMouseDown(float x, float y, const IMouseMod& mod) override {

  //};
  //void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override {

  //};
  void OnMouseUp(float x, float y, const IMouseMod& mod) override {
    for (size_t i = 0; i < items.size(); i++)
    {
      if (items.at(i).IsHit(x, y))
      {
        items.at(i).func();
        if (items.at(i).isToggle) items.at(i).toggleVal = !items.at(i).toggleVal;
        break;
      }
    }

  };
  void OnMouseOver(float x, float y, const IMouseMod& mod)override {
    hoverOpacity <= 1.0f ? hoverOpacity += 0.05 : 0;
    for (size_t i = 0; i < items.size(); i++)
    {
      if (items.at(i).IsHit(x, y)) {
        items.at(i).OnMouseOver(x, y, mod);
      }
      else {
        items.at(i).OnMouseOut();
      }
    }

  };
  void OnMouseOut()override {
    hoverOpacity = 0.25;
    collapse();

  };
  int clamp(const int& n, const int& lower, const int& upper) {
    return std::max(lower, std::min(n, upper));
  }

  bool isCollapsed = true;
  void AddButton(Tikka_Button b) {

    items.push_back(b);

  }
  void recalcItemsPosition() {

    IFlexBox fb;
    fb.Init(expandedRect, layout.dir, layout.justify, layout.wrap, layout.pad, layout.margin);
    for (size_t i = 0; i < items.size(); i++)
    {
      if (layout.calc_FromText) {
        float w = items.at(i).name.length() * (items.at(i).mRECT.H() * 0.35);
        items.at(i).mRECT = IRECT().MakeXYWH(0, 0, w, items.at(i).mRECT.H());
      }
      fb.AddItem(items.at(i).mRECT, items.at(i).align ? items.at(i).align : YGAlign::YGAlignStretch, 1, 0, 3);
    }
    fb.CalcLayout();
    auto items_bounds = fb.GetItemBounds();
    for (int i = 0; i < items_bounds.size(); i++)
    {
      items.at(i).mRECT = items_bounds.at(i);
    }

  }

private:
  std::function<void()> func;
  bool mDown = false;
  IColor color;
  float hoverOpacity = 0.25;
  std::string txt;
  IText style;
  IRECT expandedRect;
  std::vector<Tikka_Button> items;
  std::vector<Tikka_NumberKnobInt> knobs;
  Tikka_Layout layout;
};






END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE

