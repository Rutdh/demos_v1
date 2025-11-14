#pragma once

#include <cctype>
#include <cstddef>
#include <ftxui/component/component.hpp>
#include <ftxui/util/ref.hpp>
#include <gflags/gflags.h>
#include <gflags/gflags_declare.h>
#include <glog/logging.h>
#include <string>
#include <vector>

DECLARE_int32(max_history_entries);

using namespace ftxui;

struct HistoryEntry {
  std::string expression;
  std::string result;
};

class CalculatorModel {
public:
  void Clear();

  void Backspace();

  void AppendDigit(char digit);

  void AppendDot();

  void AppendOperator(char op);

  void AppendLeftParen();

  void AppendRightParen();

  void Evaluate();

  const std::string& expression() const {return expression_;}
  const std::string& result() const {return result_;}
  const std::string& error() const {return error_;}
  const std::vector<HistoryEntry>& history() const {return history_;}

  void ClearHistory() {
    history_.clear();
  }

  void SetExpression(const std::string& expr) {
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

private:
  void PushHistoryEntry(const std::string &expr, const std::string &result);

  std::string expression_;
  std::string result_ = "0";
  std::string error_;
  std::vector<HistoryEntry> history_;

  int UnmatchedLeftParentheses() const {
    int balance = 0;
    for (char c : expression_) {
      if (c == '(') {
        balance++;
      } else if (c == ')') {
        balance--;
      }
    }

    return balance;
  }

  static bool EvaluateExpression(const std::string &expr, double &out);
};