#include <iostream>
#include <queue>

int main(int argc, char *argv[]) {
  // 默认是最大堆, 可以通过自己传入std::less<T> 和 std::greater<T>来改变行为
  std::priority_queue<int> pri_que;
  pri_que.push(10);
  pri_que.push(3);
  pri_que.push(30);

  

  return 0;
}