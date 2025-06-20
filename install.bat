@echo off
echo FastFaceSDK 自动安装脚本
echo ========================
echo.

REM 检查是否以管理员权限运行
net session >nul 2>&1
if %errorLevel% == 0 (
    echo ✓ 以管理员权限运行
) else (
    echo ✗ 请以管理员权限运行此脚本
    pause
    exit /b 1
)

echo.
echo 正在检查系统环境...

REM 检查CMake
where cmake >nul 2>&1
if %errorLevel% == 0 (
    echo ✓ 找到CMake
    cmake --version
) else (
    echo ✗ 未找到CMake，正在安装...
    echo 请手动安装CMake: https://cmake.org/download/
    echo 或使用: choco install cmake
    pause
    exit /b 1
)

REM 检查Visual Studio
where cl >nul 2>&1
if %errorLevel% == 0 (
    echo ✓ 找到Visual Studio编译器
) else (
    echo ✗ 未找到Visual Studio编译器
    echo 请安装Visual Studio 2019/2022 Community
    echo 确保包含C++开发工具
    pause
    exit /b 1
)

REM 检查OpenCV
echo.
echo 检查OpenCV安装...

REM 尝试查找OpenCV
set OPENCV_FOUND=0

REM 检查vcpkg安装的OpenCV
if exist "C:\vcpkg\installed\x64-windows\include\opencv2" (
    echo ✓ 找到vcpkg安装的OpenCV
    set OPENCV_FOUND=1
    set OPENCV_DIR=C:\vcpkg\installed\x64-windows
)

REM 检查系统安装的OpenCV
if exist "C:\opencv\build\include\opencv2" (
    echo ✓ 找到系统安装的OpenCV
    set OPENCV_FOUND=1
    set OPENCV_DIR=C:\opencv\build
)

if %OPENCV_FOUND% == 0 (
    echo ✗ 未找到OpenCV，正在安装...
    echo.
    echo 推荐使用vcpkg安装OpenCV:
    echo 1. git clone https://github.com/Microsoft/vcpkg.git
    echo 2. cd vcpkg
    echo 3. .\bootstrap-vcpkg.bat
    echo 4. .\vcpkg install opencv4[contrib]:x64-windows
    echo 5. .\vcpkg integrate install
    echo.
    echo 安装完成后重新运行此脚本
    pause
    exit /b 1
)

echo.
echo 环境检查完成！
echo.
echo 正在编译SDK...

REM 运行构建脚本
call build.bat

if %errorLevel% == 0 (
    echo.
    echo ✓ SDK编译成功！
    echo.
    echo 生成的文件:
    echo   - 动态库: build\lib\Release\fast_face_sdk.dll
    echo   - 示例程序: build\bin\Release\example.exe
    echo   - 测试程序: build\bin\Release\test_sdk.exe
    echo.
    echo 运行示例程序:
    echo   build\bin\Release\example.exe
    echo.
    echo 运行测试程序:
    echo   build\bin\Release\test_sdk.exe
    echo.
    echo 安装完成！
) else (
    echo.
    echo ✗ SDK编译失败，请检查错误信息
)

pause 