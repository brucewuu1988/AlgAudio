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
#include "LaunchConfigWindow.hpp"
#include "SCLang.hpp"
#include "Theme.hpp"
#include "Canvas.hpp"
#include "MainWindow.hpp"
#include "SDLMain.hpp"
#include "Version.hpp"
#include "Config.hpp"

namespace AlgAudio{

LaunchConfigWindow::LaunchConfigWindow() : Window("AlgAudio config",290,425){
}

void LaunchConfigWindow::init(){
  marginbox = UIMarginBox::Create(shared_from_this(),10,10,2,10);
  startbutton = UIButton::Create(shared_from_this(),"Start!");
  testbutton = UIButton::Create(shared_from_this(),"Test UI");
  aboutbutton = UIButton::Create(shared_from_this(),"About");
  testbutton->SetDisplayMode(UIWidget::DisplayMode::Invisible);
  quitbutton = UIButton::Create(shared_from_this(),"Quit App");
  titlelabel = UILabel::Create(shared_from_this(),"AlgAudio",52);
  configlabel = UILabel::Create(shared_from_this(),"This place is left for config.");
  oscchkbox = UICheckbox::Create(shared_from_this(),"Enable OSC debugging");
  oscchkbox->SetDisplayMode(UIWidget::DisplayMode::Invisible);
  supernovachkbox = UICheckbox::Create(shared_from_this(),"Enable Supernova mode");
  debugchkbox = UICheckbox::Create(shared_from_this(),"Enable debug tools");
  mainvbox = UIVBox::Create(shared_from_this());
  configbox = UIVBox::Create(shared_from_this());
  buttonhbox = UIHBox::Create(shared_from_this());
  progressbar = UIProgressBar::Create(shared_from_this());
  statustext = UILabel::Create(shared_from_this(),"AlgAudio (C) CeTA 2015, released on GNU LGPL 3",12);
  statustext->SetCustomSize(Size2D(0,32));
  layered = UILayered::Create(shared_from_this());
  about_box = UIVBox::Create(shared_from_this());
  about_box->SetDisplayMode(UIWidget::DisplayMode::Invisible);
  about_text = UILabel::Create(shared_from_this(),
    "Copyright (C) 2015 CeTA - Audiovisual Technology Center\n"
    "Copyright (C) 2015 Rafal Cieslak\n"
    "\n"
    "Released on the terms of the GNU Lesser General Public\n"
    "License version 3. See LICENCE file for details, or go to:\n"
    "      https://www.gnu.org/licenses/lgpl.txt"
    ,12);
  about_text->SetAlignment(HorizAlignment_CENTERED, VertAlignment_TOP);
  version_label = UILabel::Create(shared_from_this(), ALGAUDIO_VERSION, 16);
  version_label->SetAlignment(HorizAlignment_CENTERED, VertAlignment_TOP);
  config_separator = UISeparator::Create(shared_from_this());
  config_separator->SetCustomSize(Size2D(0,20));
  sclang_path_selector = UIPathSelector::Create(shared_from_this(), Config::Global().path_to_sclang);
#ifdef __UNIX__
  path_label = UILabel::Create(shared_from_this(), "Path to SuperCollider's sclang binary", 14);
#else
  path_label = UILabel::Create(shared_from_this(), "Path to SuperCollider's sclang.exe", 14);
#endif

  Insert(marginbox);
  marginbox->Insert(mainvbox);
   mainvbox->Insert(titlelabel, UIBox::PackMode::TIGHT);
   mainvbox->Insert(version_label, UIBox::PackMode::TIGHT);
   mainvbox->Insert(config_separator, UIBox::PackMode::TIGHT);
   mainvbox->Insert(layered, UIBox::PackMode::WIDE);
    layered->Insert(configbox);
     configbox->Insert(path_label, UIBox::PackMode::TIGHT);
     configbox->Insert(sclang_path_selector, UIBox::PackMode::TIGHT);
     configbox->Insert(configlabel, UIBox::PackMode::WIDE);
     configbox->Insert(oscchkbox, UIBox::PackMode::TIGHT);
     configbox->Insert(supernovachkbox, UIBox::PackMode::TIGHT);
     configbox->Insert(debugchkbox, UIBox::PackMode::TIGHT);
    layered->Insert(about_box);
      about_box->Insert(about_text, UIBox::PackMode::WIDE);
   mainvbox->Insert(buttonhbox, UIBox::PackMode::TIGHT);
    buttonhbox->Insert(quitbutton, UIHBox::PackMode::WIDE);
    buttonhbox->Insert(testbutton, UIHBox::PackMode::WIDE);
    buttonhbox->Insert(aboutbutton, UIHBox::PackMode::WIDE);
    buttonhbox->Insert(startbutton, UIHBox::PackMode::WIDE);
   mainvbox->Insert(progressbar, UIBox::PackMode::TIGHT);
   mainvbox->Insert(statustext, UIBox::PackMode::TIGHT);

  startbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-positive"));
  quitbutton->SetColors(Theme::Get("text-button"), Theme::Get("bg-button-negative"));

  subscriptions += startbutton->on_clicked.Subscribe([this](){
    
    ApplyConfig();
    
    statustext->SetTextColor(Theme::Get("text-generic"));
    statustext->SetBold(false);
    statustext->SetText("Starting...");
    // Hide the start button
    startbutton->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    start_in_progress = true;
    
    SCLang::Start();
  });
  subscriptions += SCLang::on_start_progress.Subscribe([this](int n, std::string msg){
    progressbar->SetAmount(n/10.0);
    statustext->SetAlignment(HorizAlignment_CENTERED,VertAlignment_TOP);
    statustext->SetText(msg);
  });

  subscriptions += testbutton->on_clicked.Subscribe([this](){
    ApplyConfig();
    // Disable SC usage through the app. All inlets will be fake, etc.
    Config::Global().do_not_use_sc = true;
    on_complete.Happen();
  });
  subscriptions += aboutbutton->on_clicked.Subscribe([this](){
    ToggleAbout();
  });

  subscriptions += quitbutton->on_clicked.Subscribe([this](){
    on_close.Happen();
  });
  subscriptions += oscchkbox->on_toggled.Subscribe([](bool state){
    SCLang::SetOSCDebug(state);
  });
  subscriptions += debugchkbox->on_toggled.Subscribe([this](bool enabled){
    if(enabled){
      console = Console::Create();
      subscriptions += console->on_close.Subscribe([this](){
        debugchkbox->SetActive(false); // This will also unregister the window.
      });
      SDLMain::RegisterWindow(console);
      oscchkbox->SetDisplayMode(UIWidget::DisplayMode::Visible);
      testbutton->SetDisplayMode(UIWidget::DisplayMode::Visible);
      aboutbutton->SetDisplayMode(UIWidget::DisplayMode::Invisible);
    }else{
      SDLMain::UnregisterWindow(console);
      oscchkbox->SetDisplayMode(UIWidget::DisplayMode::Invisible);
      testbutton->SetDisplayMode(UIWidget::DisplayMode::Invisible);
      aboutbutton->SetDisplayMode(UIWidget::DisplayMode::Visible);
      console = nullptr;
    }
  });
  subscriptions += SCLang::on_start_completed.Subscribe([this](bool success, std::string message){
    if(success){
      on_complete.Happen();
    }else{
      start_in_progress = false;
      statustext->SetText(message);
      statustext->SetBold(true);
      statustext->SetTextColor(Theme::Get("text-error"));
      statustext->SetAlignment(HorizAlignment_LEFT,VertAlignment_TOP);
      progressbar->SetAmount(0);
      startbutton->SetDisplayMode(UIWidget::DisplayMode::Visible);
    }
  });
}

std::shared_ptr<LaunchConfigWindow> LaunchConfigWindow::Create(){
  auto res = std::shared_ptr<LaunchConfigWindow>( new LaunchConfigWindow());
  res->init();
  return res;
}

void LaunchConfigWindow::ToggleAbout(){
  about_displayed = !about_displayed;
  if(about_displayed){
    quitbutton->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    startbutton->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    progressbar->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    statustext->SetDisplayMode(UIWidget::DisplayMode::EmptySpace);
    
    configbox->SetDisplayMode(UIWidget::DisplayMode::Invisible);
    about_box->SetDisplayMode(UIWidget::DisplayMode::Visible);
    
    aboutbutton->SetText("Back");
  }else{
    quitbutton->SetDisplayMode(UIWidget::DisplayMode::Visible);
    startbutton->SetDisplayMode(start_in_progress ? UIWidget::DisplayMode::EmptySpace : UIWidget::DisplayMode::Visible);
    progressbar->SetDisplayMode(UIWidget::DisplayMode::Visible);
    statustext->SetDisplayMode(UIWidget::DisplayMode::Visible);
    
    configbox->SetDisplayMode(UIWidget::DisplayMode::Visible);
    about_box->SetDisplayMode(UIWidget::DisplayMode::Invisible);
    
    aboutbutton->SetText("About");
  }
}

void LaunchConfigWindow::ApplyConfig(){
  Config& c = Config::Global();
  c.path_to_sclang = sclang_path_selector->GetPath();
  c.supernova = supernovachkbox->GetActive();
}

} // namespace AlgAudio
