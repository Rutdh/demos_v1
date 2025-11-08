#include <cctype>
#include <iostream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>

int apply(int lhs, int rhs, char op) {
  switch (op) {
    case '+': return lhs + rhs;
    case '-': return lhs - rhs;
    case '*': return lhs * rhs;
    case '/':
      if (rhs == 0) throw std::runtime_error("division by zero");
      return lhs / rhs;
    default: throw std::runtime_error("unknown operator");
  }
}

int precedence(char op) {
  switch (op) {
    case '+':
    case '-': return 1;
    case '*':
    case '/': return 2;
    default: return 0;
  }
}

int evaluate(const std::string& expr) {
  std::stack<int> value_stack;
  std::stack<char> op_stack;

  auto reduce = [&]() {
    if (value_stack.size() < 2 || op_stack.empty())
      throw std::runtime_error("invalid expression");
    int rhs = value_stack.top();
    value_stack.pop();
    int lhs = value_stack.top();
    value_stack.pop();
    char op = op_stack.top();
    op_stack.pop();
    value_stack.push(apply(lhs, rhs, op));
  };

  for (std::size_t i = 0; i < expr.size(); ++i) {
    char ch = expr[i];
    if (std::isspace(static_cast<unsigned char>(ch))) continue;

    if (std::isdigit(static_cast<unsigned char>(ch))) {
      int value = 0;
      while (i < expr.size() && std::isdigit(static_cast<unsigned char>(expr[i]))) {
        value = value * 10 + (expr[i] - '0');
        ++i;
      }
      value_stack.push(value);
      --i;  // adjust because for loop will increment
    } else if (ch == '(') {
      op_stack.push(ch);
    } else if (ch == ')') {
      while (!op_stack.empty() && op_stack.top() != '(') {
        reduce();
      }
      if (op_stack.empty() || op_stack.top() != '(')
        throw std::runtime_error("mismatched parentheses");
      op_stack.pop();  // pop '('
    } else {
      while (!op_stack.empty() && precedence(op_stack.top()) >= precedence(ch)) {
        if (op_stack.top() == '(') break;
        reduce();
      }
      op_stack.push(ch);
    }
  }

  while (!op_stack.empty()) {
    if (op_stack.top() == '(') throw std::runtime_error("mismatched parentheses");
    reduce();
  }
  if (value_stack.size() != 1) throw std::runtime_error("invalid expression");
  return value_stack.top();
}

int main() {
  std::string line;
  std::cout << "Enter expression: ";
  while (std::getline(std::cin, line)) {
    try {
      std::cout << evaluate(line) << std::endl;
    } catch (const std::exception& ex) {
      std::cout << "Error: " << ex.what() << std::endl;
    }
    std::cout << "Enter expression: ";
  }
}