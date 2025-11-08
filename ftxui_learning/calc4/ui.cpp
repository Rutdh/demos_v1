#include "ui.h"
#include "ftxui/component/screen_interactive.hpp"
#include <utility>

UI::UI() {
  input_expr_ = Input(&expr_, "please input calc expr");
  ouput_result_ = text("");

  // 布局
  auto component = Container::Vertical({
    input_expr_,
  });

  // 渲染树
  renderer_ = Renderer(component, [&](){
    return vbox({
      hbox(text("input expr"), input_expr_->Render()),
      separator(),
      ouput_result_,
    }) | border;
  });
}

void UI::Render() {
  auto screen = ScreenInteractive::TerminalOutput();
  screen.Loop(renderer_);
}