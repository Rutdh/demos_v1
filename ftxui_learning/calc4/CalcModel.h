#pragma once

#include <glog/logging.h>
#include <ftxui/component/component.hpp>
#include <vector>
#include <gflags/gflags.h>

using namespace ftxui;

struct HistoryEntry {
  std::string expression;
  std::string result;
};

class CalculatorModel {
public:
  void Clear() {

  }
private:

  std::string expression_;
  std::string result = "\0";
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

  
};