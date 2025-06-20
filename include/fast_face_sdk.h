#pragma once

#include "fast_face_config.h"

#ifdef _WIN32
#define FAST_FACE_API __declspec(dllexport)
#else
#define FAST_FACE_API
#endif

extern "C" {
    /**
     * @brief 获取SDK版本信息
     * @return SDK版本字符串
     */
    FAST_FACE_API const char* get_sdk_version();
    
    /**
     * @brief 验证许可证密钥
     * @param license_key 许可证密钥
     * @return 0表示有效，非0表示无效
     * 
     * 错误代码:
     * - -1: 密钥格式错误
     * - -2: 密钥已过期
     * - -3: 试用期已过期
     */
    FAST_FACE_API int verify_license(const char* license_key);
    
    /**
     * @brief 获取许可证信息
     * @param license_info 输出许可证信息的缓冲区
     * @param info_buf_len 缓冲区长度
     * @return 0表示成功，非0表示失败
     */
    FAST_FACE_API int get_license_info(char* license_info, int info_buf_len);
    
    /**
     * @brief 初始化SDK
     * @param license_key 许可证密钥
     * @return 0表示成功，非0表示失败
     * 
     * 错误代码:
     * - -1: 密钥无效
     * - -2: 密钥已过期
     * - -3: 试用期已过期
     * - -4: 人脸检测模型加载失败
     * - -5: 眼睛检测模型加载失败
     * - -6: 初始化异常
     */
    FAST_FACE_API int sdk_init(const char* license_key);

    /**
     * @brief 分析一帧BGR图像
     * @param bgr_data BGR格式的图像数据
     * @param width 图像宽度
     * @param height 图像高度
     * @param result_json 输出JSON结果的缓冲区
     * @param json_buf_len 缓冲区长度
     * @return 0表示成功，非0表示失败
     * 
     * 错误代码:
     * - -100: SDK未初始化
     * - -8: 参数错误
     * - -9: 缓冲区太小
     * - -7: 分析过程异常
     * 
     * 返回的JSON格式:
     * {
     *   "code": 0,
     *   "msg": "success",
     *   "license_info": {
     *     "status": "valid",
     *     "expires": "2024-12-31",
     *     "type": "commercial"
     *   },
     *   "faces": [
     *     {
     *       "bbox": {"x": 100, "y": 50, "width": 200, "height": 250},
     *       "pose": {"yaw": 5.2, "pitch": -2.1, "roll": 1.5},
     *       "metrics": {"sharpness": 45.6, "brightness": 128.3, "has_mask": false, "distance": 50.0},
     *       "quality_scores": {"sharpness_score": 85.2, "brightness_score": 92.1, "contrast_score": 78.5},
     *       "stability": {"is_stable": true, "motion_blur": 15.3}
     *     }
     *   ]
     * }
     */
    FAST_FACE_API int analyze_frame(const unsigned char* bgr_data, int width, int height, char* result_json, int json_buf_len);

    /**
     * @brief 释放SDK资源
     * 
     * 使用完毕后必须调用此函数释放资源，避免内存泄漏。
     */
    FAST_FACE_API void sdk_release();
} 