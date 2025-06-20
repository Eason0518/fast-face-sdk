cmake_minimum_required(VERSION 3.15)
project(FastFaceSDK VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找OpenCV
find_package(OpenCV REQUIRED COMPONENTS core imgproc objdetect face video imgcodecs highgui)

# 查找nlohmann/json
find_package(nlohmann_json 3.9.0 REQUIRED)

# 设置包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# 创建SDK库
add_library(fast_face_sdk SHARED
    src/fast_face_sdk.cpp
)

# 链接依赖库
target_link_libraries(fast_face_sdk
    ${OpenCV_LIBS}
    nlohmann_json::nlohmann_json
)

# 设置库的属性
set_target_properties(fast_face_sdk PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    OUTPUT_NAME "fast_face_sdk"
)

# 添加子目录
add_subdirectory(src)
add_subdirectory(example)

# 创建测试程序
add_executable(test_sdk test_sdk.cpp)
target_link_libraries(test_sdk
    fast_face_sdk
    ${OpenCV_LIBS}
    nlohmann_json::nlohmann_json
)

# 创建许可证管理工具
add_executable(license_manager license_manager.cpp)
target_link_libraries(license_manager
    ${OpenCV_LIBS}
    nlohmann_json::nlohmann_json
)

# 安装规则
install(TARGETS fast_face_sdk
    EXPORT FastFaceSDKTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(TARGETS test_sdk license_manager
    RUNTIME DESTINATION bin
)

# 导出目标
install(EXPORT FastFaceSDKTargets
    FILE FastFaceSDKTargets.cmake
    NAMESPACE FastFaceSDK::
    DESTINATION lib/cmake/FastFaceSDK
)

# 创建配置文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    FastFaceSDKConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/FastFaceSDKConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/FastFaceSDKConfig.cmake
    @ONLY
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/FastFaceSDKConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/FastFaceSDKConfigVersion.cmake
    DESTINATION lib/cmake/FastFaceSDK
)

# 打印配置信息
message(STATUS "FastFaceSDK Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  OpenCV Version: ${OpenCV_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Install Prefix: ${CMAKE_INSTALL_PREFIX}") 