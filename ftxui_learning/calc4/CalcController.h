#pragma once
#include "CalcModel.h"
#include <cstddef>
#include <ftxui/dom/node.hpp>
#include <functional>
#include <utility>

class CalculatorController {
public:
  explicit CalculatorController(CalculatorModel model = {})
    : model_(std::move(model)) {}
  
  void OnChange(std::function<void()> cb) {
    on_change_ = std::move(cb);
  }

  void Digit(char digit) {model_.AppendDigit(digit); Notify();}
  void Operator(char op) {model_.AppendOperator(op); Notify();}
  void Dot() {model_.AppendDot(); Notify();}
  void LeftParen() {model_.AppendLeftParen(); Notify();}
  void RightParen() {model_.AppendRightParen(); Notify();}
  void Clear() {model_.Clear(); Notify();}
  void Backspace() {model_.Backspace(); Notify();}
  void Equals() {model_.Evaluate(); Notify();}
  void ClearHistory() {model_.ClearHistory(); Notify();}

  void LoadHistoryEntry(size_t index);

  const std::string& expression() const { return model_.expression(); }
  const std::string& result() const { return model_.result();}
  const std::string& error() const { return model_.error();}
  const std::vector<HistoryEntry>& History() const { return model_.history();}

private:
  void Notify() {
    if (on_change_) {
      on_change_();
    }
  }

  CalculatorModel model_;
  std::function<void()> on_change_;
};