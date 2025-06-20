# 🚀 FastFaceSDK 快速开始指南

本指南将帮助您在5分钟内快速上手FastFaceSDK。

## 🔑 重要：许可证密钥

**⚠️ 使用SDK前必须提供有效的许可证密钥！**

### 您的默认密钥
```
FAST_FACE_2024_LICENSE_KEY_12345
```

### 密钥验证步骤
1. **验证密钥格式**
2. **检查密钥有效性**
3. **确认密钥未过期**

## 📋 前置要求

### 系统要求
- **Windows 10+** 或 **Linux** 或 **macOS**
- **C++17** 兼容的编译器
- **OpenCV 4.5+** 已安装

### 获取许可证
**重要**: 使用SDK前需要有效的许可证密钥！

1. **商业许可证**: 联系开发者获取
2. **试用许可证**: 使用 `TRIAL_KEY`（需要启用试用模式）
3. **默认密钥**: `FAST_FACE_2024_LICENSE_KEY_12345`

## ⚡ 5分钟快速开始

### 1. 下载SDK (1分钟)
```bash
# 克隆项目
git clone https://github.com/your-repo/fast-face-sdk.git
cd fast-face-sdk

# 或下载ZIP包并解压
```

### 2. 一键编译 (2分钟)

#### Windows用户
```bash
# 双击运行
build_windows.bat

# 或在命令行运行
.\build_windows.bat
```

#### Linux/macOS用户
```bash
# 添加执行权限
chmod +x build_unix.sh

# 运行编译脚本
./build_unix.sh
```

### 3. 🔑 验证许可证密钥 (1分钟)
```bash
# 运行密钥查看工具
.\show_key.bat

# 或运行许可证测试
.\build\bin\test_license.exe
```

**预期输出:**
```
当前密钥: FAST_FACE_2024_LICENSE_KEY_12345
密钥长度: 32 字符
密钥类型: 商业许可证
过期时间: 2024年12月31日
```

### 4. 运行示例 (1分钟)
```bash
# 运行示例程序
.\build\bin\example.exe
```

## 🔐 许可证验证

### 验证许可证密钥
```cpp
#include "fast_face_sdk.h"

int main() {
    // 🔑 第一步：验证许可证
    const char* license_key = "FAST_FACE_2024_LICENSE_KEY_12345";
    int verify_result = verify_license(license_key);
    if (verify_result == 0) {
        printf("✅ 许可证验证成功\n");
    } else {
        printf("❌ 许可证验证失败，错误代码: %d\n", verify_result);
        return -1;
    }
    
    // 🔧 第二步：初始化SDK
    int init_result = sdk_init(license_key);
    if (init_result == 0) {
        printf("✅ SDK初始化成功\n");
    } else {
        printf("❌ SDK初始化失败，错误代码: %d\n", init_result);
        return -1;
    }
    
    // 🧹 释放资源
    sdk_release();
    return 0;
}
```

### 常见许可证错误
| 错误代码 | 解决方案 |
|---------|---------|
| -1 | 检查密钥格式是否正确 |
| -2 | 密钥已过期，需要更新 |
| -3 | 试用期已过期 |

## 💻 基本使用示例

