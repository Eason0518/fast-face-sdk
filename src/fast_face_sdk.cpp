#include "../include/fast_face_sdk.h"
#include <string>
#include <mutex>
#include <vector>
#include <deque>
#include <memory>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <nlohmann/json.hpp>
// #include <onnxruntime_cxx_api.h> // 需要ONNX Runtime头文件

// 全局变量
static bool g_activated = false;
static std::string g_current_license_key;
static std::mutex g_mutex;

// 许可证信息
struct LicenseInfo {
    std::string key;
    std::string status;
    std::string type;
    std::string expires;
    bool is_valid;
    bool is_trial;
    std::time_t trial_start;
    std::time_t trial_end;
};

static LicenseInfo g_license_info;

// 模型相关
static cv::CascadeClassifier g_face_cascade;
static cv::CascadeClassifier g_eye_cascade;
static cv::Ptr<cv::face::Facemark> g_facemark;
static cv::dnn::Net g_mask_model;

// 历史记录
static std::deque<cv::Rect> g_face_history;
static cv::Mat g_prev_gray;

// 常量定义
const int STABLE_FRAMES_THRESHOLD = 3;
const int SHARPNESS_THRESHOLD = 50;
const int MOTION_BLUR_THRESHOLD = 100;
const double AVG_PUPIL_DISTANCE_MM = 63.0;

// 简单的密钥验证函数
bool validate_license_key(const std::string& key) {
    if (key.empty() || key.length() < FastFaceConfig::MIN_KEY_LENGTH || 
        key.length() > FastFaceConfig::MAX_KEY_LENGTH) {
        return false;
    }
    
    // 检查是否为有效密钥
    return key == FastFaceConfig::VALID_LICENSE_KEY;
}

// 解析密钥中的过期时间（如果包含）
std::time_t parse_expiry_date(const std::string& key) {
    // 这里可以实现更复杂的密钥解析逻辑
    // 当前版本使用固定过期时间
    std::tm tm = {};
    tm.tm_year = 2024 - 1900;  // 2024年
    tm.tm_mon = 11;            // 12月
    tm.tm_mday = 31;           // 31日
    return std::mktime(&tm);
}

// 检查密钥是否过期
bool is_key_expired(const std::string& key) {
    std::time_t expiry = parse_expiry_date(key);
    std::time_t now = std::time(nullptr);
    return now > expiry;
}

// 检查试用期是否过期
bool is_trial_expired() {
    if (!g_license_info.is_trial) return false;
    
    std::time_t now = std::time(nullptr);
    return now > g_license_info.trial_end;
}

// 初始化试用期
void init_trial_period() {
    if (!FastFaceConfig::ENABLE_TRIAL_MODE) return;
    
    g_license_info.is_trial = true;
    g_license_info.trial_start = std::time(nullptr);
    g_license_info.trial_end = g_license_info.trial_start + (FastFaceConfig::TRIAL_DAYS * 24 * 3600);
    g_license_info.status = "trial";
    g_license_info.type = "trial";
    
    // 计算过期时间字符串
    std::tm* tm = std::localtime(&g_license_info.trial_end);
    std::stringstream ss;
    ss << std::put_time(tm, "%Y-%m-%d");
    g_license_info.expires = ss.str();
}

// 计算图像清晰度
double calculate_sharpness(const cv::Mat& image) {
    cv::Mat gray, laplacian;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::Laplacian(gray, laplacian, CV_64F);
    cv::Scalar mean, stddev;
    cv::meanStdDev(laplacian, mean, stddev);
    return stddev.val[0] * stddev.val[0];
}

// 检测运动模糊
double detect_motion_blur(const cv::Mat& current_gray, const cv::Mat& prev_gray) {
    if (prev_gray.empty()) return 0.0;
    
    try {
        cv::Mat flow;
        cv::calcOpticalFlowFarneback(prev_gray, current_gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
        
        std::vector<cv::Mat> flow_parts;
        cv::split(flow, flow_parts);
        cv::Mat magnitude, angle;
        cv::cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle);
        
        return cv::mean(magnitude)[0];
    } catch (...) {
        return 0.0;
    }
}

