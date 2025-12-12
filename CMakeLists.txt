cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME MappingEngine)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/MapHandler
    ${CMAKE_SOURCE_DIR}/include/MapWidget
)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Quick
    Location
    Positioning
    QuickWidgets
)

set(SRCS
    src/main.cpp
    src/MapHandler/MapHandler.cpp
    src/MapWidget/MapWidget.cpp
    
)

set(HDRS
    include/MapHandler/MapHandler.h
    include/MapWidget/MapWidget.h
)

set(QRCS
    resources/map.qrc
)

add_executable(${PROJECT_NAME}
    ${SRCS}
    ${HDRS}
    ${QRCS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Location
    Qt${QT_VERSION_MAJOR}::Positioning
    Qt${QT_VERSION_MAJOR}::QuickWidgets
)

target_include_directories(MappingEngine PRIVATE ${CMAKE_SOURCE_DIR}/include)

# # Optional: better include directories (modern CMake)
# target_include_directories(${PROJECT_NAME} PRIVATE
#     ${CMAKE_SOURCE_DIR}/include
#     ${CMAKE_SOURCE_DIR}/include/MapHandler
# )


#================ TEST ======================


# enable_testing()

# include(FetchContent)
# FetchContent_Declare(
#     googletest
#     URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
# )

