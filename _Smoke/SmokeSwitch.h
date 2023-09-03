#pragma once
#include "metropolis/MP_Colors.h"
#include "IControl.h"
#include <cmath>
#include "Easing.h"


namespace MP {
  using namespace iplug;
  using namespace iplug::igraphics;

  class Switch : public IControl, IVectorBase
  {
  public:
    Switch(const IRECT& bounds, const int paramID, IActionFunction onClickFunc ) : IControl(bounds, paramID), IVectorBase(DEFAULT_STYLE)
    {
      labelRect = bounds.GetReducedFromBottom(bounds.H()/2);
      mStyle.labelText = mStyle.labelText.WithFont("Brixton-Bold");
      mStyle.labelText.mAlign = EAlign::Center;
      mStyle.labelText.mSize = labelRect.H() * 0.35;
      mStyle.labelText.mVAlign = EVAlign::Middle;
      mStyle.labelText.mFGColor = MP::pallete.LIGHT_GRAY;
      func = onClickFunc;
      

      modes = { "G R A N U L A R", "S T R E T C H", "L O O P   D E L A Y", "S P E C T R A L" };
      /// make points to act as switches
      const int num = 4;
      radius = std::min((mRECT.W() / num), mRECT.H()/2) *0.8 ;
      for (size_t i = 0; i < num; i++)
      {
        float x = mRECT.W() / num;
        float x2 = x / 2;

        Point p{ x2+ (x*i) + mRECT.L, (mRECT.H() * 0.75) + mRECT.T };
        points.push_back(p);
      }

      
      
      colors = { MP::pallete.ACC_RED, MP::pallete.ACC_GOLD, MP::pallete.ACC_BLUE,  MP::pallete.ACC_PURPLE, MP::pallete.ACC_GREEN };
      selected = 0;
      previousSelected = -1;
      currentColor = colors.at(selected);
    }

    ~Switch()
    {
    }
    void Draw(IGraphics& g) override;

  private:
    struct Point { double x; double y; };
    void OnMouseDown(float x, float y, const IMouseMod& mod) override;
    void OnMouseUp(float x, float y, const IMouseMod& mod) override;
    void OnMouseOver(float x, float y, const IMouseMod& mod) override;
    void OnMouseOut() override;
    void OnEndAnimation() override;
    std::vector<Point> points;
    float distance(Point a, Point b) {
      return std::hypot(a.x - b.x, a.y - b.y);
    }
    char mTitle[64];
    std::vector<std::string> modes;
    float radius;
    IRECT labelRect;
    IColor currentColor, previousColor;
    int selected, previousSelected;
    std::vector<IColor> colors;
    float progress = 1;
    IActionFunction func;
  };

  void Switch::Draw(IGraphics& g)
  {
    //g.DrawRect(COLOR_WHITE, labelRect, 0, 2);
    g.DrawText(mStyle.labelText.WithFGColor(currentColor), modes[selected].c_str(), labelRect, 0);
    //g.DrawRect(COLOR_WHITE, labelRect.GetAltered(20, 0, -20, 0), 0, 0.5);
    
    g.DrawLine(currentColor, mRECT.L, mRECT.MH(), mRECT.R, mRECT.MH(), 0, mRECT.H() * 0.04);
    //g.DrawRect(COLOR_WHITE, mRECT);
    for (size_t i = 0; i < points.size(); i++)
    {
      const auto p = points.at(i);
      if (i == selected) {
        g.FillCircle(currentColor, p.x, p.y, (radius / 2)+progress* (radius*0.05));
        continue;
      }
      if (i == previousSelected) {
        g.FillCircle(MP::pallete.LIGHT_GRAY, p.x, p.y, (radius / 2) - progress * (radius * 0.2));
        continue;
      }
      else {
        g.FillCircle(MP::pallete.LIGHT_GRAY, p.x, p.y, (radius / 2) - (radius * 0.2));
      }
    }
    
  }

  void Switch::OnMouseDown(float x, float y, const IMouseMod& mod)
  {

  }


  void Switch::OnMouseUp(float x, float y, const IMouseMod& mod)
  {
    for (size_t i = 0; i < points.size(); i++)
    {
      if (distance(Point{ x,y }, points.at(i)) < radius / 2) {
        previousSelected = selected;
        selected = i;
        previousColor = currentColor;
        SetValue(selected / 4.0);
        func(this);
        SetAnimation([&](IControl* pCaller) {
           auto currentProgress = static_cast<float>(pCaller->GetAnimationProgress());

          if (currentProgress> 1.f) {
            pCaller->OnEndAnimation();
            return;
          }

          progress = currentProgress;
          currentColor = IColor::LinearInterpolateBetween(previousColor, colors.at(selected), currentProgress);
          SetDirty(true);
        },
          200);
        
        return;

      }
    }
  }
  
   

  void Switch::OnMouseOver(float x, float y, const IMouseMod& mod)
  {

  }


  void Switch::OnMouseOut()
  {

  }

  void Switch::OnEndAnimation()
  {
    currentColor = colors.at(selected);
  }

}