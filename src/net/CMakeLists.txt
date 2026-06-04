add_library(aips_net STATIC
    IcmpProbe.cpp
    ArpProbe.cpp
    PortScanner.cpp
    DnsResolver.cpp
    NetbiosQuery.cpp
    VendorDb.cpp
)

target_include_directories(aips_net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(aips_net PUBLIC
    aips_core
    Qt6::Core
    Qt6::Network
)
