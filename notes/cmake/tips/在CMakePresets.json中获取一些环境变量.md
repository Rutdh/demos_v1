在cmake3.25.0之后支持在CMakePresets.json中利用宏展开来获取环境变量
---
可以，CMakePresets 里支持“宏”（macro）展开，环境变量可以用 $env{VAR} 引用，所以可以借此拿到当前用户的 home 目录。例如：
```
{
  "version": 6,
  "configurePresets": [
    {
      "name": "default",
      "binaryDir": "${sourceDir}/build",
      "cacheVariables": {
        "MY_HOME": "$env{HOME}"
      }
    }
  ]
}
```
上面 MY_HOME 会在配置时被替换成当前用户的 HOME（比如 /home/ganjun）。如果你只想在命令里用，可以直接写 "$env{HOME}/something"。在 Windows 上要注意 HOME 不一定存在，可改用 $env{USERPROFILE}。