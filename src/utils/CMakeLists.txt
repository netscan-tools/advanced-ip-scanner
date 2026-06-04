add_library(aips_utils STATIC
    AppConfig.cpp
    Logger.cpp
)

target_include_directories(aips_utils PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(aips_utils PUBLIC
    aips_core
    Qt6::Core
)
