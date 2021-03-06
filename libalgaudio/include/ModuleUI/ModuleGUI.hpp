#ifndef MODULEGUI_HPP
#define MODULEGUI_HPP
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
#include "Module.hpp"
#include "Canvas.hpp"
#include "UI/UIWidget.hpp"

namespace AlgAudio{

namespace Exceptions{
struct GUIBuild : public Exception{
  GUIBuild(std::string t) : Exception(t) {}
};
} // namespace Exceptions

/** Merely an interface implemented by standard box types. Module creators
 *  may wish to implement a custom version of a module GUI, by inheriting
 *  from this class and writing a custom Module::BuildGUI.
 *
 *  Each Module has zero or one corresponding ModuleGUI. ModuleGUIs are drawn by
 *  the CanvasView.
 */
class ModuleGUI{
public:
  /** The position of this module on the parent CanvasView. */
  Point2D& position() { return module.lock()->position_in_canvas; }
  
  /** Returns the widget representing this module. Cannot return nullptr.
   *  Must be implemented by all ModuleGUI specialisations.
   *  ModuleGUI is just an interface defining a set of methods. It cannot
   *  inherit from UIWidget, as that would mess up inheritance completely.
   *  Tuhs, to access the underlyin widget use this method.
   *  Custom implementations, like StandardModuleGUI, return a pointer to
   *  itself, because StandardModuleGUI inherts from both ModuleGUI and
   *  UIWidget.*/
  virtual std::shared_ptr<UIWidget> Widget() = 0;
  
  /** Gets the corresponding module instance */
  std::shared_ptr<Module> GetModule(){ return module.lock(); }
  
  /** When set to true, the module shall draw itself in it's "highlighted"
   *  variant. */
  virtual void SetHighlight(bool) = 0;

  ///@{
  /** These methods are used by the CanvasView to query where it should draw
   *  connection wire endings. The only parameter is iolet ID. The ModuleGUI
   *  implementation should return the coordinates where the connector is
   *  drawn. */
  virtual Point2D WhereIsInlet(std::string inlet) = 0;
  virtual Point2D WhereIsOutlet(std::string outlet) = 0;
  virtual Point2D WhereIsParamInlet(std::string inlet) = 0;
  virtual Point2D WhereIsParamRelativeOutlet(std::string inlet) = 0;
  virtual Point2D WhereIsParamAbsoluteOutlet(std::string inlet) = 0;
  ///@}

  /** This method shall translate an inlet/outlet widget id to the corresponding
   *  param id. */
  virtual std::string GetIoletParamID(UIWidget::ID) const = 0;

  /** CanvasView uses this function to notify the ModuleGUI that a slider is
   *  being dragged. This way the drag can continue outside the ModuleGUI.
   *  The only arguments is the slider widget id (as returned by WhatIsHere).
   *  Usually, the ModuleGUI will pass this event to the right slider widget. */
  virtual void SliderDragStart(UIWidget::ID){}
  /** This method is called by CanvasView for each step of a slider drag.
   *  Note that offset values may be outside moduleGUI, or even negative.
   *  Usually, the ModuleGUI will pass this event to the right slider widget. */
  virtual void SliderDragStep(UIWidget::ID, Point2D_<float>){}
  /** This method is called by CanvasView when a slider drag has ended.
   *  Usually, the ModuleGUI will pass this event to the right slider widget. */
  virtual void SliderDragEnd(UIWidget::ID){}

  /** A Module may call this method sometimes to notify its GUI that the set of
   *  inlets has changed, and the ModuleGUI should take appropriate changes. */
  virtual void OnInletsChanged() {};

  enum class WhatIsHereType{
    Nothing,
    Inlet,
    Outlet,
    SliderInput,
    SliderOutputRelative,
    SliderOutputAbsolute,
    SliderBody,
  };
  struct WhatIsHere{
    WhatIsHereType type;
    /** The id of the widget */
    UIWidget::ID widget_id;
    /** The id of the param/inlet/outlet corresponding to this widget */
    std::string param_id;
  };
  /** This function is used by the CanvasView to ask the module GUI what kind of
   *  element is located at a given point. This way the CanvasView can handle
   *  connections etc. and ModuleGUI does not have to bother about them. */
  virtual WhatIsHere GetWhatIsHere(Point2D) const = 0;

protected:
  ModuleGUI(std::shared_ptr<Module> mod) : module(mod){}
  // A link to the module instance this GUI represents.
  std::weak_ptr<Module> module;
};

} // namespace AlgAudio

#endif //MODULEGUI_HPP
