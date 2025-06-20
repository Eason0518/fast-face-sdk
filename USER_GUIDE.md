# 📖 FastFaceSDK 用户指南

本指南详细说明如何使用FastFaceSDK进行人脸分析。

## 🔐 许可证密钥系统

### 重要提示
**使用FastFaceSDK必须提供有效的许可证密钥！**

### 获取许可证密钥
1. **商业许可证**: 联系开发者获取
2. **试用许可证**: 使用 `TRIAL_KEY`（需要启用试用模式）
3. **默认密钥**: `FAST_FACE_2024_LICENSE_KEY_12345`

### 密钥验证流程
```cpp
// 1. 验证许可证
int verify_result = verify_license("YOUR_LICENSE_KEY");
if (verify_result != 0) {
    printf("许可证验证失败，错误代码: %d\n", verify_result);
    return -1;
}

// 2. 初始化SDK
int init_result = sdk_init("YOUR_LICENSE_KEY");
if (init_result != 0) {
    printf("SDK初始化失败，错误代码: %d\n", init_result);
    return -1;
}
```

## 🚀 快速开始

### 1. 环境准备

#### 系统要求
- **操作系统**: Windows 10+, Linux, macOS
- **编译器**: 
  - Windows: Visual Studio 2019+ 或 MinGW-w64
  - Linux/macOS: GCC 7+ 或 Clang 10+
- **依赖库**:
  - OpenCV 4.5+
  - nlohmann/json 3.9+

#### 安装依赖

**Windows (使用vcpkg)**
```bash
# 安装vcpkg
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.bat

# 安装OpenCV
./vcpkg install opencv4[contrib]:x64-windows

# 集成到Visual Studio
./vcpkg integrate install
```

**Linux (Ubuntu/Debian)**
```bash
sudo apt update
sudo apt install libopencv-dev libopencv-contrib-dev cmake build-essential
```

**macOS**
```bash
brew install opencv cmake
```

### 2. 编译SDK

#### Windows
```bash
# 使用提供的批处理脚本
build_windows.bat

# 或手动编译
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

#### Linux/macOS
```bash
# 使用提供的shell脚本
chmod +x build_unix.sh
./build_unix.sh

# 或手动编译
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 3. 验证许可证密钥

在开始使用SDK之前，请先验证您的许可证密钥：

```bash
# 运行密钥查看工具
.\show_key.bat

# 或运行许可证测试
.\build\bin\test_license.exe
```

### 4. 运行示例程序

```bash
# 运行示例程序（需要摄像头）
.\build\bin\example.exe

# 运行测试程序
.\build\bin\test_sdk.exe
```

## 💻 基本使用

### 完整示例代码

