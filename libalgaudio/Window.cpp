/*
This file is part of AlgAudio.

AlgAudio, Copyright (C) 2015 CeTA - Audiovisual Technology Center

AlgAudio is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

AlgAudio is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with AlgAudio.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Window.hpp"
#include <SDL2/SDL.h>
#include <iostream>
#include "UI/DrawContext.hpp"
#include "UI/UIWidget.hpp"
#include "SDLMain.hpp"

namespace AlgAudio{

Window::Window(std::string t, int w, int h, bool centered) :
  title(t), width(w), height(h)
{
  //std::cout << "Creating a new window" << std::endl;
  // TODO: investigate SDL_WINDOW_INPUT_GRABBED
  window = SDL_CreateWindow(title.c_str(), (centered)?SDL_WINDOWPOS_CENTERED:40, (centered)?SDL_WINDOWPOS_CENTERED:40, width, height, SDL_WINDOW_RESIZABLE);
  if(!window) throw SDLException("Unable to create a window");
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
  SDL_RendererInfo r;
  SDL_GetRendererInfo(renderer,&r);
  std::cout << "New renderer: " << r.name << std::endl;
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  if(!renderer) throw SDLException("Unable to create a renderer");
  id = SDL_GetWindowID(window);
}

std::shared_ptr<Window> Window::Create(std::string title, int w, int h, bool centered){
  std::shared_ptr<Window> res(new Window(title,w,h,centered));
  return res;
}

Window::~Window(){
  std::cout << "Destructing window " << title << std::endl;
  //std::cout << "destroying renderer" << renderer << std::endl;
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void Window::Render(){
#ifdef __unix__
  if(!needs_redrawing){
	  SDL_RenderPresent(renderer);
    return;
  }
#else
  if(!needs_redrawing) return;
#endif
  std::cout << "Redrawing window" << std::endl;
  Size2D size = GetSize();
  DrawContext c(renderer, 0, 0, size.width, size.height); // Full window DC
  c.SetColor(Theme::Get("bg-main"));
	SDL_RenderClear(renderer);
  if(child) child->Draw(c);
	SDL_RenderPresent(renderer);

  needs_redrawing = false;
}

void Window::Insert(std::shared_ptr<UIWidget> ch){
  child = ch;
  //child->SetWindow(shared_from_this());
  child->parent.reset();

  child->Resize(GetSize());
}

void Window::SetNeedsRedrawing(){
  needs_redrawing = true;
}

void Window::ProcessCloseEvent(){
  on_close.Happen();
  SDL_HideWindow(window);
}

void Window::ProcessEnterEvent(){
  if(!child) return;
  // The SDL enter event has no mouse position, but is always followed by a
  // motion event. We'll mark that we an enter event, and we will send it to the
  // widgets once we receive the next motion event.
  mouse_just_entered = true;
}

void Window::ProcessMotionEvent(Point2D p){
  if(child){
    if(mouse_just_entered){
      child->OnMotionEnter(p);
      mouse_just_entered = false;
    }
    child->OnMotion(prev_motion, p);
  }
  prev_motion = p;
}


void Window::ProcessLeaveEvent(){
  if(!child) return;
  // The SDL leave event has no mouse position, but is always preceded by a
  // motion event
  child->OnMotionLeave(prev_motion);
}

void Window::ProcessResizeEvent(){
  if(!child) return;
  child->Resize(GetSize());
  needs_redrawing = true;
}

void Window::ProcessMouseButtonEvent(bool d, short b, Point2D p){
  if(child)
    child->OnMouseButton(d,b,p);
}

Size2D Window::GetSize() const{
  int w,h;
  SDL_GetWindowSize(window, &w, &h);
  return Size2D(w,h);
}

} // namespace AlgAudio
