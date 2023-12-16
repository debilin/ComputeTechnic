project(lego_technic_test)

add_subdirectory(algorithm_test)
add_subdirectory(system_test)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (WIN32)
    add_subdirectory(lib/gtest-1.7.0)
endif (WIN32)
