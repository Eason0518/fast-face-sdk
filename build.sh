#!/bin/bash

echo "FastFaceSDK 构建脚本"
echo "==================="

# 检查CMake是否安装
if ! command -v cmake &> /dev/null; then
    echo "错误: 未找到CMake，请先安装CMake"
    exit 1
fi

# 检查编译器
if command -v g++ &> /dev/null; then
    echo "找到GCC编译器"
    export CC=gcc
    export CXX=g++
elif command -v clang++ &> /dev/null; then
    echo "找到Clang编译器"
    export CC=clang
    export CXX=clang++
else
    echo "错误: 未找到C++编译器"
    exit 1
fi

# 创建构建目录
mkdir -p build
cd build

# 配置项目
echo "正在配置项目..."
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "配置失败"
    exit 1
fi

# 编译项目
echo "正在编译项目..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
if [ $? -ne 0 ]; then
    echo "编译失败"
    exit 1
fi

echo ""
echo "构建完成！"
echo "生成的文件位置:"
echo "  - 动态库: build/lib/libfast_face_sdk.so"
echo "  - 示例程序: build/bin/example"
echo ""
echo "运行示例程序:"
echo "  ./build/bin/example"
echo "" 