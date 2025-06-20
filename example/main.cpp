#include "fast_face_sdk.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <chrono>

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
            std::cout << "\n=== 许可证信息 ===" << std::endl;
            std::cout << "状态: " << license["status"] << std::endl;
            std::cout << "类型: " << license["type"] << std::endl;
            std::cout << "过期时间: " << license["expires"] << std::endl;
            
            if (license.contains("trial_remaining_days")) {
                std::cout << "试用剩余天数: " << license["trial_remaining_days"] << std::endl;
            }
        }
        
        std::cout << "\n=== 人脸分析结果 ===" << std::endl;
        std::cout << "检测到 " << result["faces"].size() << " 个人脸" << std::endl;
        
        for (size_t i = 0; i < result["faces"].size(); ++i) {
            const auto& face = result["faces"][i];
            std::cout << "\n人脸 " << (i + 1) << ":" << std::endl;
            
            // 边界框信息
            const auto& bbox = face["bbox"];
            std::cout << "  位置: (" << bbox["x"] << ", " << bbox["y"] 
                     << ") 大小: " << bbox["width"] << "x" << bbox["height"] << std::endl;
            
            // 头部姿态
            const auto& pose = face["pose"];
            std::cout << "  头部姿态: Yaw=" << pose["yaw"] << "°, Pitch=" 
                     << pose["pitch"] << "°, Roll=" << pose["roll"] << "°" << std::endl;
            
            // 图像质量指标
            const auto& metrics = face["metrics"];
            std::cout << "  清晰度: " << metrics["sharpness"] << std::endl;
            std::cout << "  亮度: " << metrics["brightness"] << std::endl;
            std::cout << "  距离: " << metrics["distance"] << "cm" << std::endl;
            std::cout << "  戴口罩: " << (metrics["has_mask"] ? "是" : "否") << std::endl;
            
            // 质量评分
            const auto& scores = face["quality_scores"];
            std::cout << "  质量评分:" << std::endl;
            std::cout << "    清晰度评分: " << scores["sharpness_score"] << "/100" << std::endl;
            std::cout << "    亮度评分: " << scores["brightness_score"] << "/100" << std::endl;
            std::cout << "    对比度评分: " << scores["contrast_score"] << "/100" << std::endl;
            
            // 稳定性
            const auto& stability = face["stability"];
            std::cout << "  稳定性: " << (stability["is_stable"] ? "稳定" : "不稳定") << std::endl;
            std::cout << "  运动模糊: " << stability["motion_blur"] << std::endl;
        }
        std::cout << "===================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "JSON解析错误: " << e.what() << std::endl;
    }
}

// 显示许可证信息
void print_license_info() {
    char license_info[1024];
    int result = get_license_info(license_info, sizeof(license_info));
    
    if (result == 0) {
        try {
            nlohmann::json info = nlohmann::json::parse(license_info);
            std::cout << "\n=== 当前许可证信息 ===" << std::endl;
            std::cout << "密钥: " << info["key"] << std::endl;
            std::cout << "状态: " << info["status"] << std::endl;
            std::cout << "类型: " << info["type"] << std::endl;
            std::cout << "过期时间: " << info["expires"] << std::endl;
            std::cout << "是否有效: " << (info["is_valid"] ? "是" : "否") << std::endl;
            std::cout << "是否试用: " << (info["is_trial"] ? "是" : "否") << std::endl;
            
            if (info["is_trial"]) {
                std::cout << "试用剩余天数: " << info["trial_remaining_days"] << std::endl;
            }
            std::cout << "=========================" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "许可证信息解析错误: " << e.what() << std::endl;
        }
    } else {
        std::cout << "获取许可证信息失败，错误代码: " << result << std::endl;
    }
}

