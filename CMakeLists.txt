cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME MappingEngine)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/MapHandler
    ${CMAKE_SOURCE_DIR}/include/MapWidget
    ${CMAKE_SOURCE_DIR}/include/MapDefines
)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Quick
    Location
    Positioning
    QuickWidgets
    
)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GDAL REQUIRED gdal)


set(SRCS
    src/main.cpp
    src/MapHandler/MapHandler.cpp
    src/MapWidget/MapWidget.cpp
    src/MapHandler/Feature.cpp
)

set(HDRS
    include/MapHandler/MapHandler.h
    include/MapWidget/MapWidget.h
    include/MapDefines/MapDefines.h
    include/MapHandler/Geometry.h
    include/MapHandler/Feature.h
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
add_subdirectory(Plugin)  # ← добавь эту строку, если плагин в папке plugin