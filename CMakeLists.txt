cmake_minimum_required(VERSION 3.5)

project(qt_icmp)

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Concurrent)

# Add the library
add_library(${PROJECT_NAME} SHARED
    src/icmp_packet.cpp
    src/icmp_socket.cpp
    src/icmp_wrapper.cpp
)

# Specify include directories
target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

# Link against Qt5Core
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Concurrent
)

# Handle MOC
set(moc_headers
    include/qt_icmp/icmp_wrapper.hpp
    include/qt_icmp/icmp_socket.hpp
)

qt5_wrap_cpp(moc_files ${moc_headers})

# Add MOC files to the library
target_sources(${PROJECT_NAME} PRIVATE ${moc_files})

# Install targets
install(TARGETS ${PROJECT_NAME}
    EXPORT export_${PROJECT_NAME}
    ARCHIVE DESTINATION lib/
    LIBRARY DESTINATION lib/
    RUNTIME DESTINATION bin/
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
)

# Export the package
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

ament_package()
