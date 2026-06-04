add_library(aips_core STATIC
    Device.cpp
    ScanEngine.cpp
    ScanWorker.cpp
    ScanRange.cpp
    DeviceStore.cpp
    ScanOptions.cpp
)

target_include_directories(aips_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(aips_core PUBLIC
    Qt6::Core
    Qt6::Widgets
)
