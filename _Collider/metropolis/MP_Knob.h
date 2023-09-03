#pragma once
#include "MP_Colors.h"
#include "IControl.h"

namespace MP {
  using namespace iplug;
  using namespace iplug::igraphics;
  
  typedef std::vector<IColor> ColorScheme;
  const ColorScheme knobDefault = { MP::pallete.LIGHT_GRAY , MP::pallete.BG_KNOB_LOW, MP::pallete.BG_KNOB };
  class KnobControl : public IKnobControlBase
    , public IVectorBase
  {
  public:
    KnobControl(const IRECT& bounds, int paramIdx,
      const char* label = "", const ColorScheme& colorScheme = knobDefault,
      const IVStyle& style = DEFAULT_STYLE,
      bool valueIsEditable = false, bool valueInWidget = false,
      float a1 = -135.f, float a2 = 135.f, float aAnchor = -135.f,
      EDirection direction = EDirection::Vertical, double gearing = DEFAULT_GEARING);

    KnobControl(const IRECT& bounds, IActionFunction aF,
      const char* label = "",
      const IVStyle& style = DEFAULT_STYLE,
      bool valueIsEditable = false, bool valueInWidget = false,
      float a1 = -135.f, float a2 = 135.f, float aAnchor = -135.f,
      EDirection direction = EDirection::Vertical, double gearing = DEFAULT_GEARING);

    virtual ~KnobControl() {}

    void Draw(IGraphics& g) override;
    virtual void DrawWidget(IGraphics& g) override;
    virtual void DrawIndicatorTrack(IGraphics& g, float angle, float cx, float cy, float radius);
    virtual void DrawPointer(IGraphics& g, float angle, float cx, float cy, float radius);

    void OnMouseDown(float x, float y, const IMouseMod& mod) override;
    void OnMouseUp(float x, float y, const IMouseMod& mod) override;
    void OnMouseOver(float x, float y, const IMouseMod& mod) override;
    void OnMouseOut() override { mValueMouseOver = false; IKnobControlBase::OnMouseOut(); SetDirty(true); }

    void OnResize() override;
    bool IsHit(float x, float y) const override;
    void SetDirty(bool push, int valIdx = kNoValIdx) override;
    void OnInit() override;

    void SetInnerPointerFrac(float frac) { mInnerPointerFrac = frac; }
    void SetOuterPointerFrac(float frac) { mOuterPointerFrac = frac; }
    void SetPointerThickness(float thickness) { mPointerThickness = thickness; }

  protected:
    virtual IRECT GetKnobDragBounds() override;

    float mTrackToHandleDistance = 4.f;
    float mInnerPointerFrac = 0.1f;
    float mOuterPointerFrac = 1.f;
    float mPointerThickness = 2.5f;
    float mAngle1, mAngle2;
    float mAnchorAngle; // for bipolar arc
    bool mValueMouseOver = false;

    IColor BG_KNOB;
    IColor BG_KNOB_LOW;
    IColor LIGHT_GRAY;

  };

  KnobControl::KnobControl(const IRECT& bounds, int paramIdx, const char* label, const ColorScheme& colorScheme, const IVStyle& style, bool valueIsEditable, bool valueInWidget, float a1, float a2, float aAnchor, EDirection direction, double gearing)
    : IKnobControlBase(bounds, paramIdx, direction, gearing)
    , IVectorBase(style, false, valueInWidget)
    , mAngle1(a1)
    , mAngle2(a2)
    , mAnchorAngle(aAnchor)
  {
    DisablePrompt(!valueIsEditable);
    mText = style.valueText;
    mHideCursorOnDrag = mStyle.hideCursor;
    
    BG_KNOB = colorScheme[2];
    BG_KNOB_LOW = colorScheme[1];
    LIGHT_GRAY = colorScheme[0];

    mStyle.valueText.mAlign = igraphics::EAlign::Center;
    mStyle.valueText.mVAlign = igraphics::EVAlign::Middle;
    mStyle.valueText.mFGColor = MP::pallete.KNOB_VALUE;
    mStyle.valueText = mStyle.valueText.WithFont("Dense-Regular");
    mStyle.valueText.mSize = bounds.H() * 0.3;
    mStyle.labelText = mStyle.labelText.WithFont("Jost-Italic");
    mStyle.labelText.mFGColor = MP::pallete.KNOB_VALUE;
    mStyle.showValue = false;


    AttachIControl(this, label);
  }

  KnobControl::KnobControl(const IRECT& bounds, IActionFunction aF, const char* label, const IVStyle& style, bool valueIsEditable, bool valueInWidget, float a1, float a2, float aAnchor, EDirection direction, double gearing)
    : IKnobControlBase(bounds, kNoParameter, direction, gearing)
    , IVectorBase(style, false, valueInWidget)
    , mAngle1(a1)
    , mAngle2(a2)
    , mAnchorAngle(aAnchor)
  {
    DisablePrompt(!valueIsEditable);
    mText = style.valueText;
    mHideCursorOnDrag = mStyle.hideCursor;

    SetActionFunction(aF);
    AttachIControl(this, label);
  }