```cpp
#include "fast_face_sdk.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <nlohmann/json.hpp>

int main() {
    // 🔑 第一步：验证许可证密钥
    const char* license_key = "FAST_FACE_2024_LICENSE_KEY_12345"; // 替换为您的密钥
    
    int verify_result = verify_license(license_key);
    if (verify_result != 0) {
        std::cout << "❌ 许可证验证失败，错误代码: " << verify_result << std::endl;
        std::cout << "请检查您的许可证密钥是否正确" << std::endl;
        return -1;
    }
    std::cout << "✅ 许可证验证成功" << std::endl;
    
    // 🔧 第二步：初始化SDK
    int init_result = sdk_init(license_key);
    if (init_result != 0) {
        std::cout << "❌ SDK初始化失败，错误代码: " << init_result << std::endl;
        return -1;
    }
    std::cout << "✅ SDK初始化成功" << std::endl;
    
    // 📊 第三步：获取许可证信息
    char license_info[1024];
    int info_result = get_license_info(license_info, sizeof(license_info));
    if (info_result == 0) {
        try {
            nlohmann::json info = nlohmann::json::parse(license_info);
            std::cout << "📋 许可证信息:" << std::endl;
            std::cout << "  类型: " << info["type"] << std::endl;
            std::cout << "  状态: " << info["status"] << std::endl;
            std::cout << "  过期时间: " << info["expires"] << std::endl;
        } catch (const std::exception& e) {
            std::cout << "许可证信息解析失败: " << e.what() << std::endl;
        }
    }
    
    // 🖼️ 第四步：读取图像
    cv::Mat image = cv::imread("test.jpg");
    if (image.empty()) {
        std::cout << "❌ 无法读取图像文件" << std::endl;
        sdk_release();
        return -1;
    }
    std::cout << "✅ 图像读取成功: " << image.cols << "x" << image.rows << std::endl;
    
    // 🔍 第五步：分析图像
    char result_json[4096];
    int analysis_result = analyze_frame(
        image.data, 
        image.cols, 
        image.rows, 
        result_json, 
        sizeof(result_json)
    );
    
    // 📊 第六步：处理分析结果
    if (analysis_result == 0) {
        std::cout << "✅ 图像分析成功" << std::endl;
        
        try {
            nlohmann::json result = nlohmann::json::parse(result_json);
            
            // 显示许可证信息
            if (result.contains("license_info")) {
                const auto& license = result["license_info"];
                std::cout << "🔐 许可证状态: " << license["status"] << std::endl;
            }
            
            // 显示人脸检测结果
            const auto& faces = result["faces"];
            std::cout << "👥 检测到 " << faces.size() << " 个人脸" << std::endl;
            
            for (size_t i = 0; i < faces.size(); ++i) {
                const auto& face = faces[i];
                std::cout << "\n人脸 " << (i + 1) << ":" << std::endl;
                
                // 边界框信息
                const auto& bbox = face["bbox"];
                std::cout << "  位置: (" << bbox["x"] << ", " << bbox["y"] << ")" << std::endl;
                std::cout << "  大小: " << bbox["width"] << "x" << bbox["height"] << std::endl;
                
                // 头部姿态
                const auto& pose = face["pose"];
                std::cout << "  头部姿态: Yaw=" << pose["yaw"] << "°, Pitch=" << pose["pitch"] << "°, Roll=" << pose["roll"] << "°" << std::endl;
                
                // 质量评分
                const auto& scores = face["quality_scores"];
                std::cout << "  清晰度评分: " << scores["sharpness_score"] << "/100" << std::endl;
                std::cout << "  亮度评分: " << scores["brightness_score"] << "/100" << std::endl;
                std::cout << "  对比度评分: " << scores["contrast_score"] << "/100" << std::endl;
                
                // 其他信息
                const auto& metrics = face["metrics"];
                std::cout << "  戴口罩: " << (metrics["has_mask"] ? "是" : "否") << std::endl;
                std::cout << "  距离: " << metrics["distance"] << "cm" << std::endl;
            }
            
        } catch (const std::exception& e) {
            std::cout << "❌ JSON解析失败: " << e.what() << std::endl;
        }
    } else {
        std::cout << "❌ 图像分析失败，错误代码: " << analysis_result << std::endl;
        
        // 处理许可证相关错误
        switch (analysis_result) {
            case -1:
                std::cout << "许可证密钥无效" << std::endl;
                break;
            case -2:
                std::cout << "许可证密钥已过期" << std::endl;
                break;
            case -3:
                std::cout << "试用期已过期" << std::endl;
                break;
            default:
                std::cout << "其他错误" << std::endl;
                break;
        }
    }
    
    // 🧹 第七步：释放资源
    sdk_release();
    std::cout << "✅ 资源释放完成" << std::endl;
    
    return 0;
}
```

### 编译示例

```bash
# Windows (Visual Studio)
cl /EHsc example.cpp /I./include /link /LIBPATH:./build/lib fast_face_sdk.lib opencv_world480.lib

# Windows (MinGW)
g++ -o example.exe example.cpp -I./include -L./build/lib -lfast_face_sdk -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_face -lopencv_video -lopencv_imgcodecs -lopencv_highgui

# Linux/macOS
g++ -o example example.cpp -I./include -L./build/lib -lfast_face_sdk -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_face -lopencv_video -lopencv_imgcodecs -lopencv_highgui
```

