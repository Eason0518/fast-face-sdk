#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <vector>
#include <random>
#include <algorithm>

// 许可证生成器
class LicenseGenerator {
private:
    static const std::string CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    
    // 生成随机字符串
    static std::string generateRandomString(int length) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, CHARS.length() - 1);
        
        std::string result;
        result.reserve(length);
        
        for (int i = 0; i < length; ++i) {
            result += CHARS[dis(gen)];
        }
        
        return result;
    }
    
    // 生成许可证密钥
    static std::string generateLicenseKey(const std::string& prefix, int year, int month, int day) {
        std::stringstream ss;
        ss << prefix << "_" << year << "_";
        ss << std::setfill('0') << std::setw(2) << month;
        ss << std::setfill('0') << std::setw(2) << day;
        ss << "_" << generateRandomString(8);
        return ss.str();
    }
    
public:
    // 生成商业许可证
    static std::string generateCommercialLicense(int year, int month, int day) {
        return generateLicenseKey("FAST_FACE_COMMERCIAL", year, month, day);
    }
    
    // 生成试用许可证
    static std::string generateTrialLicense() {
        return "FAST_FACE_TRIAL_" + generateRandomString(12);
    }
    
    // 生成临时许可证
    static std::string generateTemporaryLicense(int days) {
        std::time_t now = std::time(nullptr);
        std::time_t expiry = now + (days * 24 * 3600);
        
        std::tm* tm = std::localtime(&expiry);
        return generateLicenseKey("FAST_FACE_TEMP", 
                                tm->tm_year + 1900, 
                                tm->tm_mon + 1, 
                                tm->tm_mday);
    }
    
    // 验证许可证格式
    static bool validateLicenseFormat(const std::string& license) {
        if (license.length() < 10 || license.length() > 64) {
            return false;
        }
        
        // 检查是否包含非法字符
        for (char c : license) {
            if (!std::isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
        
        return true;
    }
    
    // 解析许可证信息
    static void parseLicenseInfo(const std::string& license) {
        std::cout << "许可证: " << license << std::endl;
        
        if (license.find("COMMERCIAL") != std::string::npos) {
            std::cout << "类型: 商业许可证" << std::endl;
        } else if (license.find("TRIAL") != std::string::npos) {
            std::cout << "类型: 试用许可证" << std::endl;
        } else if (license.find("TEMP") != std::string::npos) {
            std::cout << "类型: 临时许可证" << std::endl;
        } else {
            std::cout << "类型: 未知" << std::endl;
        }
        
        // 尝试解析日期
        size_t pos = license.find_last_of('_');
        if (pos != std::string::npos) {
            std::string date_part = license.substr(pos + 1);
            if (date_part.length() >= 8) {
                std::string year = date_part.substr(0, 4);
                std::string month = date_part.substr(4, 2);
                std::string day = date_part.substr(6, 2);
                
                std::cout << "过期日期: " << year << "-" << month << "-" << day << std::endl;
            }
        }
    }
};

// 许可证管理器
class LicenseManager {
private:
    std::vector<std::string> generated_licenses;
    
public:
    // 生成新许可证
    void generateLicense(const std::string& type, int days = 0) {
        std::string license;
        
        if (type == "commercial") {
            std::time_t now = std::time(nullptr);
            std::tm* tm = std::localtime(&now);
            license = LicenseGenerator::generateCommercialLicense(
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday
            );
        } else if (type == "trial") {
            license = LicenseGenerator::generateTrialLicense();
        } else if (type == "temporary") {
            license = LicenseGenerator::generateTemporaryLicense(days);
        } else {
            std::cout << "未知许可证类型: " << type << std::endl;
            return;
        }
        
        generated_licenses.push_back(license);
        std::cout << "生成的许可证: " << license << std::endl;
    }
    
    // 批量生成许可证
    void generateBatchLicenses(const std::string& type, int count, int days = 0) {
        std::cout << "批量生成 " << count << " 个 " << type << " 许可证:" << std::endl;
        for (int i = 0; i < count; ++i) {
            generateLicense(type, days);
        }
    }
    
    // 验证许可证
    void validateLicense(const std::string& license) {
        std::cout << "\n验证许可证: " << license << std::endl;
        
        if (!LicenseGenerator::validateLicenseFormat(license)) {
            std::cout << "❌ 许可证格式无效" << std::endl;
            return;
        }
        
        std::cout << "✅ 许可证格式有效" << std::endl;
        LicenseGenerator::parseLicenseInfo(license);
    }
    
    // 显示所有生成的许可证
    void showGeneratedLicenses() {
        if (generated_licenses.empty()) {
            std::cout << "没有生成的许可证" << std::endl;
            return;
        }
        
        std::cout << "\n已生成的许可证列表:" << std::endl;
        for (size_t i = 0; i < generated_licenses.size(); ++i) {
            std::cout << (i + 1) << ". " << generated_licenses[i] << std::endl;
        }
    }
    
    // 保存许可证到文件
    void saveLicensesToFile(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "无法打开文件: " << filename << std::endl;
            return;
        }
        
        file << "# FastFaceSDK 许可证列表" << std::endl;
        file << "# 生成时间: " << std::time(nullptr) << std::endl;
        file << std::endl;
        
        for (const auto& license : generated_licenses) {
            file << license << std::endl;
        }
        
        file.close();
        std::cout << "许可证已保存到: " << filename << std::endl;
    }
};

