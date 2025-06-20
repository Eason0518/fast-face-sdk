#include <iostream>
#include <string>

// 从配置文件中提取的密钥信息
const char* VALID_LICENSE_KEY = "FAST_FACE_2024_LICENSE_KEY_12345";
const int MAX_KEY_LENGTH = 64;
const int MIN_KEY_LENGTH = 10;
const bool ENABLE_TRIAL_MODE = false;
const int TRIAL_DAYS = 7;

int main() {
    std::cout << "🔑 FastFaceSDK 密钥信息" << std::endl;
    std::cout << "=======================" << std::endl;
    
    std::cout << "\n📋 当前配置的密钥:" << std::endl;
    std::cout << "密钥: " << VALID_LICENSE_KEY << std::endl;
    std::cout << "长度: " << strlen(VALID_LICENSE_KEY) << " 字符" << std::endl;
    
    std::cout << "\n⚙️ 密钥配置参数:" << std::endl;
    std::cout << "最小长度: " << MIN_KEY_LENGTH << " 字符" << std::endl;
    std::cout << "最大长度: " << MAX_KEY_LENGTH << " 字符" << std::endl;
    std::cout << "试用模式: " << (ENABLE_TRIAL_MODE ? "启用" : "禁用") << std::endl;
    std::cout << "试用天数: " << TRIAL_DAYS << " 天" << std::endl;
    
    std::cout << "\n🔍 密钥格式分析:" << std::endl;
    std::string key(VALID_LICENSE_KEY);
    
    if (key.find("FAST_FACE_") == 0) {
        std::cout << "✓ 前缀正确 (FAST_FACE_)" << std::endl;
    } else {
        std::cout << "✗ 前缀错误" << std::endl;
    }
    
    if (key.length() >= MIN_KEY_LENGTH && key.length() <= MAX_KEY_LENGTH) {
        std::cout << "✓ 长度符合要求 (" << key.length() << " 字符)" << std::endl;
    } else {
        std::cout << "✗ 长度不符合要求" << std::endl;
    }
    
    // 检查是否包含年份信息
    if (key.find("2024") != std::string::npos) {
        std::cout << "✓ 包含年份信息 (2024)" << std::endl;
    } else {
        std::cout << "✗ 未包含年份信息" << std::endl;
    }
    
    std::cout << "\n💡 使用说明:" << std::endl;
    std::cout << "1. 在代码中使用此密钥初始化SDK:" << std::endl;
    std::cout << "   sdk_init(\"" << VALID_LICENSE_KEY << "\");" << std::endl;
    std::cout << std::endl;
    std::cout << "2. 验证密钥有效性:" << std::endl;
    std::cout << "   verify_license(\"" << VALID_LICENSE_KEY << "\");" << std::endl;
    std::cout << std::endl;
    std::cout << "3. 如需修改密钥，请编辑 include/fast_face_config.h 文件" << std::endl;
    
    std::cout << "\n⚠️  安全提醒:" << std::endl;
    std::cout << "- 请妥善保管您的密钥" << std::endl;
    std::cout << "- 不要在公开代码中暴露密钥" << std::endl;
    std::cout << "- 定期更换密钥以提高安全性" << std::endl;
    
    std::cout << "\n🎯 密钥类型: 商业许可证" << std::endl;
    std::cout << "过期时间: 2024年12月31日" << std::endl;
    
    return 0;
} 