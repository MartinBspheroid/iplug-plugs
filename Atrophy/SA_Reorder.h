#pragma once
#include "config.h"
#include <algorithm>
#include "IControl.h"



namespace SA {

  using namespace iplug;
  using namespace igraphics;

  class ReorderItems : public IControl, public IVectorBase {
  public:
    struct Item {
      Item(ISVG _icon) : icon(_icon) {};
      IColor color;
      int pos;
      std::string name;
      ISVG icon;
     
    };
    ReorderItems(IRECT& bounds, std::vector<ISVG> icons, std::function<void(std::vector<int> order)> _func) : IControl(bounds, NULL), IVectorBase(DEFAULT_STYLE)
    {
      
      reorderFunction = _func;
      std::vector<std::string> names = { "TRANSISTOR", "DIODE", "COIL", "VINYL", "TAPE", "CAP", "POWER SAG" };
      for (size_t i = 0; i < icons.size(); i++)
      {
        Item it(icons[i]);
        it.color = IColor().GetRandomColor();
        it.name = names[i];

        it.pos = i;
        items.push_back(it);
        rects.push_back(IRECT(0, 0, 75, 75).GetTranslated(mRECT.L, mRECT.T).GetTranslated(50 + (i * 80), 0).GetAltered(5, 5, -5, -5));
      }
      UpdateOrder();

    };
    void Draw(IGraphics& p) override {
      p.DrawRect(COLOR_BLACK, mRECT);
      auto debugRect = mRECT.GetFromTop(20);
      std::string dbgTxt = "dragging " + std::to_string(drIndex) + " accIndex " + std::to_string(accIndex) + " >>  ";
      for (size_t x = 0; x < items.size(); x++)
      {
        dbgTxt += std::to_string(items.at(x).pos) + "  ";
      }
      //p.DrawText(IText().WithFont("Poppins-LightItalic").WithFGColor(COLOR_WHITE), dbgTxt.c_str(), debugRect, 0);
      auto nameRect = mRECT.GetFromBottom(20);
      p.DrawText(IText().WithFGColor(COLOR_WHITE).WithSize(nameRect.H()*0.8), showName.c_str(), nameRect, 0);

      for (size_t i = 0; i < items.size(); i++)
      {
        if (drIndex != i) {
          auto& it = items.at(i);
          //p.FillRoundRect(it.color, rects.at(i), 5);
          auto s = std::to_string(it.pos);
          p.DrawText(IText(), s.c_str(), rects.at(i));
          p.DrawSVG(it.icon, rects.at(i));
        }
      }
      if ((dragging && drIndex >= 0) && !tap) {

        auto draggingRect = rects.at(drIndex).GetTranslated(xf, yf).GetScaledAboutCentre(1.2);
        //p.DrawRoundRect(draggingCol, draggingRect, 5);
        p.DrawSVG(items.at(drIndex).icon,draggingRect);
        for (size_t j = 0; j < items.size(); j++)
        {
          auto& ci = items.at(j);
          if (j == drIndex) continue;
          accIndex = -1;

          if (rects.at(j).Contains(draggingRect.MW(), draggingRect.MH())) {
            auto ar = rects.at(j).GetScaledAboutCentre(1.2);
            //p.DrawSVG(ci.icon,ar);

            //p.DrawRect(COLOR_WHITE, ar);
            accIndex = j;
            if (draggingRect.MW() > ar.MW()) {
              p.DrawLine(COLOR_WHITE, ar.R + 2, ar.T - 2, ar.R + 2, ar.B + 2, 0, 4);
            }
            else {
              p.DrawLine(COLOR_WHITE, ar.L - 2, ar.T - 2, ar.L - 2, ar.B + 2, 0, 4);

            }
            break;
          }

        }

      }


    }

    void OnMouseDown(float x, float y, const IMouseMod& mod) override {
      
      
      for (size_t i = 0; i < items.size(); i++)
      {
        if (rects.at(i).Contains(x, y)) {

          drIndex = i;
          xf = yf = 0;
          tap = true;
          std::string msg = "Tapped " + items.at(i).name;
          DBGMSG(msg.c_str());
          SetDirty(true);
          return;
        }
      }
      
    };
    void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override {
      SetDirty(true);
      if (tap && abs(dX) > 0.1) {
        dragging = true;
        tap = false;
      }
      if (dragging) {
        xf += dX;

        //yf += dY;
      }
    };
    void OnMouseUp(float x, float y, const IMouseMod& mod) override {
      tap = false;
      if (dragging && accIndex != -1) {

        shift(drIndex, accIndex);
        UpdateOrder();

      }

      dragging = false;
      drIndex = -1;
      accIndex = -1;
      SetDirty(true);

    };

    void UpdateOrder()
    {
      std::vector<int> newOrder; /// straight out of Manchester!!!!
      for (size_t i = 0; i < items.size(); i++)
      {
        newOrder.push_back(items.at(i).pos);
      }
      reorderFunction(newOrder);
    }

    //   https://www.bfilipek.com/2014/12/top-5-beautiful-c-std-algorithms.html



    void shift(const int from, const int to) {
      auto _from = items.begin() + from;
      auto _to = items.begin() + to;


        if (_from < _to) {
          std::rotate(_from, _from + 1, _to + 1);
        }
        else if (_from > _to) {
          std::rotate(_to, _from, _from + 1);
        }

    }

    
    void OnMouseOver(float x, float y, const IMouseMod& mod) override {
      
      for (size_t i = 0; i < rects.size(); i++)
      {
        if (rects.at(i).Contains(x, y)) {

          showName = items.at(i).name;
          return;
        }
      }
      //SetDirty(true);
    };
    void OnMouseOut()override {
      showName = "";

    };
    int clamp(const int& n, const int& lower, const int& upper) {
      return std::max(lower, std::min(n, upper));
    }
    std::function<void(std::vector<int> order)> reorderFunction;
  private:
    
    std::string showName = "";
    bool tap = false;
    int hover;
    int xf, yf;
    bool dragging = false;
    IColor draggingCol = COLOR_BLUE;
    int drIndex = -1;
    int accIndex = -1;
    std::vector<Item> items;
    std::function<void()> func;
    bool mDown = false;
    IColor color;
    float hoverOpacity = 0.25;
    std::string txt;
    IText style;
    IRECT expandedRect;
    std::vector<IRECT> rects;


  };




}// end SA namespace