## 🔑 许可证密钥管理

### 查看当前密钥

```bash
# 运行密钥查看工具
.\show_key.bat

# 输出示例:
# 当前密钥: FAST_FACE_2024_LICENSE_KEY_12345
# 密钥长度: 32 字符
# 密钥类型: 商业许可证
# 过期时间: 2024年12月31日
```

### 修改许可证密钥

编辑 `include/fast_face_config.h` 文件：

```cpp
// 修改第12行的密钥
constexpr const char* VALID_LICENSE_KEY = "YOUR_NEW_LICENSE_KEY";
```

### 生成新许可证

使用许可证管理工具：

```bash
# 启动许可证管理工具
.\build\bin\license_manager.exe

# 生成商业许可证
license> generate commercial

# 生成试用许可证
license> generate trial 5

# 生成临时许可证
license> generate temporary 10 30
```

### 验证许可证

```cpp
// 验证许可证格式
int result = verify_license("YOUR_LICENSE_KEY");
if (result == 0) {
    printf("许可证有效\n");
} else {
    printf("许可证无效，错误代码: %d\n", result);
}
```

## 📊 API 详细说明

### 核心函数

#### `verify_license(const char* license_key)`
验证许可证密钥是否有效。

**参数:**
- `license_key`: 许可证密钥字符串

**返回值:**
- `0`: 密钥有效
- `-1`: 密钥格式错误
- `-2`: 密钥已过期
- `-3`: 试用期已过期

**使用示例:**
```cpp
const char* key = "FAST_FACE_2024_LICENSE_KEY_12345";
int result = verify_license(key);
if (result == 0) {
    printf("许可证验证成功\n");
} else {
    printf("许可证验证失败: %d\n", result);
}
```

#### `sdk_init(const char* license_key)`
使用许可证密钥初始化SDK。

**参数:**
- `license_key`: 许可证密钥字符串

**返回值:**
- `0`: 初始化成功
- `-1`: 密钥无效
- `-2`: 密钥已过期
- `-3`: 试用期已过期
- `-4`: 人脸检测模型加载失败
- `-5`: 眼睛检测模型加载失败
- `-6`: 初始化异常

**使用示例:**
```cpp
int result = sdk_init("FAST_FACE_2024_LICENSE_KEY_12345");
if (result != 0) {
    printf("SDK初始化失败: %d\n", result);
    return -1;
}
```

#### `get_license_info(char* license_info, int info_buf_len)`
获取当前许可证信息。

**参数:**
- `license_info`: 输出许可证信息的缓冲区
- `info_buf_len`: 缓冲区长度

**返回值:**
- `0`: 获取成功
- `-8`: 参数错误
- `-9`: 缓冲区太小

**使用示例:**
```cpp
char info[1024];
int result = get_license_info(info, sizeof(info));
if (result == 0) {
    printf("许可证信息: %s\n", info);
}
```

#### `analyze_frame(const unsigned char* bgr_data, int width, int height, char* result_json, int json_buf_len)`
分析一帧BGR格式的图像。

**参数:**
- `bgr_data`: BGR格式的图像数据
- `width`: 图像宽度
- `height`: 图像高度
- `result_json`: 输出JSON结果的缓冲区
- `json_buf_len`: 缓冲区长度

**返回值:**
- `0`: 分析成功
- `-100`: SDK未初始化
- `-8`: 参数错误
- `-9`: 缓冲区太小
- `-7`: 分析过程异常

**使用示例:**
```cpp
cv::Mat image = cv::imread("test.jpg");
char result[4096];
int analysis_result = analyze_frame(
    image.data, 
    image.cols, 
    image.rows, 
    result, 
    sizeof(result)
);

if (analysis_result == 0) {
    printf("分析结果: %s\n", result);
} else {
    printf("分析失败: %d\n", analysis_result);
}
```

#### `sdk_release()`
释放SDK资源。

**使用示例:**
```cpp
// 使用完毕后释放资源
sdk_release();
```

