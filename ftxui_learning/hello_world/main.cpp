#include "ftxui/screen/color.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

void PrintX() {
    auto screen = ftxui::Screen::Create(
    ftxui::Dimension::Full(),
    ftxui::Dimension::Fixed(10)
  );

  auto &pixel = screen.PixelAt(10, 5);

  pixel.character = U'X';
  pixel.foreground_color = ftxui::Color::Red;
  pixel.background_color = ftxui::Color::RGB(0, 255, 0);
  pixel.bold = true;
  screen.Print();

  std::cout << "test ftxui::Screen::ToString(): " 
    << screen.ToString() << std::endl;
}

void PrintCurTerminalDefaultColor() {
  auto var = ftxui::Color::Default;
  // std::cout << "current terminal default color is: "
  //   << var << std::endl;
  
}

int main() {
  using namespace ftxui;
 
  Element document = hbox({
    text("left")   | border,
    text("middle") | border | flex,
    text("right")  | border,
  });
 
  auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
  Render(screen, document);
  screen.Print();
  std::cout << "test ftxui::Screen::ToString(): " 
    << screen.ToString() << std::endl;

  screen.Clear();

  PrintX();

  screen.Clear();

  PrintCurTerminalDefaultColor();
}