#include <iostream>
#include <thread>

class Test {
public:
  // 非显式构造函数
  Test(int a) : a_(a) {
    std::cout << "调用了构造函数" << std::endl;
  }
  

// private:
  // 如果没记错的话, 因为 "五三法则", 这里其他拷贝控制成员也成private了
  ~Test() = default;

  Test(const Test& t) {
    std::cout << "拷贝构造函数" << std::endl;
    a_ = t.a_;
  }

  Test& operator=(const Test& t) {
    std::cout << "拷贝赋值运算符函数" << std::endl;
    a_ = t.a_;

    return *this;
  }

  int a_;
};

int main() {
  // 下面这行代码因为std::thread 的构造函数是explict 的, 所以当右边尝试转换成thread对象然后进行初始化的时候, 就违反语法了
  // 但是即使不是显示构造, 也不会通过, 因为 thread没有拷贝构造和拷贝赋值运算符
  // 如果是C++17, 因为强制拷贝消除特性, 当用同类型的纯右值初始化对象时，直接在目标位置构造，完全跳过拷贝/移动构造函数
  // std::thread t1 = [] {
  //   std::cout << "hello, world." << std::endl;
  // };

  // 如果Test的定义里面的private放开注释这里就会失败
  // 在Test的析构函数是public的情况下, 1是一个纯右值, 而且可以被视为Test(1)这样的一个语义上面的纯右值
  // 所以因为C++17的强制拷贝消除, 直接调用了构造
  Test test = 1;


}