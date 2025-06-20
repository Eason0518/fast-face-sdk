#!/bin/bash

echo "FastFaceSDK Unix 构建脚本"
echo "================================"

# 检查CMake是否安装
if ! command -v cmake &> /dev/null; then
    echo "错误: 未找到CMake，请先安装CMake"
    exit 1
fi

# 检查编译器
if command -v g++ &> /dev/null; then
    COMPILER="g++"
elif command -v clang++ &> /dev/null; then
    COMPILER="clang++"
else
    echo "错误: 未找到C++编译器"
    exit 1
fi

echo "使用编译器: $COMPILER"

# 创建构建目录
mkdir -p build
cd build

# 配置项目
echo "配置项目..."
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "错误: CMake配置失败"
    exit 1
fi

# 编译项目
echo "编译项目..."
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "错误: 编译失败"
    exit 1
fi

echo ""
echo "构建完成！"
echo "================================"
echo "生成的文件:"
echo "  - SDK库: build/lib/libfast_face_sdk.so"
echo "  - 示例程序: build/bin/example"
echo "  - 测试程序: build/bin/test_sdk"
echo "  - 许可证管理工具: build/bin/license_manager"
echo ""

# 运行测试
echo "运行测试程序..."
./bin/test_sdk
if [ $? -ne 0 ]; then
    echo "警告: 测试程序运行失败，请检查许可证配置"
fi

echo ""
echo "构建脚本执行完成！" 