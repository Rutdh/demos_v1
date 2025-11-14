#pragma once

#include "CalcController.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <string>
#include <vector>


class CalcView {
public:
  CalcView();
private:
  Component CreateButtonByLabel(const std::string& label);
  CalculatorController controller_;
  std::vector<std::vector<std::string>> rows_;
  std::vector<Component> horizontal_rows_;
};