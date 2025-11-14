#include "ftxui/component/component_options.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <functional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <glog/logging.h>

using namespace ftxui;

struct HistoryEntry {
  std::string expression;
  std::string result;
};

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
    if (expression_.empty() || IsOperator(expression_.back()) ||
        expression_.back() == '(') {
      expression_ += "0";
    }

    expression_ += ".";
    error_.clear();
  }

  void AppendOperator(char op) {
    if (expression_.empty()) {
      // 为什么这里只允许-?
      if (op == '-') {
        expression_.push_back(op);
        error_.clear();
      }
      return;
    }

    const char last = expression_.back();
    if (last == '(') {
      if (op == '-') {
        expression_.push_back(op);
        error_.clear();
      }
      return;
    }

    if (IsOperator(last)) {
      if (op == '-') {
        const bool last_is_unary =
            expression_.size() == 1 ||
            IsOperator(expression_[expression_.size() - 2]) ||
            expression_[expression_.size() - 2] == '(';
        if (!last_is_unary) {
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

  void AppendLeftParen() {
    expression_.push_back('(');
    error_.clear();
  }

  void AppendRightParen() {
    if (expression_.empty()) {
      return;
    }
    if (UnmatchedLeftParentheses() <= 0) {
      return;
    }
    const char last = expression_.back();
    if (IsOperator(last) || last == '(') {
      return;
    }
    expression_.push_back(')');
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
      // 这里为什么要先输出到一个流里面
      std::ostringstream oss;
      oss << value;
      result_ = oss.str();
      PushHistoryEntry(expression_, result_);
      error_.clear();
    } else {
      LOG(INFO) << "Invalid expression: " << expression_;
      error_ = "Invalid expression.";
    }
  }

  const std::string &expression() const { return expression_; }
  const std::string &result() const { return result_; }
  const std::string &error() const { return error_; }
  const std::vector<HistoryEntry> &history() const { return history_; }

  void ClearHistory() { history_.clear(); }

  void SetExpression(const std::string &expr) {
    expression_ = expr;
    error_.clear();
  }

private:
  static bool IsOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/');
  }

  bool CurrentNumberHasDot() const {
    for (auto it = expression_.rbegin(); it != expression_.rend(); ++it) {
      if (IsOperator(*it) || *it == '(') {
        break;
      }
      if (*it == '.') {
        return true;
      }
    }
    return false;
  }

  static bool EvaluateExpression(const std::string &expr, double &out) {
    auto precedence = [](char op) {
      switch (op) {
      case '+':
      case '-':
        return 1;
      case '*':
      case '/':
        return 2;
      default:
        return 0;
      }
    };

    auto apply = [](std::vector<double> &values, std::vector<char> &ops) {
      if (values.size() < 2 || ops.empty()) {
        return false;
      }

      double b = values.back();
      values.pop_back();
      double a = values.back();
      values.pop_back();
      char op = ops.back();
      ops.pop_back();

      LOG(INFO) << "[apply]: a:" << a << " op:" << op << " b:" << b;

      double res = 0.0;
      switch (op) {
      case '+':
        res = a + b;
        break;
      case '-':
        res = a - b;
        break;
      case '*':
        res = a * b;
        break;
      case '/':
        if (b == 0.0) {
          return false;
        }
        res = a / b;
        break;
      default:
        return false;
      }
      values.push_back(res);
      return true;
    };

    auto is_unary_boundary = [&](size_t index) {
      if (index == 0) {
        return true;
      }
      char prev = expr[index - 1];
      return IsOperator(prev) || prev == '(';
    };

    std::vector<double> values;
    std::vector<char> ops;
    for (size_t i = 0; i < expr.size();) {
      if (std::isspace(expr[i])) {
        ++i;
        continue;
      }

      if (expr[i] == '(') {
        ops.push_back('(');
        ++i;
        continue;
      }

      if (expr[i] == ')') {
        while (!ops.empty() && ops.back() != '(') {
          if (!apply(values, ops)) {
            LOG(INFO) << "apply calc failed.";
            return false;
          }
        }
        if (ops.empty()) {
          LOG(INFO) << "Unmatched right parenthesis.";
          return false;
        }
        ops.pop_back();
        ++i;
        continue;
      }

      if (std::isdigit(expr[i]) || expr[i] == '.' ||
          (expr[i] == '-' && is_unary_boundary(i))) {
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
        LOG(INFO) << "value: " << value << " push back to values";
        continue;
      }

      if (IsOperator(expr[i])) {
        char op = expr[i];
        LOG(INFO) << "IsOperator before: " << op;
        while (!ops.empty() && ops.back() != '(' &&
               precedence(ops.back()) >= precedence(op)) {
          if (!apply(values, ops)) {
            LOG(INFO) << "apply calc failed.";
            return false;
          }
        }
        ops.push_back(op);
        LOG(INFO) << "op: " << op << " push back to ops";
        ++i;
        continue;
      }

      LOG(INFO) << "Unsupported character: " << expr[i];
      return false; // 不支持的情况
    }

    while (!ops.empty()) {
      if (ops.back() == '(') {
        LOG(INFO) << "Unmatched left parenthesis.";
        return false;
      }
      if (!apply(values, ops)) {
        LOG(INFO) << "apply calc failed.";
        return false;
      }
    }

    if (values.size() != 1) {
      LOG(INFO) << "unexpected values size: " << values.size();
      return false;
    }

    out = values.back();
    return true;
  }

private:
  void PushHistoryEntry(const std::string &expr, const std::string &result) {
    history_.push_back({expr, result});
    if (history_.size() > kMaxHistoryEntries) {
      history_.erase(history_.begin());
    }
  }

  std::string expression_;
  std::string result_ = "0";
  std::string error_;
  std::vector<HistoryEntry> history_;

  int UnmatchedLeftParentheses() const {
    int balance = 0;
    for (char c : expression_) {
      if (c == '(') {
        ++balance;
      } else if (c == ')') {
        --balance;
      }
    }
    return balance;
  }

  static constexpr size_t kMaxHistoryEntries = 20;
};

// ---------------------- Controller ------------------------------------------
class CalculatorController {
public:
  explicit CalculatorController(CalculatorModel model = {})
      : model_(std::move(model)) {}

  void OnChange(std::function<void()> cb) { on_change_ = std::move(cb); }

  void Digit(char digit) {
    model_.AppendDigit(digit);
    Notify();
  }
  void Operator(char op) {
    model_.AppendOperator(op);
    Notify();
  }
  void Dot() {
    model_.AppendDot();
    Notify();
  }
  void LeftParen() {
    model_.AppendLeftParen();
    Notify();
  }
  void RightParen() {
    model_.AppendRightParen();
    Notify();
  }
  void Clear() {
    model_.Clear();
    Notify();
  }
  void Backspace() {
    model_.Backspace();
    Notify();
  }
  void Equals() {
    model_.Evaluate();
    Notify();
  }
  void ClearHistory() {
    model_.ClearHistory();
    Notify();
  }

  void LoadHistoryEntry(size_t index) {
    const auto &history = model_.history();
    if (index >= history.size()) {
      return;
    }
    model_.SetExpression(history[index].expression);
    Notify();
  }

  const std::string &expression() const { return model_.expression(); }
  const std::string &result() const { return model_.result(); }
  const std::string &error() const { return model_.error(); }
  const std::vector<HistoryEntry> &History() const { return model_.history(); }

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
int main(int argc, char *argv[]) {
  FLAGS_log_dir = "./logs";
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << "hello, world";

  CalculatorController controller;
  // 这里为什么只有初始化的时候可以用=, 其他情况下用move也不行
  auto screen = ScreenInteractive::TerminalOutput();
  controller.OnChange([&] { screen.Post(Event::Custom); });

  auto button_for = [&](const std::string &label) {
    auto action = [&, label] {
      LOG(INFO) << "label: " << label;
      if (label == "C") {
        controller.Clear();
      } else if (label == "BS") {
        controller.Backspace();
      } else if (label == "=") {
        controller.Equals();
      } else if (label == "(") {
        controller.LeftParen();
      } else if (label == ")") {
        controller.RightParen();
      } else if (label == ".") {
        controller.Dot();
      } else if (label == "+" || label == "-" || label == "*" || label == "/") {
        controller.Operator(label.front());
      } else {
        controller.Digit(label.front());
      }
    };

    auto option = ButtonOption::Ascii().Border();
    auto base_option = option.transform;
    option.transform = [base_option](const EntryState &state) {
      Element element = base_option(state);
      return element | size(ftxui::WIDTH, ftxui::EQUAL, 5) |
             size(ftxui::HEIGHT, ftxui::EQUAL, 3);
    };

    return Button(label, std::move(action), option);
  };

  std::vector<std::vector<std::string>> rows = {
      {"C", "(", ")", "BS"}, {"7", "8", "9", "/"}, {"4", "5", "6", "*"},
      {"1", "2", "3", "-"},  {"0", ".", "=", "+"},
  };

  std::vector<Component> horizontal_rows;
  for (const auto &row : rows) {
    std::vector<Component> buttons;
    for (const auto &label : row) {
      buttons.push_back(button_for(label));
    }
    horizontal_rows.push_back(Container::Horizontal(std::move(buttons)));
  }
  auto button_grid = Container::Vertical(std::move(horizontal_rows));

  std::vector<std::string> history_labels;
  int history_selected = 0;
  MenuOption history_menu_option;
  history_menu_option.on_enter = [&] {
    if (history_selected >= 0 &&
        history_selected < static_cast<int>(controller.History().size())) {
      controller.LoadHistoryEntry(static_cast<size_t>(history_selected));
    }
  };
  auto history_menu =
      Menu(&history_labels, &history_selected, history_menu_option);
  auto clear_history_button =
      Button("Clear History", [&] { controller.ClearHistory(); });
  auto history_container =
      Container::Vertical({history_menu, clear_history_button});

  auto root_container = Container::Vertical({button_grid, history_container});

  /*
    这里为什么要把button_grid作为第一个参数传入, 有什么用吗,
    还是单纯只是这个函数的参数列表的要求如此而已
   */
  auto main_component = Renderer(root_container, [&] {
    history_labels.clear();
    for (const auto &entry : controller.History()) {
      history_labels.push_back(entry.expression + " = " + entry.result);
    }
    if (!history_labels.empty()) {
      history_selected = std::clamp(
          history_selected, 0, static_cast<int>(history_labels.size()) - 1);
    } else {
      history_selected = 0;
    }

    auto expr_panel =
        window(text("Expression"),
               text(controller.expression().empty() 
                  ? "enter expression..."
                  : controller.expression()) | bold);

    auto status_text =
        controller.error().empty()
            ? text("Result: " + controller.result()) | color(Color::Green)
            : text("Error: " + controller.error()) | color(Color::Red) | bold;

    Element history_body = history_labels.empty()
                               ? text("no history yet") | dim
                               : history_menu->Render() | frame |
                                     size(ftxui::HEIGHT, ftxui::LESS_THAN, 8) |
                                     vscroll_indicator;
    auto history_panel = window(text("History"), history_body);

    return vbox({
               expr_panel, separator(), window(text("Status"), status_text),
               separator(), history_panel, clear_history_button->Render(),
               separator(),
               button_grid->Render(), // 这里为什么要调用一下Render方法
           }) |
           border | size(ftxui::WIDTH, ftxui::GREATER_THAN, 40);
  });

  screen.Loop(main_component);
  return 0;
}
