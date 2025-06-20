# FastFaceSDK - 人脸分析SDK

FastFaceSDK 是一个高性能的C++人脸分析SDK，提供人脸检测、头部姿态估计、图像质量评估、口罩检测、运动模糊检测、稳定性分析和距离估计等功能。

## 🔐 许可证系统

**重要提示：使用本SDK需要有效的许可证密钥！**

### 许可证类型

1. **商业许可证** - 完整功能访问权限
2. **试用许可证** - 有限时间试用（7天）

### 获取许可证

请联系开发者获取有效的许可证密钥：
- 商业许可证：`FAST_FACE_2024_LICENSE_KEY_12345`
- 试用许可证：`TRIAL_KEY`（需要启用试用模式）

### 许可证验证

SDK会在以下情况下验证许可证：
- SDK初始化时
- 每次调用分析函数时
- 获取许可证信息时

## 🚀 快速开始

### 1. 环境要求

- **操作系统**: Windows 10+, Linux, macOS
- **编译器**: 
  - Windows: Visual Studio 2019+ 或 MinGW-w64
  - Linux/macOS: GCC 7+ 或 Clang 10+
- **依赖库**:
  - OpenCV 4.5+
  - nlohmann/json 3.9+

### 2. 编译安装

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

### 3. 基本使用

```cpp
#include "fast_face_sdk.h"

int main() {
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
    
    // 3. 分析图像
    char result_json[4096];
    int analysis_result = analyze_frame(image_data, width, height, result_json, sizeof(result_json));
    
    if (analysis_result == 0) {
        printf("分析结果: %s\n", result_json);
    } else {
        printf("分析失败，错误代码: %d\n", analysis_result);
    }
    
    // 4. 释放资源
    sdk_release();
    
    return 0;
}
```

## 📋 API 参考

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

#### `sdk_init(const char* license_key)`
初始化SDK。

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

#### `get_license_info(char* license_info, int info_buf_len)`
获取当前许可证信息。

**参数:**
- `license_info`: 输出许可证信息的缓冲区
- `info_buf_len`: 缓冲区长度

**返回值:**
- `0`: 获取成功
- `-8`: 参数错误
- `-9`: 缓冲区太小

#### `sdk_release()`
释放SDK资源。

### 返回的JSON格式

```json
{
  "code": 0,
  "msg": "success",
  "license_info": {
    "status": "valid",
    "expires": "2024-12-31",
    "type": "commercial",
    "trial_remaining_days": 5
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

## 🛠️ 功能特性

### 人脸检测
- 基于OpenCV Haar级联分类器
- 支持多人脸检测
- 实时性能优化

### 头部姿态估计
- 3D头部姿态计算
- Yaw（偏航）、Pitch（俯仰）、Roll（翻滚）角度
- 基于面部关键点

### 图像质量评估
- **清晰度评分**: 基于Laplacian算子
- **亮度评分**: 基于图像亮度分布
- **对比度评分**: 基于图像标准差

### 口罩检测
- 基于HSV颜色空间
- 支持蓝色和白色口罩检测
- 可配置检测阈值

### 运动模糊检测
- 基于光流算法
- 实时运动模糊评估
- 稳定性分析

### 距离估计
- 基于瞳孔距离
- 简化的距离计算
- 可配置参数

## 📁 项目结构

```
new_fast_1/
├── include/
│   ├── fast_face_sdk.h          # 主要API头文件
│   └── fast_face_config.h       # 配置和常量定义
├── src/
│   └── fast_face_sdk.cpp        # SDK实现
├── example/
│   ├── main.cpp                 # 示例程序
│   └── CMakeLists.txt           # 示例构建配置
├── test_sdk.cpp                 # 测试程序
├── CMakeLists.txt               # 主构建配置
├── build_windows.bat            # Windows构建脚本
├── build_unix.sh                # Linux/macOS构建脚本
├── install_windows.bat          # Windows安装脚本
├── install_unix.sh              # Linux/macOS安装脚本
└── README.md                    # 本文档
```

## 🔧 配置选项

在 `include/fast_face_config.h` 中可以修改以下配置：

### 许可证配置
```cpp
// 有效许可证密钥
constexpr const char* VALID_LICENSE_KEY = "FAST_FACE_2024_LICENSE_KEY_12345";

// 密钥长度限制
constexpr int MAX_KEY_LENGTH = 64;
constexpr int MIN_KEY_LENGTH = 10;

// 试用模式
constexpr bool ENABLE_TRIAL_MODE = false;
constexpr int TRIAL_DAYS = 7;
```

### 检测参数
```cpp
// 人脸检测参数
constexpr double FACE_DETECTION_SCALE_FACTOR = 1.1;
constexpr int FACE_DETECTION_MIN_NEIGHBORS = 3;
constexpr int FACE_DETECTION_MIN_SIZE = 50;

// 稳定性检测
constexpr int STABLE_FRAMES_THRESHOLD = 3;
constexpr double STABILITY_THRESHOLD = 0.1;
```

### 质量评估参数
```cpp
// 图像质量阈值
constexpr int SHARPNESS_THRESHOLD = 50;
constexpr int MOTION_BLUR_THRESHOLD = 100;
constexpr double BRIGHTNESS_MIN = 30.0;
constexpr double BRIGHTNESS_MAX = 220.0;
constexpr double BRIGHTNESS_OPTIMAL = 127.5;
```

## 🧪 测试

运行测试程序：
```bash
# 编译测试程序
g++ -o test_sdk test_sdk.cpp -I./include -L./build -lfast_face_sdk -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_face -lopencv_video -lopencv_imgcodecs -lopencv_highgui

# 运行测试
./test_sdk
```

## 📝 错误代码

| 错误代码 | 含义 |
|---------|------|
| 0 | 成功 |
| -1 | 密钥无效 |
| -2 | 密钥已过期 |
| -3 | 试用期已过期 |
| -4 | 人脸检测模型加载失败 |
| -5 | 眼睛检测模型加载失败 |
| -6 | 初始化异常 |
| -7 | 分析过程异常 |
| -8 | 参数错误 |
| -9 | 缓冲区太小 |
| -100 | SDK未初始化 |

## 🤝 许可证管理

### 商业使用
如需商业使用，请联系开发者获取商业许可证。

### 试用版本
试用版本具有以下限制：
- 7天使用期限
- 功能完整
- 需要启用试用模式

### 许可证更新
许可证过期后需要更新密钥才能继续使用。

## 📞 技术支持

如有问题或需要技术支持，请联系开发者。

## 📄 许可证

本项目采用专有许可证，未经授权不得使用、复制或分发。

---

**注意**: 使用本SDK即表示同意遵守相关许可证条款。 