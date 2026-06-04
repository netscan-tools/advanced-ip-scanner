add_library(aips_remote STATIC
    RdpLauncher.cpp
    WakeOnLan.cpp
    RemoteShutdown.cpp
)

target_include_directories(aips_remote PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(aips_remote PUBLIC
    aips_core
    Qt6::Core
    Qt6::Network
)
