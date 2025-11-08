#pragma once

#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include <string>
#include <unordered_map>


// ftxui::Component input_expr = Input()



class UI {
public:
  std::string expr_;
  ftxui::Component input_expr_;
  ftxui::Element ouput_result_;
  
};