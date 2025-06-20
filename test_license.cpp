#include "include/fast_face_sdk.h"
#include <iostream>

int main() {
    std::cout << "FastFaceSDK 许可证测试程序" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "SDK版本: " << get_sdk_version() << std::endl;
    
    // 测试1: 验证有效密钥
    std::cout << "\n1. 测试有效密钥..." << std::endl;
    int result1 = verify_license("FAST_FACE_2024_LICENSE_KEY_12345");
    if (result1 == 0) {
        std::cout << "   ✓ 有效密钥验证成功" << std::endl;
    } else {
        std::cout << "   ✗ 有效密钥验证失败: " << result1 << std::endl;
    }
    
    // 测试2: 验证无效密钥
    std::cout << "\n2. 测试无效密钥..." << std::endl;
    int result2 = verify_license("INVALID_KEY");
    if (result2 != 0) {
        std::cout << "   ✓ 无效密钥被正确拒绝: " << result2 << std::endl;
    } else {
        std::cout << "   ✗ 无效密钥未被拒绝" << std::endl;
    }
    
    // 测试3: 初始化SDK
    std::cout << "\n3. 测试SDK初始化..." << std::endl;
    int init_result = sdk_init("FAST_FACE_2024_LICENSE_KEY_12345");
    if (init_result == 0) {
        std::cout << "   ✓ SDK初始化成功" << std::endl;
    } else {
        std::cout << "   ✗ SDK初始化失败: " << init_result << std::endl;
        return -1;
    }
    
    // 测试4: 获取许可证信息
    std::cout << "\n4. 测试许可证信息获取..." << std::endl;
    char license_info[1024];
    int info_result = get_license_info(license_info, sizeof(license_info));
    if (info_result == 0) {
        std::cout << "   ✓ 许可证信息获取成功" << std::endl;
        std::cout << "   信息: " << license_info << std::endl;
    } else {
        std::cout << "   ✗ 许可证信息获取失败: " << info_result << std::endl;
    }
    
    // 测试5: 释放资源
    std::cout << "\n5. 测试资源释放..." << std::endl;
    sdk_release();
    std::cout << "   ✓ 资源释放完成" << std::endl;
    
    std::cout << "\n所有测试完成！" << std::endl;
    return 0;
} 