#ifndef UIBUTTON_HPP
#define UIBUTTON_HPP
#include "UIWidget.hpp"

namespace AlgAudio{

class UIButton : public UIWidget{
public:
  static std::shared_ptr<UIButton> Create(std::weak_ptr<Window> parent_window, std::string text);
  Signal<> on_clicked;
  void SetText(std::string);
  virtual void CustomDraw(DrawContext& c) override;
  virtual void OnMouseButton(bool,short,int,int) override;
  virtual void OnMotionEnter() override;
  virtual void OnMotionLeave() override;
private:
  UIButton(std::weak_ptr<Window> parent_window, std::string text);
  std::string text;
  std::shared_ptr<SDLTexture> texture;
  void UpdateTexture();
  bool pressed = false;
  bool pointed = false;
};

} // namespace AlgAudio

#endif // UIBUTTON_HPP