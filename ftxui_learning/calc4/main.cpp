#include <cctype>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

// ---------------------- Domain model ----------------------------------------
class CalculatorModel {
 public:
  void Clear() {
    expression_.clear();
    result_ = "0";
    error_.clear();
  }

  void Backspace() {
    if (!expression_.empty()) {
      expression_.pop_back();
      error_.clear();
    }
  }

  void AppendDigit(char digit) {
    expression_.push_back(digit);
    error_.clear();
  }

  void AppendDot() {
    if (CurrentNumberHasDot()) {
      return;
    }
    if (expression_.empty() || IsOperator(expression_.back())) {
      expression_ += "0";
    }
    expression_.push_back('.');
    error_.clear();
  }

  void AppendOperator(char op) {
    if (expression_.empty()) {
      // allow unary minus
      if (op == '-') {
        expression_.push_back(op);
      }
      return;
    }
    const char last = expression_.back();
    if (IsOperator(last)) {
      if (op == '-') {
        const bool last_is_unary =
            expression_.size() == 1 ||
            IsOperator(expression_[expression_.size() - 2]);
        if (!last_is_unary) {
          // enable "--" so expressions like "5--2" are possible
          expression_.push_back(op);
        } else {
          expression_.back() = op;
        }
      } else {
        expression_.back() = op;
      }
    } else {
      expression_.push_back(op);
    }
    error_.clear();
  }

  void Evaluate() {
    if (expression_.empty()) {
      result_ = "0";
      error_.clear();
      return;
    }

    double value = 0.0;
    if (EvaluateExpression(expression_, value)) {
      std::ostringstream oss;
      oss << value;
      result_ = oss.str();
      error_.clear();
    } else {
      error_ = "Invalid expression";
    }
  }

  const std::string& expression() const { return expression_; }
  const std::string& result() const { return result_; }
  const std::string& error() const { return error_; }

 private:
  static bool IsOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
  }

  bool CurrentNumberHasDot() const {
    for (auto it = expression_.rbegin(); it != expression_.rend(); ++it) {
      if (IsOperator(*it)) {
        break;
      }
      if (*it == '.') {
        return true;
      }
    }
    return false;
  }

  static bool EvaluateExpression(const std::string& expr, double& out) {
    auto precedence = [](char op) {
      return (op == '+' || op == '-') ? 1 : 2;
    };

    auto apply = [](std::vector<double>& values, std::vector<char>& ops) {
      if (values.size() < 2 || ops.empty()) {
        return false;
      }
      double b = values.back();
      values.pop_back();
      double a = values.back();
      values.pop_back();
      char op = ops.back();
      ops.pop_back();

      double res = 0.0;
      switch (op) {
        case '+': res = a + b; break;
        case '-': res = a - b; break;
        case '*': res = a * b; break;
        case '/':
          if (b == 0.0) {
            return false;
          }
          res = a / b;
          break;
        default: return false;
      }
      values.push_back(res);
      return true;
    };

    std::vector<double> values;
    std::vector<char> ops;
    for (size_t i = 0; i < expr.size();) {
      if (std::isspace(expr[i])) {
        ++i;
        continue;
      }

      if (std::isdigit(expr[i]) || expr[i] == '.' ||
          (expr[i] == '-' && (i == 0 || IsOperator(expr[i - 1])))) {
        size_t start = i;
        bool has_dot = expr[i] == '.';
        ++i;
        while (i < expr.size() &&
               (std::isdigit(expr[i]) || (!has_dot && expr[i] == '.'))) {
          if (expr[i] == '.') {
            has_dot = true;
          }
          ++i;
        }
        double value = std::stod(expr.substr(start, i - start));
        values.push_back(value);
        continue;
      }

      if (IsOperator(expr[i])) {
        char op = expr[i];
        while (!ops.empty() && precedence(ops.back()) >= precedence(op)) {
          if (!apply(values, ops)) {
            return false;
          }
        }
        ops.push_back(op);
        ++i;
        continue;
      }

      return false;  // unsupported character
    }

    while (!ops.empty()) {
      if (!apply(values, ops)) {
        return false;
      }
    }

    if (values.size() != 1) {
      return false;
    }
    out = values.back();
    return true;
  }

  std::string expression_;
  std::string result_ = "0";
  std::string error_;
};

// ---------------------- Controller ------------------------------------------
class CalculatorController {
 public:
  explicit CalculatorController(CalculatorModel model = {})
      : model_(std::move(model)) {}

  void OnChange(std::function<void()> cb) { on_change_ = std::move(cb); }

  void Digit(char digit) { model_.AppendDigit(digit); Notify(); }
  void Operator(char op) { model_.AppendOperator(op); Notify(); }
  void Dot() { model_.AppendDot(); Notify(); }
  void Clear() { model_.Clear(); Notify(); }
  void Backspace() { model_.Backspace(); Notify(); }
  void Equals() { model_.Evaluate(); Notify(); }

  const std::string& expression() const { return model_.expression(); }
  const std::string& result() const { return model_.result(); }
  const std::string& error() const { return model_.error(); }

 private:
  void Notify() {
    if (on_change_) {
      on_change_();
    }
  }

  CalculatorModel model_;
  std::function<void()> on_change_;
};

// ---------------------- View / App wiring -----------------------------------
int main() {
  CalculatorController controller;
  auto screen = ScreenInteractive::TerminalOutput();
  controller.OnChange([&] { screen.Post(Event::Custom); });

  auto button_for = [&](const std::string& label) {
    auto action = [&, label] {
      if (label == "C") {
        controller.Clear();
      } else if (label == "BS") {
        controller.Backspace();
      } else if (label == "=") {
        controller.Equals();
      } else if (label == ".") {
        controller.Dot();
      } else if (label == "+" || label == "-" ||
                 label == "*" || label == "/") {
        controller.Operator(label.front());
      } else {
        controller.Digit(label.front());
      }
    };

    auto option = ButtonOption::Ascii();
    option.border = true;
    option.transform = [](Element e) {
      return e | size(WIDTH, EQUAL, 5) | size(HEIGHT, EQUAL, 3);
    };
    return Button(label, std::move(action), option);
  };

  std::vector<std::vector<std::string>> rows = {
      {"7", "8", "9", "/"},
      {"4", "5", "6", "*"},
      {"1", "2", "3", "-"},
      {"0", ".", "=", "+"},
      {"C", "BS"},
  };

  std::vector<Component> horizontal_rows;
  for (const auto& row : rows) {
    std::vector<Component> buttons;
    for (const auto& label : row) {
      buttons.push_back(button_for(label));
    }
    horizontal_rows.push_back(Container::Horizontal(std::move(buttons)));
  }
  auto button_grid = Container::Vertical(std::move(horizontal_rows));

  auto main_component = Renderer(button_grid, [&] {
    auto expr_panel =
        window(text("Expression"),
               text(controller.expression().empty()
                        ? "enter expression..."
                        : controller.expression()) |
                   bold);

    auto status_text = controller.error().empty()
                           ? text("Result: " + controller.result()) |
                                 color(Color::Green)
                           : text("Error: " + controller.error()) |
                                 color(Color::Red) | bold;

    return vbox({
               expr_panel,
               separator(),
               window(text("Status"), status_text),
               separator(),
               button_grid->Render(),
           }) |
           border | size(WIDTH, GREATER_THAN, 40);
  });

  screen.Loop(main_component);
  return 0;
}
