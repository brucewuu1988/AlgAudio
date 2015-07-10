#ifndef Window_HPP
#define Window_HPP
#include <memory>
#include "SDLHandle.hpp"

struct SDL_Window;
struct SDL_Renderer;

namespace AlgAudio{

struct UIWidget;

class Window : public std::enable_shared_from_this<Window>{
private:
  SDLHandle h;
public:
  static std::shared_ptr<Window> Create(std::string title = "AlgAudio", int w = 350, int h = 300);
  ~Window();

  // explicitly forbid copying windows
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  void Render();

  void Insert(std::shared_ptr<UIWidget> child);

  void SetNeedsRedrawing();

  void ProcessCloseEvent();
  void ProcessMouseButtonEvent(bool down, short button, int x, int y);
  void ProcessMotionEvent(int x, int y);
  void ProcessEnterEvent();
  void ProcessLeaveEvent();
  void ProcessResizeEvent();

  template<class W, typename... Args>
  std::shared_ptr<W> Create(Args... args){
    return W::Create(shared_from_this(), args...);
  }

  unsigned int GetID() const {return id;}
  SDL_Renderer* GetRenderer() const {return renderer;}
private:
  Window(std::string title, int w, int h);
  std::string title;
  int width;
  int height;
  unsigned int id;
  SDL_Window* window;
  SDL_Renderer* renderer;

  int prev_motion_x = -1, prev_motion_y = -1;

  bool needs_redrawing = true;

  std::shared_ptr<UIWidget> child;
};

} //namespace AlgAudio
#endif // Window_HPP
