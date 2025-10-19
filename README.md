## 三方库
在demos_v1中, 每个项目作为一个基本单位, 都会通过git进行管理, 因此, github就是唯一合适的远程代码托管平台.  
三方库作为一个项目, 自然也受到了此约定的束缚, 但是和一般个人项目不同, 三方库要供给demos_v1下面的多个子项目使用,  
因此, 三方库会生成多个不同版本的库, 生成的库和头文件打包放在顶层目录的lib下面.  
构建命令, 以gtest生成静态/动态, debug/release组合共计4个版本的库为例子, 如下:  
``` cmake
# 静态 Debug
cmake -S thirdparty/googletest -B build/gtest-static-debug -G Ninja \
      -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_LIBDIR=. -DCMAKE_INSTALL_INCLUDEDIR=include \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build/gtest-static-debug
cmake --install build/gtest-static-debug --prefix lib/gtest/static-debug

# 静态 Release
cmake -S thirdparty/googletest -B build/gtest-static-release -G Ninja \
      -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_LIBDIR=. -DCMAKE_INSTALL_INCLUDEDIR=include \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build/gtest-static-release
cmake --install build/gtest-static-release --prefix lib/gtest/static-release

# 动态 Debug
cmake -S thirdparty/googletest -B build/gtest-shared-debug -G Ninja \
      -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_LIBDIR=. -DCMAKE_INSTALL_INCLUDEDIR=include \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build/gtest-shared-debug
cmake --install build/gtest-shared-debug --prefix lib/gtest/shared-debug

# 动态 Release
cmake -S thirdparty/googletest -B build/gtest-shared-release -G Ninja \
      -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_LIBDIR=. -DCMAKE_INSTALL_INCLUDEDIR=include \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build/gtest-shared-release
cmake --install build/gtest-shared-release --prefix lib/gtest/shared-release

```

## git submodule
添加子模块:
```
git submodule add https://github.com/google/googletest.git thirdparty/googletest
```
同步命令:
```
git submodule init && git submodule update
```