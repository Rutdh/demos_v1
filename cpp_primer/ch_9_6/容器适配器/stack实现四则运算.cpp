#include <iostream>
#include <stack>
#include <string>
#include <cctype>
#include <stdexcept>

int precedence(char op) {
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
}

double applyOp(double lhs, double rhs, char op) {
    switch (op) {
        case '+': return lhs + rhs;
        case '-': return lhs - rhs;
        case '*': return lhs * rhs;
        case '/':
            if (rhs == 0.0) throw std::runtime_error("divide by zero");
            return lhs / rhs;
        default:
            throw std::runtime_error("unknown operator");
    }
}

double evaluate(const std::string& expr) {
    std::stack<double> values;
    std::stack<char> ops;
    const std::size_t n = expr.size();

    for (std::size_t i = 0; i < n; ) {
        const char c = expr[i];

        if (std::isspace(static_cast<unsigned char>(c))) {
            ++i;
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(c)) || c == '.') {
            std::size_t parsedLen = 0;
            double val = std::stod(expr.substr(i), &parsedLen);
            values.push(val);
            i += parsedLen;
            continue;
        }

        if (c == '(') {
            ops.push(c);
            ++i;
            continue;
        }

        if (c == ')') {
            while (!ops.empty() && ops.top() != '(') {
                const double rhs = values.top(); values.pop();
                const double lhs = values.top(); values.pop();
                const char op = ops.top(); ops.pop();
                values.push(applyOp(lhs, rhs, op));
            }
            if (ops.empty()) throw std::runtime_error("unmatched parentheses");
            ops.pop(); // remove '('
            ++i;
            continue;
        }

        // operator
        while (!ops.empty() && precedence(ops.top()) >= precedence(c)) {
            const double rhs = values.top(); values.pop();
            const double lhs = values.top(); values.pop();
            const char op = ops.top(); ops.pop();
            values.push(applyOp(lhs, rhs, op));
        }
        ops.push(c);
        ++i;
    }

    while (!ops.empty()) {
        const double rhs = values.top(); values.pop();
        const double lhs = values.top(); values.pop();
        const char op = ops.top(); ops.pop();
        values.push(applyOp(lhs, rhs, op));
    }

    if (values.size() != 1) throw std::runtime_error("invalid expression");
    return values.top();
}

int main() {
    std::string expr;
    std::getline(std::cin, expr);
    try {
        std::cout << evaluate(expr) << '\n';
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << '\n';
    }
}
