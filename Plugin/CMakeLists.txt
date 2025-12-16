cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME MapPlugin)
project(${PROJECT_NAME} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


message("Building " ${PROJECT_NAME})

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Location Network Positioning)
find_package(Qt6 REQUIRED COMPONENTS Location Network Positioning)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(${PROJECT_NAME} SHARED
    ./src/MapPlugin.cpp 
    ./src/MapPlugin.h 
    ./src/MapPluginEngine.h 
    ./src/MapPluginEngine.cpp
    ./src/MapTileFetcher.cpp 
    ./src/MapTileFetcher.h
    ./src/MapReply.cpp 
    ./src/MapReply.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    QT_NEEDS_OLD_LIBS TRUE
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Location
    Qt6::Network
    Qt6::Positioning
)
include_directories(${Qt6Location_PRIVATE_INCLUDE_DIRS})

install(TARGETS ${PROJECT_NAME}
    DESTINATION ${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/geoservices
)