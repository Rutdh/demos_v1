#pragma once

#include <cctype>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>

using Expr = std::string;
struct CalcResult {
  double result_;
  bool   bCalc_;
};

class Calc4 {
public:
  int apply(int lhs, int rhs, char op);
  int precedence(char op);
  int evaluate(const std::string& expr);

private:
  std::unordered_map<Expr, CalcResult> data_;
};


