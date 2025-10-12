cmake_minimum_required(VERSION 3.16)
project(QtOpenCVDemo)

# C++ 标准配置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 生成编译命令数据库（供 IDE 索引）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置默认构建类型（若未指定，默认 Debug）
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# 定义 Debug 模式选项
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -Wall -DDEBUG")

# 定义 Release 模式选项
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG -Wall")

# 可选：明确指定支持的构建类型（避免用户输入无效类型）
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")

# Qt 自动处理（MOC/UIC/RCC）
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 查找依赖（指定最低版本和必要组件）
find_package(Qt5 5.12 COMPONENTS Widgets REQUIRED)
find_package(OpenCV 4.0 REQUIRED COMPONENTS core imgproc highgui)

# 源文件管理
set(SOURCES
  src/main.cpp
  src/videowidget.cpp
  src/imageprocessor.cpp
  src/mainwindow.cpp
  src/videoplayer.cpp
)
set(HEADERS
  include/videowidget.h
  include/imageprocessor.h
  include/mainwindow.h
  include/videoplayer.h
)

# 输出目录配置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CONFIG} CONFIG_UPPER)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/bin/${CONFIG})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${CONFIG})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${CMAKE_BINARY_DIR}/lib/${CONFIG})
endforeach()

# 定义资源目录（相对于 CMakeLists.txt 的路径）
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
# 目标目录（与可执行文件同目录）
set(DEST_ASSETS_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)

# 复制 assets 目录到输出目录（每次构建时检查更新）
add_custom_command(
  OUTPUT ${DEST_ASSETS_DIR}
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_DIR} ${DEST_ASSETS_DIR}
  MAIN_DEPENDENCY ${ASSETS_DIR}  # 资源目录变化时重新复制
  COMMENT "Copying assets to output directory..."
)

# 创建一个目标，确保复制操作在编译前执行
add_custom_target(
  copy_assets ALL
  DEPENDS ${DEST_ASSETS_DIR}
)

# 生成可执行文件
add_executable(qt_opencv_demo
  ${SOURCES}
  ${HEADERS}
)

# 让可执行文件依赖于资源复制目标（确保先复制再编译）
add_dependencies(qt_opencv_demo copy_assets)

# 目标属性配置（Windows 隐藏控制台）
if(WIN32)
  set_target_properties(qt_opencv_demo PROPERTIES WIN32_EXECUTABLE ON)
endif()

# 包含目录（目标专属）
target_include_directories(qt_opencv_demo
  PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 链接依赖
target_link_libraries(qt_opencv_demo
  PRIVATE
    Qt5::Widgets
    ${OpenCV_LIBS}
)

# 编译警告
target_compile_options(qt_opencv_demo PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)