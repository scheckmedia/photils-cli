include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE TEST_SRC ${PROJECT_SOURCE_DIR}/tests/*.cpp)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
)

add_executable(
  ${PROJECT_NAME}_test
  ${CMAKE_SOURCE_DIR}/src/helper.cpp
  ${CMAKE_SOURCE_DIR}/src/inference.cpp
  ${TEST_SRC}
)

target_include_directories(${PROJECT_NAME}_test PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(
  ${PROJECT_NAME}_test
  ${CONAN_LIBS} ${EXTRA_LIBS}
  gtest_main
  gtest
)

include(GoogleTest)

# gtest_discover_tests(${PROJECT_NAME}_test)