// 显示帮助信息
void showHelp() {
    std::cout << "FastFaceSDK 许可证管理器" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "用法:" << std::endl;
    std::cout << "  generate <type> [count] [days]  - 生成许可证" << std::endl;
    std::cout << "  validate <license>              - 验证许可证" << std::endl;
    std::cout << "  list                            - 显示生成的许可证" << std::endl;
    std::cout << "  save <filename>                 - 保存许可证到文件" << std::endl;
    std::cout << "  help                            - 显示此帮助" << std::endl;
    std::cout << "  quit                            - 退出程序" << std::endl;
    std::cout << std::endl;
    std::cout << "许可证类型:" << std::endl;
    std::cout << "  commercial                      - 商业许可证" << std::endl;
    std::cout << "  trial                           - 试用许可证" << std::endl;
    std::cout << "  temporary                       - 临时许可证" << std::endl;
    std::cout << std::endl;
    std::cout << "示例:" << std::endl;
    std::cout << "  generate commercial             - 生成1个商业许可证" << std::endl;
    std::cout << "  generate trial 5                - 生成5个试用许可证" << std::endl;
    std::cout << "  generate temporary 10 30        - 生成10个30天临时许可证" << std::endl;
    std::cout << "  validate FAST_FACE_2024_KEY     - 验证许可证" << std::endl;
    std::cout << "  save licenses.txt               - 保存许可证到文件" << std::endl;
}

int main() {
    LicenseManager manager;
    
    std::cout << "FastFaceSDK 许可证管理器 v1.0" << std::endl;
    std::cout << "输入 'help' 查看帮助信息" << std::endl;
    std::cout << std::endl;
    
    std::string command;
    while (true) {
        std::cout << "license> ";
        std::getline(std::cin, command);
        
        if (command.empty()) continue;
        
        // 解析命令
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "quit" || cmd == "exit") {
            break;
        } else if (cmd == "help") {
            showHelp();
        } else if (cmd == "generate") {
            std::string type;
            int count = 1;
            int days = 0;
            
            iss >> type;
            iss >> count;
            iss >> days;
            
            if (type.empty()) {
                std::cout << "请指定许可证类型" << std::endl;
                continue;
            }
            
            if (count <= 0) count = 1;
            
            if (count == 1) {
                manager.generateLicense(type, days);
            } else {
                manager.generateBatchLicenses(type, count, days);
            }
        } else if (cmd == "validate") {
            std::string license;
            iss >> license;
            
            if (license.empty()) {
                std::cout << "请指定许可证" << std::endl;
                continue;
            }
            
            manager.validateLicense(license);
        } else if (cmd == "list") {
            manager.showGeneratedLicenses();
        } else if (cmd == "save") {
            std::string filename;
            iss >> filename;
            
            if (filename.empty()) {
                std::cout << "请指定文件名" << std::endl;
                continue;
            }
            
            manager.saveLicensesToFile(filename);
        } else {
            std::cout << "未知命令: " << cmd << std::endl;
            std::cout << "输入 'help' 查看帮助信息" << std::endl;
        }
    }
    
    std::cout << "再见！" << std::endl;
    return 0;
} 