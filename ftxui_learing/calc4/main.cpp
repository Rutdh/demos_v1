#include "calc4.h"

int main() {
  std::string line;
  std::cout << "Enter expression: ";
  while (std::getline(std::cin, line)) {
    try {
      Calc4 tmp;
      std::cout << tmp.evaluate(line) << std::endl;
    } catch (const std::exception& ex) {
      std::cout << "Error: " << ex.what() << std::endl;
    }
    std::cout << "Enter expression: ";
  }
}