// 增强人脸亮度
cv::Mat enhance_face_brightness(const cv::Mat& face_roi, double gamma = 1.5) {
    cv::Mat lab, enhanced;
    cv::cvtColor(face_roi, lab, cv::COLOR_BGR2LAB);
    
    std::vector<cv::Mat> lab_planes(3);
    cv::split(lab, lab_planes);
    
    // 应用CLAHE
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(3.0, cv::Size(8, 8));
    clahe->apply(lab_planes[0], lab_planes[0]);
    
    // 应用gamma校正
    cv::Mat l_gamma;
    lab_planes[0].convertTo(l_gamma, CV_64F);
    l_gamma = cv::pow(l_gamma / 255.0, 1.0 / gamma) * 255;
    l_gamma.convertTo(lab_planes[0], CV_8U);
    
    cv::merge(lab_planes, lab);
    cv::cvtColor(lab, enhanced, cv::COLOR_LAB2BGR);
    return enhanced;
}

// 估计头部姿态
std::tuple<double, double, double> estimate_pose(const std::vector<cv::Point2f>& landmarks, const cv::Size& frame_shape) {
    try {
        // 简化的头部姿态估计，使用眼睛和鼻子关键点
        if (landmarks.size() < 6) return {0.0, 0.0, 0.0};
        
        // 3D模型点（简化的人脸模型）
        std::vector<cv::Point3f> model_points = {
            cv::Point3f(0.0f, 0.0f, 0.0f),           // 鼻子
            cv::Point3f(0.0f, -330.0f, -65.0f),      // 下巴
            cv::Point3f(-165.0f, 170.0f, -135.0f),   // 左眼
            cv::Point3f(165.0f, 170.0f, -135.0f),    // 右眼
            cv::Point3f(-150.0f, -150.0f, -125.0f),  // 左嘴角
            cv::Point3f(150.0f, -150.0f, -125.0f)    // 右嘴角
        };
        
        // 2D图像点
        std::vector<cv::Point2f> image_points = {
            landmarks[0], landmarks[1], landmarks[2],
            landmarks[3], landmarks[4], landmarks[5]
        };
        
        // 相机内参
        double focal_length = frame_shape.width;
        cv::Point2d center(frame_shape.width / 2, frame_shape.height / 2);
        cv::Mat camera_matrix = (cv::Mat_<double>(3, 3) << 
            focal_length, 0, center.x,
            0, focal_length, center.y,
            0, 0, 1);
        
        cv::Mat dist_coeffs = cv::Mat::zeros(4, 1, CV_64F);
        cv::Mat rotation_vector, translation_vector;
        
        cv::solvePnP(model_points, image_points, camera_matrix, dist_coeffs, 
                    rotation_vector, translation_vector);
        
        // 转换为欧拉角
        cv::Mat rotation_matrix;
        cv::Rodrigues(rotation_vector, rotation_matrix);
        
        // 简化的欧拉角计算
        double yaw = atan2(rotation_matrix.at<double>(2, 1), rotation_matrix.at<double>(2, 2)) * 180.0 / CV_PI;
        double pitch = atan2(-rotation_matrix.at<double>(2, 0), 
                           sqrt(rotation_matrix.at<double>(2, 1) * rotation_matrix.at<double>(2, 1) + 
                                rotation_matrix.at<double>(2, 2) * rotation_matrix.at<double>(2, 2))) * 180.0 / CV_PI;
        double roll = atan2(rotation_matrix.at<double>(1, 0), rotation_matrix.at<double>(0, 0)) * 180.0 / CV_PI;
        
        return {yaw, pitch, roll};
    } catch (...) {
        return {0.0, 0.0, 0.0};
    }
}

// 分析人脸特征
struct FaceMetrics {
    double sharpness;
    double brightness;
    bool has_mask;
    double distance;
};