  void KnobControl::Draw(IGraphics& g)
  {
    DrawBackground(g, mRECT);
    DrawLabel(g);
    DrawWidget(g);
    if (mValueMouseOver) {
      g.DrawText(mStyle.valueText, std::to_string((int)(GetValue() * 100.0f)).c_str(), mWidgetBounds, 0);

    }
    else {
      g.DrawText(mStyle.valueText.WithFGColor(MP::pallete.KNOB_VALUE.WithOpacity(0.5)), std::to_string((int)(GetValue() * 100.0f)).c_str(), mWidgetBounds, 0);

    }
  }

  IRECT KnobControl::GetKnobDragBounds()
  {
    IRECT r;

    if (mWidgetBounds.W() > mWidgetBounds.H())
      r = mWidgetBounds.GetCentredInside(mWidgetBounds.H() / 2.f, mWidgetBounds.H());
    else
      r = mWidgetBounds.GetCentredInside(mWidgetBounds.W(), mWidgetBounds.W() / 2.f);

    return r;
  }

  void KnobControl::DrawWidget(IGraphics& g)
  {
    float widgetRadius; // The radius out to the indicator track arc

    if (mWidgetBounds.W() > mWidgetBounds.H())
      widgetRadius = (mWidgetBounds.H() / 2.f);
    else
      widgetRadius = (mWidgetBounds.W() / 2.f);

    const float cx = mWidgetBounds.MW(), cy = mWidgetBounds.MH();

    widgetRadius -= (mTrackSize / 2.f);

    IRECT knobHandleBounds = mWidgetBounds.GetCentredInside((widgetRadius - mTrackToHandleDistance) * 2.f);
    const float angle = mAngle1 + (static_cast<float>(GetValue()) * (mAngle2 - mAngle1));

    float indicatorThickness = 8;
    //Draw background
    g.FillCircle(BG_KNOB, knobHandleBounds.MW(), knobHandleBounds.MH(), widgetRadius);
    //DrawIndicator background
    //g.DrawCircle(MP::pallete.BG_KNOB_LOW, cx, cy, widgetRadius * 0.8, 0, indicatorThickness);
    g.DrawArc(BG_KNOB_LOW, cx, cy, widgetRadius * 0.8, 135.f, -135.f, 0, indicatorThickness);
    /// Draw indicator

    g.DrawArc(LIGHT_GRAY, cx, cy, widgetRadius * 0.8, angle >= -135.f ? -135.f : -135.f - (-135.f - angle), angle >= -135.f ? angle : -135.f, 0, indicatorThickness);

    //DrawPointer(g, angle, cx, cy, knobHandleBounds.W() / 2.f);
  }

  void KnobControl::DrawIndicatorTrack(IGraphics& g, float angle, float cx, float cy, float radius)
  {

  }

  void KnobControl::DrawPointer(IGraphics& g, float angle, float cx, float cy, float radius)
  {
    g.DrawRadialLine(COLOR_WHITE, cx, cy, angle, mInnerPointerFrac * radius, mOuterPointerFrac * radius, &mBlend, mPointerThickness);

  }

  void KnobControl::OnMouseDown(float x, float y, const IMouseMod& mod)
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

  void KnobControl::OnMouseUp(float x, float y, const IMouseMod& mod)
  {
    IKnobControlBase::OnMouseUp(x, y, mod);
    SetDirty(true);
  }

  void KnobControl::OnMouseOver(float x, float y, const IMouseMod& mod)
  {
    mValueMouseOver = true;
    /*if (mStyle.showValue && !mDisablePrompt)
      mValueMouseOver = mValueBounds.Contains(x, y);*/

    IKnobControlBase::OnMouseOver(x, y, mod);

    SetDirty(true);
  }

  void KnobControl::OnResize()
  {
    SetTargetRECT(MakeRects(mRECT));
    SetDirty(false);
  }

  bool KnobControl::IsHit(float x, float y) const
  {
    if (!mDisablePrompt)
    {
      if (mValueBounds.Contains(x, y))
        return true;
    }

    return mWidgetBounds.Contains(x, y);
  }

  void KnobControl::SetDirty(bool push, int valIdx)
  {
    IKnobControlBase::SetDirty(push);

    const IParam* pParam = GetParam();

    if (pParam)
      pParam->GetDisplayWithLabel(mValueStr);
  }

  void KnobControl::OnInit()
  {
    const IParam* pParam = GetParam();

    if (pParam)
    {
      pParam->GetDisplayWithLabel(mValueStr);

      if (!mLabelStr.GetLength())
        mLabelStr.Set(pParam->GetName());
    }
  }

}