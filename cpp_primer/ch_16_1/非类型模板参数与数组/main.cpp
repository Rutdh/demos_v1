#include <cstring>
#include <iostream>
#include <numeric>
#include <type_traits>

template<typename T, unsigned M, unsigned N>
bool compare(const T (&p1)[M], const T (&p2)[N]) {
  if constexpr (std::is_same_v<T, int>) {
    auto sum1 = std::accumulate(p1, p1 + M, 0);
    auto sum2 = std::accumulate(p2, p2 + N, 0);
    return sum1 == sum2;
  } else if constexpr (std::is_same_v<T, char>) {
    return !strcmp(p1, p2);
  } else {
    std::cout << "暂时不支持的类型" << std::endl;
  }
}

int main() {
  // compare(const T (&p1)[M], const T (&p2)[N])
  int arr1[] = {1, 2, 3, 4, 5};
  int arr2[] = {5, 4, 3, 2, 1};
  std::cout << compare<int>(arr1, arr2) << std::endl;
}