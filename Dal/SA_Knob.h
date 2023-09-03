#pragma once

#include "IControl.h"
#include "IColorPickerControl.h"
#include "IVKeyboardControl.h"
#include "IControls.h"
#include <functional>
#include "SA_Vector2D.h"


BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

#pragma mark - Vector Controls

  class SA_Knob : public IKnobControlBase
  , public IVectorBase
{
public:

  struct KnobStyle {
    vec2f size = vec2f(100, 100);
    IColor trackColor = COLOR_WHITE;
    bool valInWidget = false;
    bool valueIsEditable = false;
    float a1 = -135.f;
    float a2 = 135.f;
    float aAnchor = -135.f;
    EDirection direction = EDirection::Vertical;
    double gearing = DEFAULT_GEARING;
    float trackSize = 2.f;
    IText textStyle = IText().WithSize(size.y*0.2);
    IVStyle style = IVStyle().WithValueText(IText().WithSize(size.y * 0.2).WithVAlign(EVAlign::Bottom));
  };


  SA_Knob::SA_Knob(const IRECT bounds, int paramIdx, KnobStyle style, const char* label = "", IActionFunction aF = [](IControl* c) {})
    : IKnobControlBase(bounds, paramIdx, style.direction, style.gearing)
    , IVectorBase(style.style, false, style.valInWidget)
    , mAngle1(style.a1)
    , mAngle2(style.a2)
    , mAnchorAngle(style.aAnchor)
  {

    DisablePrompt(!style.valueIsEditable);
    mText = style.textStyle;
    mText.mSize = style.size.y * 0.2;
    mHideCursorOnDrag = mStyle.hideCursor;
    
    mTrackSize = style.trackSize;
    mStyle.labelText = style.textStyle;
    track_color = style.trackColor;
    track_color.SetOpacity(0.75);

    SetActionFunction(aF);
    AttachIControl(this, label);

  }



  SA_Knob::SA_Knob(const vec2f position, int paramIdx, KnobStyle style, const char* label = "", IActionFunction aF = [](IControl* c) {})
    : IKnobControlBase(IRECT().MakeXYWH(position.x, position.y, style.size.x, style.size.y), paramIdx, style.direction, style.gearing)
    , IVectorBase(style.style, false, style.valInWidget)
    , mAngle1(style.a1)
    , mAngle2(style.a2)
    , mAnchorAngle(style.aAnchor)
  {
    DisablePrompt(!style.valueIsEditable);
    mText = style.textStyle;
    mText.mSize = style.size.y * 0.2;
    mHideCursorOnDrag = mStyle.hideCursor;
    
    mTrackSize = style.trackSize;
    mStyle.labelText = style.textStyle;
    track_color = style.trackColor;
    track_color.SetOpacity(0.75);
    
    SetActionFunction(aF);
    AttachIControl(this, label);

  }

  virtual ~SA_Knob() {}
  
 
  void OnMouseOut() override { track_color.SetOpacity(0.75);  mValueMouseOver = false; IKnobControlBase::OnMouseOut(); }

  void SetInnerPointerFrac(float frac) { mInnerPointerFrac = frac; }
  void SetOuterPointerFrac(float frac) { mOuterPointerFrac = frac; }
  void SetPointerThickness(float thickness) { mPointerThickness = thickness; }
 


  void Draw(IGraphics& g) override
  {
    DrawBackground(g, mRECT);
    //DrawLabel(g);
    g.DrawText(mText, mLabelStr.Get(), mRECT.FracRectVertical(0.2, true), 0);
    DrawWidget(g);
    DrawValue(g, mValueMouseOver);
  }

  virtual IRECT SA_Knob::GetKnobDragBounds() override
  {
    IRECT r;
    if (mWidgetBounds.W() > mWidgetBounds.H())
      r = mWidgetBounds.GetCentredInside(mWidgetBounds.H() / 2.f, mWidgetBounds.H());
    else
      r = mWidgetBounds.GetCentredInside(mWidgetBounds.W(), mWidgetBounds.W() / 2.f);

    return r;
  }

  void DrawWidget(IGraphics& g) override
  {
    float widgetRadius; // The radius out to the indicator track arc
    float sc = 2.25f;
    if (mWidgetBounds.W() > mWidgetBounds.H())
      widgetRadius = (mWidgetBounds.H() / sc);
    else
      widgetRadius = (mWidgetBounds.W() / sc);

    const float cx = mWidgetBounds.MW(), cy = mWidgetBounds.MH();

    widgetRadius -= (mTrackSize / sc);

    IRECT knobHandleBounds = mWidgetBounds.GetCentredInside((widgetRadius - mTrackToHandleDistance) * 2.f);
    const float angle = mAngle1 + (static_cast<float>(GetValue()) * (mAngle2 - mAngle1));
    //DrawIndicatorTrack
    if (mTrackSize > 0.f)
    {
      g.DrawArc(track_color, cx, cy, widgetRadius, angle >= mAnchorAngle ? mAnchorAngle : mAnchorAngle - (mAnchorAngle - angle), angle >= mAnchorAngle ? angle : mAnchorAngle, &mBlend, mTrackSize);
    }
    
    //drawEllipse

    IRECT handleBounds = knobHandleBounds;
    IRECT centreBounds = knobHandleBounds.GetPadded(-mRECT.H() * 0.15);
    IRECT shadowBounds = knobHandleBounds.GetTranslated(mStyle.shadowOffset, mStyle.shadowOffset);
    const IBlend blend = mControl->GetBlend();
    const float contrast = IsDisabled() ? -GRAYED_ALPHA : 0.f;
    if (mMouseDown)
    {
      g.FillEllipse(COLOR_BLACK, handleBounds/*, &blend*/);
    }
    else
    {
      g.FillEllipse(COLOR_BLACK, handleBounds/*, &blend*/);
      if (mMouseIsOver) {
        g.FillEllipse(COLOR_BLACK.WithOpacity(0.7), handleBounds, &blend);
        g.DrawEllipse(COLOR_WHITE, handleBounds, 0, 2);

        }
      
    }
    
    if (mMouseDown && mControl->GetAnimationFunction())
      DrawSplash(g, handleBounds);


    //if (mStyle.drawFrame)
      g.DrawEllipse(GetColor(kFR), handleBounds, 0, mStyle.frameThickness);


    // draw pointer;
    float radius = knobHandleBounds.W() / 2.f;
    g.DrawRadialLine(track_color, cx, cy, angle, mInnerPointerFrac * radius, mOuterPointerFrac * radius, &mBlend, mPointerThickness);
  }
  void OnMouseDown(float x, float y, const IMouseMod& mod) override
  {
    if (mStyle.showValue && mValueBounds.Contains(x, y))
    {
      PromptUserInput(mValueBounds);
    }
    else
    {
      IKnobControlBase::OnMouseDown(x, y, mod);
    }

    SetDirty(false);
  }

  void OnMouseUp(float x, float y, const IMouseMod& mod) override
  {
    IKnobControlBase::OnMouseUp(x, y, mod);
    SetDirty(true);
  }

  void OnMouseOver(float x, float y, const IMouseMod& mod) override
  {
    track_color.SetOpacity(1);
    
    if (mStyle.showValue && !mDisablePrompt)
      mValueMouseOver = mValueBounds.Contains(x, y);

    IKnobControlBase::OnMouseOver(x, y, mod);
    
    SetDirty(true);
  }

  void OnResize() override
  {
    SetTargetRECT(MakeRects(mRECT));
    SetDirty(false);
  }

  bool SA_Knob::IsHit(float x, float y) const
  {
    if (!mDisablePrompt)
    {
      if (mValueBounds.Contains(x, y))
        return true;
    }

    return mWidgetBounds.Contains(x, y);
  }

  void SetDirty(bool push, int valIdx = kNoValIdx) override
  {
    IKnobControlBase::SetDirty(push);

    const IParam* pParam = GetParam();

    if (pParam)
      pParam->GetDisplayWithLabel(mValueStr);
  }

  void OnInit() override
  {
    const IParam* pParam = GetParam();

    if (pParam)
    {
      pParam->GetDisplayWithLabel(mValueStr);

      if (!mLabelStr.GetLength())
        mLabelStr.Set(pParam->GetName());
    }
  }


protected:
  IColor track_color;
  float mTrackToHandleDistance = 4.f;
  float mInnerPointerFrac = 0.1f;
  float mOuterPointerFrac = 1.f;
  float mPointerThickness = 5.5f;
  float mAngle1, mAngle2;
  float mAnchorAngle; // for bipolar arc
  bool mValueMouseOver = false;
};

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
