@echo off
echo 编译密钥查看工具...

REM 尝试使用Visual Studio编译器
where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo 使用Visual Studio编译器...
    cl /EHsc show_key.cpp /Fe:show_key.exe
    if %errorlevel% equ 0 (
        echo 编译成功！
        echo.
        echo 运行密钥查看工具...
        echo ================================
        show_key.exe
    ) else (
        echo 编译失败！
    )
) else (
    echo 未找到Visual Studio编译器，尝试使用g++...
    where g++ >nul 2>nul
    if %errorlevel% equ 0 (
        g++ -o show_key.exe show_key.cpp
        if %errorlevel% equ 0 (
            echo 编译成功！
            echo.
            echo 运行密钥查看工具...
            echo ================================
            show_key.exe
        ) else (
            echo 编译失败！
        )
    ) else (
        echo 错误: 未找到C++编译器
        echo 请安装Visual Studio或MinGW-w64
        echo.
        echo 手动查看密钥信息:
        echo 当前密钥: FAST_FACE_2024_LICENSE_KEY_12345
        echo 密钥长度: 32 字符
        echo 密钥类型: 商业许可证
        echo 过期时间: 2024年12月31日
    )
)

echo.
pause 