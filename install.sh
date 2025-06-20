#!/bin/bash

echo "FastFaceSDK 自动安装脚本"
echo "========================"
echo

# 检查是否为root用户
if [[ $EUID -eq 0 ]]; then
   echo "✓ 以root权限运行"
else
   echo "✗ 请以root权限运行此脚本"
   echo "使用: sudo ./install.sh"
   exit 1
fi

echo
echo "正在检查系统环境..."

# 检测操作系统
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    echo "✓ 检测到Linux系统"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    echo "✓ 检测到macOS系统"
else
    echo "✗ 不支持的操作系统: $OSTYPE"
    exit 1
fi

# 检查CMake
if command -v cmake &> /dev/null; then
    echo "✓ 找到CMake"
    cmake --version
else
    echo "✗ 未找到CMake，正在安装..."
    if [[ "$OS" == "linux" ]]; then
        # 检测Linux发行版
        if command -v apt-get &> /dev/null; then
            apt-get update
            apt-get install -y cmake build-essential
        elif command -v yum &> /dev/null; then
            yum install -y cmake gcc gcc-c++ make
        elif command -v dnf &> /dev/null; then
            dnf install -y cmake gcc gcc-c++ make
        else
            echo "无法自动安装CMake，请手动安装"
            exit 1
        fi
    elif [[ "$OS" == "macos" ]]; then
        if command -v brew &> /dev/null; then
            brew install cmake
        else
            echo "请先安装Homebrew: https://brew.sh/"
            exit 1
        fi
    fi
fi

# 检查编译器
if command -v g++ &> /dev/null; then
    echo "✓ 找到GCC编译器"
    export CC=gcc
    export CXX=g++
elif command -v clang++ &> /dev/null; then
    echo "✓ 找到Clang编译器"
    export CC=clang
    export CXX=clang++
else
    echo "✗ 未找到C++编译器"
    if [[ "$OS" == "linux" ]]; then
        apt-get install -y build-essential
    elif [[ "$OS" == "macos" ]]; then
        xcode-select --install
    fi
fi

# 检查OpenCV
echo
echo "检查OpenCV安装..."

OPENCV_FOUND=0

# 检查pkg-config
if command -v pkg-config &> /dev/null; then
    if pkg-config --exists opencv4; then
        echo "✓ 找到OpenCV 4.x"
        OPENCV_FOUND=1
    elif pkg-config --exists opencv; then
        echo "✓ 找到OpenCV"
        OPENCV_FOUND=1
    fi
fi

# 检查常见安装路径
if [[ -d "/usr/include/opencv4" ]] || [[ -d "/usr/local/include/opencv4" ]]; then
    echo "✓ 找到OpenCV头文件"
    OPENCV_FOUND=1
fi

if [[ $OPENCV_FOUND -eq 0 ]]; then
    echo "✗ 未找到OpenCV，正在安装..."
    if [[ "$OS" == "linux" ]]; then
        if command -v apt-get &> /dev/null; then
            apt-get update
            apt-get install -y libopencv-dev libopencv-contrib-dev
        elif command -v yum &> /dev/null; then
            yum install -y opencv-devel opencv-contrib-devel
        elif command -v dnf &> /dev/null; then
            dnf install -y opencv-devel opencv-contrib-devel
        else
            echo "无法自动安装OpenCV，请手动安装"
            echo "Ubuntu/Debian: sudo apt install libopencv-dev libopencv-contrib-dev"
            echo "CentOS/RHEL: sudo yum install opencv-devel opencv-contrib-devel"
            exit 1
        fi
    elif [[ "$OS" == "macos" ]]; then
        if command -v brew &> /dev/null; then
            brew install opencv
        else
            echo "请先安装Homebrew: https://brew.sh/"
            exit 1
        fi
    fi
fi

echo
echo "环境检查完成！"
echo
echo "正在编译SDK..."

# 运行构建脚本
chmod +x build.sh
./build.sh

if [[ $? -eq 0 ]]; then
    echo
    echo "✓ SDK编译成功！"
    echo
    echo "生成的文件:"
    echo "  - 动态库: build/lib/libfast_face_sdk.so"
    echo "  - 示例程序: build/bin/example"
    echo "  - 测试程序: build/bin/test_sdk"
    echo
    echo "运行示例程序:"
    echo "  ./build/bin/example"
    echo
    echo "运行测试程序:"
    echo "  ./build/bin/test_sdk"
    echo
    echo "安装完成！"
else
    echo
    echo "✗ SDK编译失败，请检查错误信息"
    exit 1
fi 