#!/usr/bin/env bash

set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "用法: $0 <debug|release>"
  exit 1
fi

build_type="${1,,}"

case "${build_type}" in
  debug|release)
    preset="${build_type}"
    ;;
  *)
    echo "错误: 不支持的编译类型 '${1}'，仅支持 debug 或 release。"
    exit 1
    ;;
esac

echo "==> 配置 (${preset})"
cmake --preset "${preset}"

build_dir="build/${preset}"
compile_db="${build_dir}/compile_commands.json"
link_path="build/compile_commands.json"

mkdir -p "build"

if [[ -f "${compile_db}" ]]; then
  target="$(realpath "${compile_db}")"
  echo "==> 更新符号链接 ${link_path} -> ${target}"
  ln -sfn "${target}" "${link_path}"
else
  echo "警告: 未找到 ${compile_db}，跳过符号链接创建。"
fi

echo "==> 构建 (${preset})"
cmake --build --preset "${preset}"
