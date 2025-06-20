@echo off
echo FastFaceSDK 构建脚本
echo ===================

REM 检查CMake是否安装
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 未找到CMake，请先安装CMake
    pause
    exit /b 1
)

REM 检查Visual Studio是否安装
where cl >nul 2>nul
if %errorlevel% neq 0 (
    echo 警告: 未找到Visual Studio编译器，尝试使用默认生成器
    set GENERATOR="Visual Studio 16 2019"
) else (
    echo 找到Visual Studio编译器
    set GENERATOR="Visual Studio 16 2019"
)

REM 创建构建目录
if not exist build mkdir build
cd build

REM 配置项目
echo 正在配置项目...
cmake .. -G %GENERATOR% -A x64
if %errorlevel% neq 0 (
    echo 配置失败
    pause
    exit /b 1
)

REM 编译项目
echo 正在编译项目...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo 编译失败
    pause
    exit /b 1
)

echo.
echo 构建完成！
echo 生成的文件位置:
echo   - 动态库: build\lib\Release\fast_face_sdk.dll
echo   - 示例程序: build\bin\Release\example.exe
echo.
echo 运行示例程序:
echo   build\bin\Release\example.exe
echo.

pause 