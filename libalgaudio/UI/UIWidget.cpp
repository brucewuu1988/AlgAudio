#include "UI/UIWidget.hpp"
#include <iostream>
#include "Window.hpp"

namespace AlgAudio{

void UIWidget::Draw(DrawContext& c){
  if(visible){
    const Size2D newsize = c.Size();
    if(newsize != last_drawn_size){
      cache_texture->Resize(newsize);
    }
    if(newsize != last_drawn_size || needs_redrawing){
      c.Push(cache_texture, newsize.width, newsize.height);
      c.Clear();
      CustomDraw(c);
      c.Pop();
    }
    c.DrawTexture(cache_texture);
    last_drawn_size = newsize;
    needs_redrawing = false;
  }
}

void UIWidget::SetNeedsRedrawing(){
  needs_redrawing = true;
  auto p = parent.lock();
  auto w = window.lock();
  if(p) p->SetNeedsRedrawing();
  else if(w) w->SetNeedsRedrawing();
}

} // namespace AlgAudio