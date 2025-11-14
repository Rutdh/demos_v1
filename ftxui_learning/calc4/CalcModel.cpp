#include "CalcModel.h"

DEFINE_int32(max_history_entries, 20, "最大历史消息队列长度");

void CalculatorModel::Clear() {
  expression_.clear();
  result_ = "0";
  error_.clear();
}

void CalculatorModel::Backspace() {
  if (!expression_.empty()) {
    expression_.pop_back();
    error_.clear();
  }
}

void CalculatorModel::AppendDigit(char digit) {
  if (std::isdigit(digit)) {
    expression_.push_back(digit);
    error_.clear();
  }
}

void CalculatorModel::PushHistoryEntry(const std::string &expr,
                                       const std::string &result) {
  history_.push_back({expr, result});
  if (history_.size() > FLAGS_max_history_entries) {
    history_.erase(history_.begin());
  }
}

void CalculatorModel::AppendDot() {
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

void CalculatorModel::AppendOperator(char op) {
  if (expression_.empty()) {
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

void CalculatorModel::AppendLeftParen() {
  expression_.push_back('(');
  error_.clear();
}

void CalculatorModel::AppendRightParen() {
  if (expression_.empty()) {
    LOG(INFO) << "[AppendRightParen] expression is empty.";
    return;
  }

  if (auto unmatched = UnmatchedLeftParentheses(); unmatched <= 0) {
    LOG(INFO) << "[AppendRightParen] unmatched parentheses: " << unmatched;
    return;
  }

  if (expression_.back() == '(' || IsOperator(expression_.back())) {
    LOG(INFO) << "[AppendRightParen] last char is op or (: ";
    return;
  }

  expression_.push_back(')');
  error_.clear();
  return;
}

void CalculatorModel::Evaluate() {
  if (expression_.empty()) {
    LOG(INFO) << "[Evaluate] expr is empty";
    error_.clear();
    return;
  }
}

bool CalculatorModel::EvaluateExpression(const std::string &expr, double &out) {
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
      LOG(INFO) << "[apply] values size: " << values.size()
                << "ops size: " << ops.size();
      return false;
    }

    double b = values.back();
    values.pop_back();
    char op = ops.back();
    ops.pop_back();
    double a = values.back();
    values.pop_back();

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
      if (b == 0) {
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
      while (!ops.empty() || ops.back() != '(') {
        if (!apply(values, ops)) {
          LOG(INFO) << "[EvaluateExpression] apply failed.";
          return false;
        }
      }

      if (ops.empty()) {
        LOG(INFO) << "apply calc failed.";
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
      while (!ops.empty() && ops.back() != '(' &&
             precedence(op) <= precedence(ops.back())) {
        if (!apply(values, ops)) {
          LOG(INFO) << "[EvaluateExpression] calc failed.";
          return false;
        }
      }

      ops.push_back(op);
      LOG(INFO) << "op: " << op << " push back to ops.";
      ++i;
      continue;
    }

    LOG(INFO) << "Unsupported character: " << expr[i];
    return false;
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
