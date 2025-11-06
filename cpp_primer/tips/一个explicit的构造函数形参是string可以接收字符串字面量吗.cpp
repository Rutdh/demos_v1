#include <string>
#include <utility>

// 4 种 explicit 构造函数形参演示：
// 1. std::string        -> 允许字面量直接初始化 (构造临时并按值传递)
// 2. const std::string& -> 允许字面量直接初始化 (临时绑定到 const 引用)
// 3. std::string&       -> 需要 std::string 左值，字面量无法绑定
// 4. const std::string  -> 顶层 const 会被丢弃，效果与按值版本相同

struct FromStringValue {
    explicit FromStringValue(std::string s) : payload(std::move(s)) {}
    std::string payload;
};

struct FromConstRef {
    explicit FromConstRef(const std::string& s) : payload(s) {}
    std::string payload;
};

struct FromStringRef {
    explicit FromStringRef(std::string& s) : payload(s) {}
    std::string payload;
};

struct FromConstValue {
    explicit FromConstValue(const std::string s) : payload(s) {}
    std::string payload;
};

int main() {
    // direct-initialization：显式调用构造函数，都可以通过
    FromStringValue a("literal");
    FromConstRef b("literal");
    FromConstValue c("literal");

    std::string s = "literal";
    FromStringRef d(s);  // 必须使用 std::string 左值

    (void)a;
    (void)b;
    (void)c;
    (void)d;

    // 以下语句如果取消注释，会触发编译错误，验证 “为什么”的部分：
    // FromStringRef e("literal");   // ERROR: 临时无法绑定到非常量引用
    // FromStringValue f = "literal"; // ERROR: explicit 禁止拷贝初始化
    // FromConstRef g = "literal";    // ERROR: explicit 禁止拷贝初始化
    // FromConstValue h = "literal";  // ERROR: explicit 禁止拷贝初始化
}
