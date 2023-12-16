include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(runAlgorithmTest algorithm_test.cpp)

if(WIN32)
    target_link_libraries(runAlgorithmTest lego_technic_system)
    target_link_libraries(runAlgorithmTest gtest gtest_main)
endif()

if(UNIX AND NOT APPLE)
    find_package(GTest REQUIRED)
    target_link_libraries(runAlgorithmTest PUBLIC lego_technic_system)
    target_link_libraries(runAlgorithmTest PUBLIC GTest::GTest GTest::Main)
endif()

