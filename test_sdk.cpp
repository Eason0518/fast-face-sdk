#include "include/fast_face_sdk.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>

// 解析JSON结果的辅助函数
void print_analysis_results(const std::string& json_str) {
    try {
        nlohmann::json result = nlohmann::json::parse(json_str);
        
        if (result["code"] != 0) {
            std::cout << "分析失败: " << result["msg"] << std::endl;
            return;
        }
        
        // 显示许可证信息
        if (result.contains("license_info")) {
            const auto& license = result["license_info"];
            std::cout << "许可证状态: " << license["status"] << std::endl;
        }
        
        std::cout << "检测到 " << result["faces"].size() << " 个人脸" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "JSON解析错误: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "FastFaceSDK 测试程序 (带密钥验证)" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "SDK版本: " << get_sdk_version() << std::endl;
    
    // 测试1: 密钥验证功能
    std::cout << "\n1. 测试密钥验证功能..." << std::endl;
    
    // 测试无效密钥
    int verify_result = verify_license("INVALID_KEY");
    if (verify_result != 0) {
        std::cout << "   ✓ 无效密钥被正确拒绝" << std::endl;
    } else {
        std::cout << "   ✗ 无效密钥未被拒绝" << std::endl;
    }
    
    // 测试有效密钥
    verify_result = verify_license("FAST_FACE_2024_LICENSE_KEY_12345");
    if (verify_result == 0) {
        std::cout << "   ✓ 有效密钥验证成功" << std::endl;
    } else {
        std::cout << "   ✗ 有效密钥验证失败，错误代码: " << verify_result << std::endl;
    }
    
    // 测试2: SDK初始化
    std::cout << "\n2. 测试SDK初始化..." << std::endl;
    int init_result = sdk_init("FAST_FACE_2024_LICENSE_KEY_12345");
    if (init_result == 0) {
        std::cout << "   ✓ SDK初始化成功" << std::endl;
    } else {
        std::cout << "   ✗ SDK初始化失败，错误代码: " << init_result << std::endl;
        return -1;
    }
    
    // 测试3: 许可证信息获取
    std::cout << "\n3. 测试许可证信息获取..." << std::endl;
    char license_info[1024];
    int license_result = get_license_info(license_info, sizeof(license_info));
    if (license_result == 0) {
        std::cout << "   ✓ 许可证信息获取成功" << std::endl;
        try {
            nlohmann::json info = nlohmann::json::parse(license_info);
            std::cout << "   密钥: " << info["key"] << std::endl;
            std::cout << "   状态: " << info["status"] << std::endl;
            std::cout << "   类型: " << info["type"] << std::endl;
            std::cout << "   过期时间: " << info["expires"] << std::endl;
        } catch (const std::exception& e) {
            std::cout << "   ✗ 许可证信息解析失败: " << e.what() << std::endl;
        }
    } else {
        std::cout << "   ✗ 许可证信息获取失败，错误代码: " << license_result << std::endl;
    }
    
    // 测试4: 错误密钥初始化
    std::cout << "\n4. 测试错误密钥初始化..." << std::endl;
    int wrong_key_result = sdk_init("WRONG_KEY");
    if (wrong_key_result != 0) {
        std::cout << "   ✓ 错误密钥被正确拒绝" << std::endl;
    } else {
        std::cout << "   ✗ 错误密钥未被拒绝" << std::endl;
    }
    
    // 重新初始化
    sdk_init("FAST_FACE_2024_LICENSE_KEY_12345");
    
    // 测试5: 空图像分析
    std::cout << "\n5. 测试空图像分析..." << std::endl;
    char result_json[4096];
    int empty_result = analyze_frame(nullptr, 640, 480, result_json, sizeof(result_json));
    if (empty_result != 0) {
        std::cout << "   ✓ 空图像被正确拒绝" << std::endl;
    } else {
        std::cout << "   ✗ 空图像未被拒绝" << std::endl;
    }
    
    // 测试6: 创建测试图像
    std::cout << "\n6. 测试图像分析..." << std::endl;
    cv::Mat test_image(480, 640, CV_8UC3, cv::Scalar(128, 128, 128));
    
    // 在图像中心画一个简单的"人脸"
    cv::circle(test_image, cv::Point(320, 240), 100, cv::Scalar(255, 200, 150), -1);
    cv::circle(test_image, cv::Point(300, 220), 10, cv::Scalar(0, 0, 0), -1); // 左眼
    cv::circle(test_image, cv::Point(340, 220), 10, cv::Scalar(0, 0, 0), -1); // 右眼
    cv::ellipse(test_image, cv::Point(320, 260), cv::Size(20, 10), 0, 0, 180, cv::Scalar(0, 0, 0), 2); // 嘴
    
    int analysis_result = analyze_frame(test_image.data, test_image.cols, test_image.rows, result_json, sizeof(result_json));
    if (analysis_result == 0) {
        std::cout << "   ✓ 图像分析成功" << std::endl;
        print_analysis_results(result_json);
    } else {
        std::cout << "   ✗ 图像分析失败，错误代码: " << analysis_result << std::endl;
    }
    
    // 测试7: 保存测试图像
    cv::imwrite("test_image.jpg", test_image);
    std::cout << "\n7. 测试图像已保存为 test_image.jpg" << std::endl;
    
    // 测试8: 释放资源
    std::cout << "\n8. 测试资源释放..." << std::endl;
    sdk_release();
    std::cout << "   ✓ 资源释放完成" << std::endl;
    
    std::cout << "\n所有测试完成！" << std::endl;
    return 0;
} 