### 完整示例代码
```cpp
#include "fast_face_sdk.h"
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // 🔑 第一步：设置许可证密钥
    const char* license_key = "FAST_FACE_2024_LICENSE_KEY_12345"; // 替换为您的密钥
    
    // 🔍 第二步：验证许可证
    int verify_result = verify_license(license_key);
    if (verify_result != 0) {
        std::cout << "❌ 许可证验证失败: " << verify_result << std::endl;
        std::cout << "请检查您的许可证密钥是否正确" << std::endl;
        return -1;
    }
    std::cout << "✅ 许可证验证成功" << std::endl;
    
    // 🔧 第三步：初始化SDK
    int init_result = sdk_init(license_key);
    if (init_result != 0) {
        std::cout << "❌ SDK初始化失败: " << init_result << std::endl;
        return -1;
    }
    std::cout << "✅ SDK初始化成功" << std::endl;
    
    // 📊 第四步：获取许可证信息
    char license_info[1024];
    int info_result = get_license_info(license_info, sizeof(license_info));
    if (info_result == 0) {
        std::cout << "📋 许可证信息: " << license_info << std::endl;
    }
    
    // 🖼️ 第五步：读取图像
    cv::Mat image = cv::imread("test.jpg");
    if (image.empty()) {
        std::cout << "❌ 无法读取图像文件" << std::endl;
        sdk_release();
        return -1;
    }
    std::cout << "✅ 图像读取成功: " << image.cols << "x" << image.rows << std::endl;
    
    // 🔍 第六步：分析图像
    char result_json[4096];
    int analysis_result = analyze_frame(
        image.data, 
        image.cols, 
        image.rows, 
        result_json, 
        sizeof(result_json)
    );
    
    // 📊 第七步：处理分析结果
    if (analysis_result == 0) {
        std::cout << "✅ 图像分析成功" << std::endl;
        std::cout << "分析结果: " << result_json << std::endl;
    } else {
        std::cout << "❌ 图像分析失败，错误代码: " << analysis_result << std::endl;
        
        // 处理许可证相关错误
        if (analysis_result == -1) {
            std::cout << "许可证密钥无效" << std::endl;
        } else if (analysis_result == -2) {
            std::cout << "许可证密钥已过期" << std::endl;
        } else if (analysis_result == -3) {
            std::cout << "试用期已过期" << std::endl;
        }
    }
    
    // 🧹 第八步：释放资源
    sdk_release();
    std::cout << "✅ 资源释放完成" << std::endl;
    
    return 0;
}
```

### 编译示例
```bash
# 编译示例程序
g++ -o example example.cpp \
    -I./include \
    -L./build \
    -lfast_face_sdk \
    -lopencv_core -lopencv_imgproc -lopencv_objdetect \
    -lopencv_face -lopencv_video -lopencv_imgcodecs \
    -lopencv_highgui

# 运行示例
./example
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
    "type": "commercial"
  },
  "faces": [
    {
      "bbox": {"x": 100, "y": 50, "width": 200, "height": 250},
      "pose": {"yaw": 5.2, "pitch": -2.1, "roll": 1.5},
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

void parse_result(const char* json_str) {
    try {
        nlohmann::json result = nlohmann::json::parse(json_str);
        
        if (result["code"] == 0) {
            std::cout << "检测到 " << result["faces"].size() << " 个人脸" << std::endl;
            
            for (const auto& face : result["faces"]) {
                const auto& bbox = face["bbox"];
                std::cout << "人脸位置: (" << bbox["x"] << ", " << bbox["y"] << ")" << std::endl;
                
                const auto& pose = face["pose"];
                std::cout << "头部姿态: Yaw=" << pose["yaw"] << "°" << std::endl;
                
                const auto& scores = face["quality_scores"];
                std::cout << "清晰度评分: " << scores["sharpness_score"] << "/100" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "JSON解析错误: " << e.what() << std::endl;
    }
}
```

## 🔧 配置选项

### 修改许可证密钥
编辑 `include/fast_face_config.h`:
```cpp
// 修改为您的许可证密钥
constexpr const char* VALID_LICENSE_KEY = "YOUR_ACTUAL_LICENSE_KEY";
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

### Q: 分析结果为空怎么办？
A: 检查以下几点：
1. 图像是否包含人脸
2. 图像质量是否足够
3. 图像格式是否为BGR

### Q: 性能较慢怎么办？
A: 优化建议：
1. 降低输入图像分辨率
2. 减少处理频率
3. 使用更快的硬件

### Q: 如何更换许可证密钥？
A: 按以下步骤操作：
1. 编辑 `include/fast_face_config.h` 文件
2. 修改 `VALID_LICENSE_KEY` 的值
3. 重新编译SDK
4. 更新您的应用程序代码

## 📞 获取帮助

如果遇到问题，请：
1. 查看错误代码对照表
2. 运行测试程序验证环境
3. 检查许可证状态
4. 联系技术支持

## 🎯 下一步

完成快速开始后，建议：
1. 阅读完整的[用户指南](USER_GUIDE.md)
2. 查看[API参考](README.md#api-参考)
3. 尝试集成到您的项目中

---

**提示**: 首次使用建议先运行测试程序确保环境配置正确！

**重要提醒**: 请妥善保管您的许可证密钥，不要在公开代码中暴露！ 