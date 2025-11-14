#include "CalcController.h"
#include <cstddef>

void CalculatorController::LoadHistoryEntry(size_t index) {
  const auto &history = model_.history();
  if (index >= history.size()) {
    return;
  }
  model_.SetExpression(history[index].expression);
  Notify();
}