## 🔍 错误处理

### 常见错误代码

| 错误代码 | 含义 | 解决方案 |
|---------|------|---------|
| 0 | 成功 | - |
| -1 | 密钥无效 | 检查密钥格式和内容 |
| -2 | 密钥已过期 | 更新许可证密钥 |
| -3 | 试用期已过期 | 申请新的试用许可证或购买商业许可证 |
| -4 | 人脸检测模型加载失败 | 检查OpenCV安装 |
| -5 | 眼睛检测模型加载失败 | 检查OpenCV安装 |
| -6 | 初始化异常 | 检查系统环境 |
| -7 | 分析过程异常 | 检查输入图像 |
| -8 | 参数错误 | 检查函数参数 |
| -9 | 缓冲区太小 | 增加缓冲区大小 |
| -100 | SDK未初始化 | 先调用sdk_init() |

### 错误处理示例

```cpp
int handle_error(int error_code, const char* operation) {
    printf("操作 '%s' 失败，错误代码: %d\n", operation, error_code);
    
    switch (error_code) {
        case -1:
            printf("许可证密钥无效，请检查密钥\n");
            break;
        case -2:
            printf("许可证已过期，请联系开发者更新\n");
            break;
        case -3:
            printf("试用期已过期，请申请新的试用许可证\n");
            break;
        case -4:
        case -5:
            printf("模型加载失败，请检查OpenCV安装\n");
            break;
        case -6:
            printf("初始化异常，请检查系统环境\n");
            break;
        case -7:
            printf("分析过程异常，请检查输入图像\n");
            break;
        case -8:
            printf("参数错误，请检查函数参数\n");
            break;
        case -9:
            printf("缓冲区太小，请增加缓冲区大小\n");
            break;
        case -100:
            printf("SDK未初始化，请先调用sdk_init()\n");
            break;
        default:
            printf("未知错误\n");
            break;
    }
    
    return error_code;
}
```

## 📊 结果解析

### JSON结果格式

```json
{
  "code": 0,
  "msg": "success",
  "license_info": {
    "status": "valid",
    "expires": "2024-12-31",
    "type": "commercial",
    "trial_remaining_days": 0
  },
  "faces": [
    {
      "bbox": {
        "x": 100,
        "y": 50,
        "width": 200,
        "height": 250
      },
      "pose": {
        "yaw": 5.2,
        "pitch": -2.1,
        "roll": 1.5
      },
      "metrics": {
        "sharpness": 45.6,
        "brightness": 128.3,
        "has_mask": false,
        "distance": 50.0
      },
      "quality_scores": {
        "sharpness_score": 85.2,
        "brightness_score": 92.1,
        "contrast_score": 78.5
      },
      "stability": {
        "is_stable": true,
        "motion_blur": 15.3
      }
    }
  ]
}
```

### 解析示例

