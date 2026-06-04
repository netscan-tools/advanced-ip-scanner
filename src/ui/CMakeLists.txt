add_library(aips_ui STATIC
    MainWindow.cpp
    ScanResultView.cpp
    FavoritesPanel.cpp
    DeviceDetailsPanel.cpp
    PreferencesDialog.cpp
)

target_include_directories(aips_ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(aips_ui PUBLIC
    aips_core
    aips_net
    aips_remote
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
)
