include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runBasicTest basic_check.cpp cpp_test.cpp)

if (WIN32)
    target_link_libraries(runBasicTest lego_technic_system)
    target_link_libraries(runBasicTest gtest gtest_main)
endif (WIN32)
    
if(UNIX AND NOT APPLE)
    find_package(GTest REQUIRED)
    target_link_libraries(runBasicTest PUBLIC lego_technic_system)
    target_link_libraries(runBasicTest PUBLIC GTest::GTest GTest::Main)
endif()