```cpp
#include <nlohmann/json.hpp>

void parse_analysis_result(const char* json_str) {
    try {
        nlohmann::json result = nlohmann::json::parse(json_str);
        
        if (result["code"] != 0) {
            printf("分析失败: %s\n", result["msg"].get<std::string>().c_str());
            return;
        }
        
        // 显示许可证信息
        if (result.contains("license_info")) {
            const auto& license = result["license_info"];
            printf("许可证状态: %s\n", license["status"].get<std::string>().c_str());
            printf("许可证类型: %s\n", license["type"].get<std::string>().c_str());
            printf("过期时间: %s\n", license["expires"].get<std::string>().c_str());
        }
        
        // 处理人脸检测结果
        const auto& faces = result["faces"];
        printf("检测到 %zu 个人脸\n", faces.size());
        
        for (size_t i = 0; i < faces.size(); ++i) {
            const auto& face = faces[i];
            printf("\n人脸 %zu:\n", i + 1);
            
            // 边界框
            const auto& bbox = face["bbox"];
            printf("  位置: (%d, %d)\n", bbox["x"].get<int>(), bbox["y"].get<int>());
            printf("  大小: %dx%d\n", bbox["width"].get<int>(), bbox["height"].get<int>());
            
            // 头部姿态
            const auto& pose = face["pose"];
            printf("  头部姿态: Yaw=%.1f°, Pitch=%.1f°, Roll=%.1f°\n", 
                   pose["yaw"].get<double>(), 
                   pose["pitch"].get<double>(), 
                   pose["roll"].get<double>());
            
            // 质量评分
            const auto& scores = face["quality_scores"];
            printf("  清晰度评分: %.1f/100\n", scores["sharpness_score"].get<double>());
            printf("  亮度评分: %.1f/100\n", scores["brightness_score"].get<double>());
            printf("  对比度评分: %.1f/100\n", scores["contrast_score"].get<double>());
            
            // 其他信息
            const auto& metrics = face["metrics"];
            printf("  戴口罩: %s\n", metrics["has_mask"].get<bool>() ? "是" : "否");
            printf("  距离: %.1fcm\n", metrics["distance"].get<double>());
            
            // 稳定性
            const auto& stability = face["stability"];
            printf("  稳定性: %s\n", stability["is_stable"].get<bool>() ? "稳定" : "不稳定");
            printf("  运动模糊: %.1f\n", stability["motion_blur"].get<double>());
        }
        
    } catch (const std::exception& e) {
        printf("JSON解析错误: %s\n", e.what());
    }
}
```

## 🔧 配置选项

### 修改检测参数

编辑 `include/fast_face_config.h`:

```cpp
// 人脸检测参数
constexpr double FACE_DETECTION_SCALE_FACTOR = 1.1;  // 检测尺度因子
constexpr int FACE_DETECTION_MIN_NEIGHBORS = 3;      // 最小邻居数
constexpr int FACE_DETECTION_MIN_SIZE = 50;          // 最小人脸尺寸

// 稳定性检测参数
constexpr int STABLE_FRAMES_THRESHOLD = 3;           // 稳定帧数阈值
constexpr double STABILITY_THRESHOLD = 0.1;          // 稳定性阈值

// 图像质量参数
constexpr int SHARPNESS_THRESHOLD = 50;              // 清晰度阈值
constexpr double BRIGHTNESS_MIN = 30.0;              // 最小亮度
constexpr double BRIGHTNESS_MAX = 220.0;             // 最大亮度
constexpr double BRIGHTNESS_OPTIMAL = 127.5;         // 最佳亮度
```

### 启用试用模式

```cpp
// 启用试用模式
constexpr bool ENABLE_TRIAL_MODE = true;
constexpr int TRIAL_DAYS = 7;
```

## 🚨 常见问题

### Q: 许可证验证失败怎么办？
A: 检查以下几点：
1. 密钥格式是否正确
2. 密钥是否已过期
3. 是否启用了试用模式
4. 密钥是否与配置文件中的一致

### Q: SDK初始化失败怎么办？
A: 检查以下几点：
1. OpenCV是否正确安装
2. 模型文件是否存在
3. 许可证是否有效
4. 系统环境是否满足要求

### Q: 分析结果为空怎么办？
A: 检查以下几点：
1. 图像是否包含人脸
2. 图像质量是否足够
3. 图像格式是否为BGR
4. 检测参数是否合适

### Q: 性能较慢怎么办？
A: 优化建议：
1. 降低输入图像分辨率
2. 减少处理频率
3. 使用更快的硬件
4. 调整检测参数

### Q: 如何更换许可证密钥？
A: 按以下步骤操作：
1. 编辑 `include/fast_face_config.h` 文件
2. 修改 `VALID_LICENSE_KEY` 的值
3. 重新编译SDK
4. 更新您的应用程序代码

## 📞 技术支持

如果遇到问题，请：

1. 查看错误代码对照表
2. 运行测试程序验证环境
3. 检查许可证状态
4. 联系技术支持获取帮助

---

**注意**: 使用本SDK即表示同意遵守相关许可证条款。未经授权不得使用、复制或分发。 