FaceMetrics analyze_face(const cv::Mat& frame, const cv::Rect& face_roi) {
    FaceMetrics metrics;
    
    // 提取人脸区域
    cv::Mat face_img = frame(face_roi);
    
    // 计算清晰度
    metrics.sharpness = calculate_sharpness(face_img);
    
    // 计算亮度
    cv::Mat gray;
    cv::cvtColor(face_img, gray, cv::COLOR_BGR2GRAY);
    metrics.brightness = cv::mean(gray)[0];
    
    // 简化的口罩检测（基于颜色和形状）
    metrics.has_mask = false;
    try {
        cv::Mat hsv;
        cv::cvtColor(face_img, hsv, cv::COLOR_BGR2HSV);
        
        // 检测蓝色口罩（HSV范围）
        cv::Mat blue_mask;
        cv::inRange(hsv, cv::Scalar(100, 50, 50), cv::Scalar(130, 255, 255), blue_mask);
        
        // 检测白色口罩
        cv::Mat white_mask;
        cv::inRange(hsv, cv::Scalar(0, 0, 200), cv::Scalar(180, 30, 255), white_mask);
        
        double blue_ratio = cv::countNonZero(blue_mask) / (double)(blue_mask.rows * blue_mask.cols);
        double white_ratio = cv::countNonZero(white_mask) / (double)(white_mask.rows * white_mask.cols);
        
        metrics.has_mask = (blue_ratio > FastFaceConfig::BLUE_MASK_RATIO_THRESHOLD || 
                           white_ratio > FastFaceConfig::WHITE_MASK_RATIO_THRESHOLD);
    } catch (...) {
        metrics.has_mask = false;
    }
    
    // 简化的距离估计
    metrics.distance = FastFaceConfig::DEFAULT_DISTANCE_CM;
    
    return metrics;
}

// 判断人脸是否稳定
bool is_face_stable(const cv::Rect& current_bbox, const std::deque<cv::Rect>& history) {
    if (history.size() < FastFaceConfig::STABLE_FRAMES_THRESHOLD) return false;
    
    double total_movement = 0.0;
    for (const auto& prev_bbox : history) {
        double dx = abs(current_bbox.x - prev_bbox.x) / (double)current_bbox.width;
        double dy = abs(current_bbox.y - prev_bbox.y) / (double)current_bbox.height;
        total_movement += dx + dy;
    }
    
    return (total_movement / history.size()) < FastFaceConfig::STABILITY_THRESHOLD;
}

// 评分函数
double sharpness_score(double sharpness_value) {
    return std::min(100.0, std::max(0.0, sharpness_value / 10.0));
}

double brightness_score(double brightness_value) {
    if (brightness_value < FastFaceConfig::BRIGHTNESS_MIN) return 0.0;
    if (brightness_value > FastFaceConfig::BRIGHTNESS_MAX) return 0.0;
    return 100.0 - abs(brightness_value - FastFaceConfig::BRIGHTNESS_OPTIMAL) / FastFaceConfig::BRIGHTNESS_OPTIMAL * 100.0;
}

double contrast_score(const cv::Mat& gray_image) {
    cv::Scalar mean, stddev;
    cv::meanStdDev(gray_image, mean, stddev);
    double std_val = stddev.val[0];
    
    if (std_val < 20) return 0.0;
    if (std_val > 100) return 100.0;
    return (std_val - 20) / 80.0 * 100.0;
}

