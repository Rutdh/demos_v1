#pragma once

#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include <string>
#include <unordered_map>
#include "ftxui/component/screen_interactive.hpp"  // for Component, 


// Component input_expr = Input()

using namespace ftxui;

class UI {
public:
  UI();

  void Render();
private:
  std::string expr_;
  Component input_expr_;
  Element ouput_result_;
  Component renderer_;
};