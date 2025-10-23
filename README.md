## 三方库
在demos_v1中, 每个项目作为一个基本单位, 都会通过git进行管理, 因此, github就是唯一合适的远程代码托管平台.  
三方库作为一个项目, 自然也受到了此约定的束缚, 但是和一般个人项目不同, 三方库要供给demos_v1下面的多个子项目使用,  
因此, 三方库会生成多个不同版本的库, 生成的库和头文件打包放在顶层目录的lib下面.  
构建命令, 以gtest生成静态/动态, debug/release组合共计4个版本的库为例子, 如下:  
(generator是ninja, 编译器为clang)  
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
git submodule update --init --recursive
```

## 构建工具链--通过CMakePresets.json管理
现在希望项目使用的工具链是clang++, Ninja, clangd, 并且构建目标要有多种配置(比如debug或者release)  
所以通过CMakePresets.json管理比较方便, 这里顺便提到cmake tools 这个插件, 它唯一的作用就是在写CMakeLists.txt  
和CMakePresets.json的时候, 提供一些语法支持. 其他方面要么用不到要么甚至是负面干扰.  
所以希望去掉这个插件的负载. 目前使用下面这种办法

在用户或工作区设置里把 
1. cmake.configureOnOpen
2. cmake.configureBeforeBuild
3. cmake.buildBeforeRun
4. cmake.statusbar.visibility 
5. cmake.configure on edit

等全部关掉，只保留语法高亮；它仍提供语言补全，但不会触发自动配置/扫描

### clangd怎么找到合适的compile_commands.json
现在因为通过CMakePresets.json搞出来了各种配置的构建, 所以会生成多份 compile_commands.json, 并且不是位于build层级目录里面,  
所以clangd插件默认可能会找不到, 要在.clangd里面指定比那一数据库所在的目录:
``` yaml
CompileFlags:
  CompilationDatabase: build/debug
```
目前的解决方案是搞一个compile.sh脚本, 这个脚本会在build层级目录下面创建一个指向当前活跃配置的compile_commands.json的软链接.

### 编译脚本 compile.sh脚本
主要用于封装编译命令, 让人手动执行的部分更少, 同时编译提示, 报错和输入信息量反而更多.  
这个脚本和CMakePresets.json都在

### 单个源文件一键编译配置
在项目根目录下面的.vscode里面的tasks.json里面进行了相关配置, 注意需要依赖CodeLLDB插件 

## 一些有趣的现象
现在我的learn_cxx_by_projects项目来源是`南山烟雨珠江潮`的项目, 但是我并没有权限给它的仓库推送代码,  
所以, learn_cxx_by_projects 推送到了我的github远程仓库上面, 那么日后如果要更新上游修改应该怎么做?  
```
cd learn_cxx_by_projects
git remote add upstream <原开源项目地址>  # 只需一次
git fetch upstream
git checkout main                       # 或你正在同步的分支
git merge upstream/main                 # 或 git rebase upstream/main
git push origin main
```