extern "C" {

const char* get_sdk_version() {
    return FAST_FACE_SDK_VERSION_STRING;
}

int verify_license(const char* license_key) {
    if (!license_key) return FastFaceError::INVALID_PARAMETERS;
    
    std::string key(license_key);
    
    // 检查密钥格式
    if (!validate_license_key(key)) {
        return FastFaceError::INVALID_KEY;
    }
    
    // 检查密钥是否过期
    if (is_key_expired(key)) {
        return FastFaceError::KEY_EXPIRED;
    }
    
    return FastFaceError::SUCCESS;
}

int get_license_info(char* license_info, int info_buf_len) {
    if (!license_info || info_buf_len <= 0) return FastFaceError::INVALID_PARAMETERS;
    
    nlohmann::json info;
    info["key"] = g_license_info.key;
    info["status"] = g_license_info.status;
    info["type"] = g_license_info.type;
    info["expires"] = g_license_info.expires;
    info["is_valid"] = g_license_info.is_valid;
    info["is_trial"] = g_license_info.is_trial;
    
    if (g_license_info.is_trial) {
        std::time_t now = std::time(nullptr);
        int remaining_days = (g_license_info.trial_end - now) / (24 * 3600);
        info["trial_remaining_days"] = std::max(0, remaining_days);
    }
    
    std::string json_str = info.dump();
    if ((int)json_str.size() >= info_buf_len) return FastFaceError::BUFFER_TOO_SMALL;
    
    strcpy(license_info, json_str.c_str());
    return FastFaceError::SUCCESS;
}

int sdk_init(const char* license_key) {
    std::lock_guard<std::mutex> lock(g_mutex);
    
    if (!license_key) return FastFaceError::INVALID_PARAMETERS;
    
    std::string key(license_key);
    
    // 验证密钥
    int verify_result = verify_license(license_key);
    if (verify_result != FastFaceError::SUCCESS) {
        return verify_result;
    }
    
    // 检查试用期
    if (FastFaceConfig::ENABLE_TRIAL_MODE && is_trial_expired()) {
        return FastFaceError::TRIAL_EXPIRED;
    }
    
    try {
        // 加载OpenCV人脸检测模型
        if (!g_face_cascade.load(cv::data::haarcascades + "haarcascade_frontalface_alt2.xml")) {
            return FastFaceError::FACE_CASCADE_LOAD_FAILED;
        }
        
        if (!g_eye_cascade.load(cv::data::haarcascades + "haarcascade_eye.xml")) {
            return FastFaceError::EYE_CASCADE_LOAD_FAILED;
        }
        
        // 初始化Facemark
        g_facemark = cv::face::createFacemarkLBF();
        std::string facemark_model = cv::data::face + "lbfmodel.yaml";
        if (!g_facemark->loadModel(facemark_model)) {
            // 如果加载失败，使用简化版本
            g_facemark = nullptr;
        }
        
        // 更新许可证信息
        g_current_license_key = key;
        g_license_info.key = key;
        g_license_info.is_valid = true;
        g_license_info.status = "valid";
        g_license_info.type = "commercial";
        
        // 解析过期时间
        std::time_t expiry = parse_expiry_date(key);
        std::tm* tm = std::localtime(&expiry);
        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d");
        g_license_info.expires = ss.str();
        
        // 如果是试用模式，初始化试用期
        if (FastFaceConfig::ENABLE_TRIAL_MODE && key == "TRIAL_KEY") {
            init_trial_period();
        }
        
        g_activated = true;
        return FastFaceError::SUCCESS;
    } catch (...) {
        g_activated = false;
        return FastFaceError::INIT_EXCEPTION;
    }
}

int analyze_frame(const unsigned char* bgr_data, int width, int height, char* result_json, int json_buf_len) {
    std::lock_guard<std::mutex> lock(g_mutex);
    
    if (!g_activated) return FastFaceError::NOT_INITIALIZED;
    if (!bgr_data || width <= 0 || height <= 0 || !result_json) return FastFaceError::INVALID_PARAMETERS;
    
    // 检查许可证状态
    if (!g_license_info.is_valid) return FastFaceError::INVALID_KEY;
    if (is_key_expired(g_license_info.key)) return FastFaceError::KEY_EXPIRED;
    if (g_license_info.is_trial && is_trial_expired()) return FastFaceError::TRIAL_EXPIRED;
    
    try {
        // 创建OpenCV Mat
        cv::Mat frame(height, width, CV_8UC3, (void*)bgr_data);
        cv::Mat gray;
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        
        // 运动模糊检测
        double motion_blur = detect_motion_blur(gray, g_prev_gray);
        g_prev_gray = gray.clone();
        
        // 人脸检测
        std::vector<cv::Rect> faces;
        g_face_cascade.detectMultiScale(gray, faces, 
                                       FastFaceConfig::FACE_DETECTION_SCALE_FACTOR,
                                       FastFaceConfig::FACE_DETECTION_MIN_NEIGHBORS, 
                                       0, cv::Size(FastFaceConfig::FACE_DETECTION_MIN_SIZE, 
                                                  FastFaceConfig::FACE_DETECTION_MIN_SIZE));
        
        nlohmann::json result;
        result["code"] = FastFaceError::SUCCESS;
        result["msg"] = "success";
        
        // 添加许可证信息
        nlohmann::json license_info;
        license_info["status"] = g_license_info.status;
        license_info["expires"] = g_license_info.expires;
        license_info["type"] = g_license_info.type;
        if (g_license_info.is_trial) {
            std::time_t now = std::time(nullptr);
            int remaining_days = (g_license_info.trial_end - now) / (24 * 3600);
            license_info["trial_remaining_days"] = std::max(0, remaining_days);
        }
        result["license_info"] = license_info;
        
        result["faces"] = nlohmann::json::array();
        
        for (const auto& face_rect : faces) {
            // 更新历史记录
            g_face_history.push_back(face_rect);
            if (g_face_history.size() > FastFaceConfig::FACE_HISTORY_SIZE) {
                g_face_history.pop_front();
            }
            
            // 判断稳定性
            bool is_stable = is_face_stable(face_rect, g_face_history);
            
            // 分析人脸特征
            FaceMetrics metrics = analyze_face(frame, face_rect);
            
            // 头部姿态估计（简化版）
            double yaw = 0.0, pitch = 0.0, roll = 0.0;
            
            // 尝试使用Facemark进行关键点检测
            if (g_facemark) {
                std::vector<cv::Rect> face_rects = {face_rect};
                std::vector<std::vector<cv::Point2f>> landmarks;
                
                if (g_facemark->fit(frame, face_rects, landmarks) && !landmarks.empty()) {
                    std::tie(yaw, pitch, roll) = estimate_pose(landmarks[0], frame.size());
                }
            }
            
            // 构建人脸结果
            nlohmann::json face_result;
            face_result["bbox"] = {
                {"x", face_rect.x},
                {"y", face_rect.y},
                {"width", face_rect.width},
                {"height", face_rect.height}
            };
            
            face_result["pose"] = {
                {"yaw", yaw},
                {"pitch", pitch},
                {"roll", roll}
            };
            
            face_result["metrics"] = {
                {"sharpness", metrics.sharpness},
                {"brightness", metrics.brightness},
                {"has_mask", metrics.has_mask},
                {"distance", metrics.distance}
            };
            
            face_result["quality_scores"] = {
                {"sharpness_score", sharpness_score(metrics.sharpness)},
                {"brightness_score", brightness_score(metrics.brightness)},
                {"contrast_score", contrast_score(gray)}
            };
            
            face_result["stability"] = {
                {"is_stable", is_stable},
                {"motion_blur", motion_blur}
            };
            
            result["faces"].push_back(face_result);
        }
        
        // 转换为字符串
        std::string json_str = result.dump();
        if ((int)json_str.size() >= json_buf_len) return FastFaceError::BUFFER_TOO_SMALL;
        
        strcpy(result_json, json_str.c_str());
        return FastFaceError::SUCCESS;
        
    } catch (...) {
        return FastFaceError::ANALYSIS_EXCEPTION;
    }
}

void sdk_release() {
    std::lock_guard<std::mutex> lock(g_mutex);
    
    g_activated = false;
    g_current_license_key.clear();
    g_license_info = LicenseInfo();
    g_face_cascade = cv::CascadeClassifier();
    g_eye_cascade = cv::CascadeClassifier();
    g_facemark = nullptr;
    g_mask_model = cv::dnn::Net();
    g_face_history.clear();
    g_prev_gray = cv::Mat();
}

} // extern "C" 