#pragma once

// SDK版本信息
#define FAST_FACE_SDK_VERSION_MAJOR 1
#define FAST_FACE_SDK_VERSION_MINOR 0
#define FAST_FACE_SDK_VERSION_PATCH 0
#define FAST_FACE_SDK_VERSION_STRING "1.0.0"

// 密钥验证配置
namespace FastFaceConfig {
    // 授权密钥（请修改为您的实际密钥）
    constexpr const char* VALID_LICENSE_KEY = "FAST_FACE_2024_LICENSE_KEY_12345";
    
    // 密钥验证相关
    constexpr int MAX_KEY_LENGTH = 64;
    constexpr int MIN_KEY_LENGTH = 10;
    
    // 试用模式配置
    constexpr bool ENABLE_TRIAL_MODE = false;  // 是否启用试用模式
    constexpr int TRIAL_DAYS = 7;              // 试用天数
    
    // 人脸检测参数
    constexpr double FACE_DETECTION_SCALE_FACTOR = 1.1;
    constexpr int FACE_DETECTION_MIN_NEIGHBORS = 3;
    constexpr int FACE_DETECTION_MIN_SIZE = 50;
    
    // 稳定性检测参数
    constexpr int STABLE_FRAMES_THRESHOLD = 3;
    constexpr double STABILITY_THRESHOLD = 0.1;
    
    // 图像质量参数
    constexpr int SHARPNESS_THRESHOLD = 50;
    constexpr int MOTION_BLUR_THRESHOLD = 100;
    constexpr double BRIGHTNESS_MIN = 30.0;
    constexpr double BRIGHTNESS_MAX = 220.0;
    constexpr double BRIGHTNESS_OPTIMAL = 127.5;
    
    // 距离估计参数
    constexpr double AVG_PUPIL_DISTANCE_MM = 63.0;
    constexpr double DEFAULT_DISTANCE_CM = 50.0;
    
    // 口罩检测参数
    constexpr double BLUE_MASK_RATIO_THRESHOLD = 0.1;
    constexpr double WHITE_MASK_RATIO_THRESHOLD = 0.3;
    
    // 头部姿态参数
    constexpr double POSE_WARNING_THRESHOLD = 30.0;
    
    // 性能参数
    constexpr int MAX_IMAGE_WIDTH = 1920;
    constexpr int MAX_IMAGE_HEIGHT = 1080;
    constexpr int MAX_FACES_PER_FRAME = 10;
    constexpr int MAX_JSON_BUFFER_SIZE = 4096;
    
    // 历史记录参数
    constexpr int FACE_HISTORY_SIZE = 5;
}

// 错误代码定义
namespace FastFaceError {
    constexpr int SUCCESS = 0;
    constexpr int INVALID_KEY = -1;
    constexpr int KEY_EXPIRED = -2;
    constexpr int TRIAL_EXPIRED = -3;
    constexpr int FACE_CASCADE_LOAD_FAILED = -4;
    constexpr int EYE_CASCADE_LOAD_FAILED = -5;
    constexpr int INIT_EXCEPTION = -6;
    constexpr int ANALYSIS_EXCEPTION = -7;
    constexpr int NOT_INITIALIZED = -100;
    constexpr int INVALID_PARAMETERS = -8;
    constexpr int BUFFER_TOO_SMALL = -9;
} 