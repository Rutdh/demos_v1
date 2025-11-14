#include "CalcView.h"
#include "CalcModel.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <utility>
#include <vector>

CalcView::CalcView() :
  rows_ {
    {"C", "(", ")", "BS"},
    {"7", "8", "9", "/"},
    {"4", "5", "6", "*"},
    {"1", "2", "3", "-"},
    {"0", ".", "=", "+"},
  } {
    for (const auto &row : rows_) {
      std::vector<Component> buttons;
      for (const auto &label : row) {
        buttons.push_back(CreateButtonByLabel(label));
      }

      horizontal_rows_.push_back(Container::Horizontal(std::move(horizontal_rows_)));
    }

    auto button_grid = Container::Vertical(std::move(horizontal_rows_));

    std::vector<std::string> history_labels;
    int history_selected = 0;
    MenuOption history_menu_option;
    history_menu_option.on_enter = [&] {
      if (history_selected >= 0 &&
        history_selected < controller_.History().size()) {
          controller_.LoadHistoryEntry(history_selected);
        }
    };

    auto history_menu = Menu(&history_labels, &history_selected, history_menu_option);
    auto clear_history_button = Button("Clear History", [&]{
      controller_.ClearHistory();
    });
    auto history_container = Container::Vertical(
      {history_menu, clear_history_button}
    );
    auto root_container = Container::Vertical(
      {button_grid, history_container}
    );

    auto main_component = Renderer(root_container, [&] {
      history_labels.clear();
      for (const auto &entry : controller_.History()) {
        history_labels.push_back(entry.expression + " = " + entry.result);
      }

      if (!history_labels.empty()) {
        
      }
    };

    )

}

Component CalcView::CreateButtonByLabel(const std::string& label) {
  auto action = [&] {
    LOG(INFO) << "[CreateButtonByLabel] label " << label;
    if (label == "C") {
      controller_.Clear();
    } else if (label == "BS") {
      controller_.Backspace();
    } else if (label == "=") {
      controller_.Equals();
    } else if (label == ")") {
      controller_.RightParen();
    } else if (label == "(") {
      controller_.LeftParen();
    } else if (label == "-" || label == "+" || label == "*" || label == "/") {
      controller_.Operator(label.front());
    } else {
      controller_.Digit(label.front());
    }
  };

  auto option = ButtonOption::Ascii().Border();
  auto base_option = option.transform;
  option.transform = [base_option](const EntryState& state) {
    Element element = base_option(state);
    return element | size(ftxui::WIDTH, ftxui::EQUAL, 3)
      | size(ftxui::HEIGHT, ftxui::EQUAL, 3);
  };
  
  return Button(label, std::move(action), option);
}