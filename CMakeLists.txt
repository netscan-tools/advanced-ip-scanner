cmake_minimum_required(VERSION 3.18)
project(AdvancedIPScanner
    VERSION 2.5.4995
    DESCRIPTION "Advanced IP Scanner — fast and reliable network scanner"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(AIPS_BUILD_TESTS    "Build unit tests"          OFF)
option(AIPS_BUILD_PORTABLE "Build portable executable" ON)
option(AIPS_ENABLE_RADMIN  "Enable Radmin integration" ON)
option(AIPS_ENABLE_RDP     "Enable RDP launcher"       ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network Concurrent Xml)
find_package(WinSock2 REQUIRED)
find_package(IPHLPAPI REQUIRED)

add_subdirectory(src/core)
add_subdirectory(src/net)
add_subdirectory(src/remote)
add_subdirectory(src/ui)
add_subdirectory(src/utils)

add_executable(${PROJECT_NAME} WIN32
    src/main.cpp
    resources/app.rc
    resources/resources.qrc
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    aips_core
    aips_net
    aips_remote
    aips_ui
    aips_utils
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    ws2_32
    iphlpapi
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    AIPS_VERSION="${PROJECT_VERSION}"
    AIPS_APP_NAME="${PROJECT_NAME}"
    $<$<BOOL:${AIPS_BUILD_PORTABLE}>:AIPS_PORTABLE>
    $<$<BOOL:${AIPS_ENABLE_RADMIN}>:AIPS_RADMIN_SUPPORT>
    $<$<BOOL:${AIPS_ENABLE_RDP}>:AIPS_RDP_SUPPORT>
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
)

if(AIPS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
