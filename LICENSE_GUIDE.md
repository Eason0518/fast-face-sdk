# 🔐 FastFaceSDK 许可证使用指南

本指南详细说明FastFaceSDK的许可证系统使用方法。

## 📋 许可证系统概述

FastFaceSDK采用密钥验证机制，确保只有获得授权的用户才能使用SDK功能。

### 许可证类型

1. **商业许可证** - 完整功能访问权限，无时间限制
2. **试用许可证** - 7天试用期，功能完整
3. **临时许可证** - 指定天数的临时使用权限

### 许可证格式

```
FAST_FACE_[TYPE]_[YEAR]_[MM][DD]_[RANDOM]
```

示例：
- `FAST_FACE_COMMERCIAL_2024_1231_ABC12345`
- `FAST_FACE_TRIAL_XYZ789DEF`
- `FAST_FACE_TEMP_2024_0201_12345678`

## 🛠️ 许可证管理工具

SDK提供了专门的许可证管理工具 `license_manager`，用于生成和管理许可证。

### 编译许可证管理工具

```bash
# Windows
build_windows.bat

# Linux/macOS
./build_unix.sh
```

### 使用许可证管理工具

```bash
# 启动工具
./build/bin/license_manager

# 查看帮助
license> help
```

### 常用命令

#### 生成许可证
```bash
# 生成1个商业许可证
license> generate commercial

# 生成5个试用许可证
license> generate trial 5

# 生成10个30天临时许可证
license> generate temporary 10 30
```

#### 验证许可证
```bash
# 验证许可证格式
license> validate FAST_FACE_2024_LICENSE_KEY_12345
```

#### 管理许可证
```bash
# 显示生成的许可证列表
license> list

# 保存许可证到文件
license> save licenses.txt
```

## 🔧 SDK中的许可证验证

### 1. 验证许可证

```cpp
#include "fast_face_sdk.h"

int main() {
    // 验证许可证
    int result = verify_license("YOUR_LICENSE_KEY");
    if (result == 0) {
        printf("许可证有效\n");
    } else {
        printf("许可证无效，错误代码: %d\n", result);
        return -1;
    }
    
    return 0;
}
```

### 2. 初始化SDK

```cpp
// 使用许可证初始化SDK
int init_result = sdk_init("YOUR_LICENSE_KEY");
if (init_result != 0) {
    printf("SDK初始化失败: %d\n", init_result);
    return -1;
}
```

### 3. 获取许可证信息

```cpp
char license_info[1024];
int info_result = get_license_info(license_info, sizeof(license_info));
if (info_result == 0) {
    printf("许可证信息: %s\n", license_info);
}
```

### 4. 分析过程中的验证

每次调用 `analyze_frame` 时，SDK会自动验证许可证状态：

```cpp
char result_json[4096];
int analysis_result = analyze_frame(image_data, width, height, result_json, sizeof(result_json));

if (analysis_result == -1) {
    printf("许可证密钥无效\n");
} else if (analysis_result == -2) {
    printf("许可证密钥已过期\n");
} else if (analysis_result == -3) {
    printf("试用期已过期\n");
}
```

## 📊 许可证信息格式

### JSON格式
```json
{
  "key": "FAST_FACE_2024_LICENSE_KEY_12345",
  "status": "valid",
  "type": "commercial",
  "expires": "2024-12-31",
  "is_valid": true,
  "is_trial": false,
  "trial_remaining_days": 0
}
```

### 字段说明
- `key`: 许可证密钥
- `status`: 许可证状态 (valid/invalid/expired)
- `type`: 许可证类型 (commercial/trial/temporary)
- `expires`: 过期时间 (YYYY-MM-DD)
- `is_valid`: 是否有效
- `is_trial`: 是否为试用版
- `trial_remaining_days`: 试用剩余天数

## ⚙️ 配置许可证系统

### 修改默认许可证

编辑 `include/fast_face_config.h`:

```cpp
// 修改为您的许可证密钥
constexpr const char* VALID_LICENSE_KEY = "YOUR_ACTUAL_LICENSE_KEY";

// 启用试用模式
constexpr bool ENABLE_TRIAL_MODE = true;
constexpr int TRIAL_DAYS = 7;
```

### 自定义许可证验证逻辑

在 `src/fast_face_sdk.cpp` 中修改验证函数：

```cpp
bool validate_license_key(const std::string& key) {
    // 添加您的自定义验证逻辑
    if (key.empty() || key.length() < FastFaceConfig::MIN_KEY_LENGTH) {
        return false;
    }
    
    // 检查密钥格式
    if (key.find("FAST_FACE_") != 0) {
        return false;
    }
    
    // 检查是否为有效密钥
    return key == FastFaceConfig::VALID_LICENSE_KEY;
}
```

## 🚨 错误处理

### 常见错误代码

| 错误代码 | 含义 | 解决方案 |
|---------|------|---------|
| -1 | 密钥无效 | 检查密钥格式和内容 |
| -2 | 密钥已过期 | 更新许可证密钥 |
| -3 | 试用期已过期 | 申请新的试用许可证或购买商业许可证 |

### 错误处理示例

```cpp
int handle_license_error(int error_code) {
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
        default:
            printf("未知错误: %d\n", error_code);
            break;
    }
    return error_code;
}
```

## 🔒 安全建议

### 1. 密钥保护
- 不要在代码中硬编码许可证密钥
- 使用环境变量或配置文件存储密钥
- 定期更换许可证密钥

### 2. 网络验证
- 考虑添加在线验证机制
- 实现许可证服务器
- 添加反调试和反逆向工程措施

### 3. 加密存储
- 对许可证密钥进行加密存储
- 使用硬件绑定技术
- 实现许可证吊销机制

## 📝 许可证文件格式

### 许可证列表文件
```
# FastFaceSDK 许可证列表
# 生成时间: 1704067200

FAST_FACE_COMMERCIAL_2024_1231_ABC12345
FAST_FACE_TRIAL_XYZ789DEF
FAST_FACE_TEMP_2024_0201_12345678
```

### 配置文件格式
```ini
[License]
Key=FAST_FACE_2024_LICENSE_KEY_12345
Type=commercial
Expires=2024-12-31

[Trial]
Enabled=true
Days=7
```

## 🎯 最佳实践

### 1. 开发阶段
- 使用试用许可证进行开发
- 定期验证许可证状态
- 实现优雅的错误处理

### 2. 部署阶段
- 使用商业许可证
- 实现许可证自动更新
- 添加许可证状态监控

### 3. 维护阶段
- 定期检查许可证状态
- 及时更新过期许可证
- 备份许可证信息

## 📞 技术支持

如果遇到许可证相关问题，请：

1. 检查许可证格式是否正确
2. 确认许可证是否已过期
3. 运行许可证管理工具验证
4. 联系技术支持获取帮助

---

**注意**: 使用本SDK即表示同意遵守相关许可证条款。未经授权不得使用、复制或分发。 