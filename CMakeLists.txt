cmake_minimum_required(VERSION 3.16)
project(new_final VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置Qt路径
set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt@6")

# 设置UI文件目录
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

# 查找Qt包
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)
# 源文件
set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/loginwindow.cpp
        src/parkingmanager.cpp
        src/car.cpp
        src/addcardialog.cpp    # 添加这行
)

# 头文件
set(HEADERS
        include/mainwindow.h
        include/loginwindow.h
        include/parkingmanager.h
        include/car.h
        include/addcardialog.h  # 添加这行
        include/mainwindow.h
)
# UI文件
set(UI_FILES
        ui/loginwindow.ui
        ui/mainwindow.ui
        src/addcardialog.cpp
)

add_executable(new_final
        ${SOURCES}
        ${HEADERS}
        ${UI_FILES}
        src/addcardialog.cpp
)

# 设置包含目录
target_include_directories(new_final PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}
)

# 链接Qt库
target_link_libraries(new_final PRIVATE
        Qt6::Core
        Qt6::Widgets
)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})