int main() {
    std::cout << "FastFaceSDK 示例程序 (带密钥验证)" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "SDK版本: " << get_sdk_version() << std::endl;
    
    // 演示密钥验证
    std::cout << "\n=== 密钥验证演示 ===" << std::endl;
    
    // 测试无效密钥
    std::cout << "测试无效密钥..." << std::endl;
    int verify_result = verify_license("INVALID_KEY");
    if (verify_result == 0) {
        std::cout << "  ✓ 无效密钥被正确识别" << std::endl;
    } else {
        std::cout << "  ✗ 无效密钥验证失败，错误代码: " << verify_result << std::endl;
    }
    
    // 测试有效密钥
    std::cout << "测试有效密钥..." << std::endl;
    verify_result = verify_license("FAST_FACE_2024_LICENSE_KEY_12345");
    if (verify_result == 0) {
        std::cout << "  ✓ 有效密钥验证成功" << std::endl;
    } else {
        std::cout << "  ✗ 有效密钥验证失败，错误代码: " << verify_result << std::endl;
    }
    
    // 初始化SDK
    std::cout << "\n正在初始化SDK..." << std::endl;
    int init_result = sdk_init("FAST_FACE_2024_LICENSE_KEY_12345");
    if (init_result != 0) {
        std::cout << "SDK初始化失败，错误代码: " << init_result << std::endl;
        std::cout << "请检查许可证密钥是否正确" << std::endl;
        return -1;
    }
    std::cout << "SDK初始化成功!" << std::endl;
    
    // 显示许可证信息
    print_license_info();
    
    // 打开摄像头
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cout << "无法打开摄像头" << std::endl;
        sdk_release();
        return -1;
    }
    
    // 设置摄像头参数
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 30);
    
    std::cout << "\n按 'q' 退出程序" << std::endl;
    std::cout << "按 's' 保存当前帧" << std::endl;
    std::cout << "按 'a' 分析当前帧" << std::endl;
    std::cout << "按 'l' 显示许可证信息" << std::endl;
    
    cv::Mat frame;
    int frame_count = 0;
    auto last_analysis_time = std::chrono::steady_clock::now();
    
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            std::cout << "无法获取视频帧" << std::endl;
            break;
        }
        
        // 显示原始帧
        cv::imshow("FastFaceSDK Demo", frame);
        
        // 每30帧自动分析一次（约1秒）
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - last_analysis_time);
        
        if (elapsed.count() > 1000) { // 1秒
            // 分析当前帧
            char result_json[4096];
            int analysis_result = analyze_frame(frame.data, frame.cols, frame.rows, result_json, sizeof(result_json));
            
            if (analysis_result == 0) {
                print_analysis_results(result_json);
            } else {
                std::cout << "分析失败，错误代码: " << analysis_result << std::endl;
                
                // 检查是否是许可证相关错误
                if (analysis_result == -1) {
                    std::cout << "许可证密钥无效" << std::endl;
                } else if (analysis_result == -2) {
                    std::cout << "许可证密钥已过期" << std::endl;
                } else if (analysis_result == -3) {
                    std::cout << "试用期已过期" << std::endl;
                }
            }
            
            last_analysis_time = current_time;
        }
        
        frame_count++;
        
        // 处理键盘输入
        char key = cv::waitKey(1) & 0xFF;
        if (key == 'q') {
            break;
        } else if (key == 's') {
            // 保存当前帧
            std::string filename = "captured_frame_" + std::to_string(frame_count) + ".jpg";
            cv::imwrite(filename, frame);
            std::cout << "帧已保存为: " << filename << std::endl;
        } else if (key == 'a') {
            // 手动分析当前帧
            char result_json[4096];
            int analysis_result = analyze_frame(frame.data, frame.cols, frame.rows, result_json, sizeof(result_json));
            
            if (analysis_result == 0) {
                print_analysis_results(result_json);
            } else {
                std::cout << "分析失败，错误代码: " << analysis_result << std::endl;
            }
        } else if (key == 'l') {
            // 显示许可证信息
            print_license_info();
        }
    }
    
    // 清理资源
    cap.release();
    cv::destroyAllWindows();
    sdk_release();
    
    std::cout << "\n程序结束" << std::endl;
    return 0;
} 