## 三方库
在demos_v1中, 每个项目作为一个基本单位, 都会通过git进行管理[^1], 因此, github就是唯一合适的远程代码托管平台.  
三方库作为一个项目, 自然也受到了此约定的束缚, 但是和其他项目不同, 三方库还要供给demos_v1下面的多个子项目使用,  
因此, 三方库会生成多个不同版本的库, 生成的库和头文件也要打包放在顶层目录的lib下面.  
构建命令, 以gtest生成静态/动态, debug/release组合共计4个版本的库为例子, 如下:  
(generator是ninja, 编译器为clang)  

对于公共版本来说, 没必要生成 `compile_commands.json`, 用ninja是为了利用其多线程编译特性. 
但是如果想阅读三方库的源码, 那么 `compile_commands.json` 就是必须的了,  
所以还是在cmake的构建阶段加上`-DCMAKE_EXPORT_COMPILE_COMMANDS=ON` 这条命令吧, build放在源码目录即可   
注意如果是研究源码使用的话, 尽量不install, 避免lib体积膨胀太快, 当然如果是个性化修改版本有必要的话, install也行

下面给出三方库的通用编译命令
``` cmake
# 静态 Debug
cmake -S thirdparty/ftxui -B build/ftxui-static-debug -G Ninja \
      -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_LIBDIR=. -DCMAKE_INSTALL_INCLUDEDIR=include \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/ftxui-static-debug
cmake --install build/ftxui-static-debug --prefix lib/static-debug/ftxui

# 静态 Release
cmake -S thirdparty/ftxui -B build/ftxui-static-release -G Ninja \
      -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_LIBDIR=. -DCMAKE_INSTALL_INCLUDEDIR=include \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/ftxui-static-release
cmake --install build/ftxui-static-release --prefix lib/static-release/ftxui

# 动态 Debug
cmake -S thirdparty/ftxui -B build/ftxui-shared-debug -G Ninja \
      -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_INSTALL_LIBDIR=. -DCMAKE_INSTALL_INCLUDEDIR=include \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/ftxui-shared-debug
cmake --install build/ftxui-shared-debug --prefix lib/shared-debug/ftxui

# 动态 Release
cmake -S thirdparty/ftxui -B build/ftxui-shared-release -G Ninja \
      -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_LIBDIR=. -DCMAKE_INSTALL_INCLUDEDIR=include \
      -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build/ftxui-shared-release
cmake --install build/ftxui-shared-release --prefix lib/shared-release/ftxui
```


[^1]: 存在部分项目比如cpp_primer,ftxui_learning其实没有遵循这个规定


### 作为一个项目研究源码的版本
而且这样会有很多

## git submodule
添加子模块:
```
git submodule add https://github.com/google/googletest.git thirdparty/googletest
```
同步命令:
```
git submodule update --init --recursive
```
注意上面的添加子模块命令里面, 目标路径要是不存在的, 不是空的而是不存在的, 并且git会在目标路径下面clone子模块的代码,  
但是不会带上子模块名字, 所以目标路径最后一层应该是子模块名字或者其他合适的名字.  
这点很容易踩坑, 所以记录一下怎么删除添加到错误路径的子模块的方法.
``` txt
普遍做法是把子模块从 .gitmodules、.git/config 和工作区里都清理掉，再提交一次变更即可。常见步骤如下（在仓库根目录）：

git submodule deinit -f path/to/submodule：让 Git 忘记这个子模块的配置。
rm -rf path/to/submodule：删除工作区里的子模块目录（只删你要移除的那一项）。
编辑 .gitmodules，删除对应 [submodule "…"] 块；如果项目根的 .git/config 中也有该块，一并删掉。
git add .gitmodules path/to/submodule（以及 .git/config 若被追踪）并提交：git commit -m "Remove mistaken submodule".
如需彻底清理 .git/modules/path/to/submodule 缓存目录（通常在主仓库的 .git/modules 下），可执行 rm -rf .git/modules/path/to/submodule。

之后重新用正确路径执行 git submodule add … new/path 即可。
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
~~主要用于封装编译命令, 让人手动执行的部分更少, 同时编译提示, 报错和输入信息量反而更多.  
这个脚本和CMakePresets.json都在顶层存在模板~~  
已经废弃, 可能在老项目里面会存在, 目前迁移到compile.py

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


