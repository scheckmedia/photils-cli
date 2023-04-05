include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE TEST_SRC ${PROJECT_SOURCE_DIR}/tests/*.cpp)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${OpenCV_INCLUDE_DIRS}
  ${ghc_filesystem_INCLUDE_DIRS}
)

add_executable(
  ${PROJECT_NAME}_test
  ${CMAKE_SOURCE_DIR}/src/helper.cpp
  ${CMAKE_SOURCE_DIR}/src/inference.cpp
  ${TEST_SRC}
)

set_target_properties(${PROJECT_NAME}_test
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}${CMAKE_BUILD_TYPE}//bin
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin
)

target_include_directories(${PROJECT_NAME}_test PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(
  ${PROJECT_NAME}_test
  ${OpenCVModules}
  JsonCpp::JsonCpp cxxopts::cxxopts ghcFilesystem::ghc_filesystem
  gtest_main
  gtest
)

include(GoogleTest)

# gtest_discover_tests(${PROJECT_NAME}_test)
