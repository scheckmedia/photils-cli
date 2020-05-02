cmake_minimum_required(VERSION 3.10)
project(PhotilsCLI)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)

set(OpenCV_STATIC ON)
set(Boost_USE_STATIC_LIBS ON)


list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

add_subdirectory(libs/cpp-netlib)
include_directories(libs/cpp-netlib)

find_package(OpenCV 4.1.0 COMPONENTS core imgproc dnn imgcodecs highgui REQUIRED)
find_package(Boost 1.66 COMPONENTS program_options log system thread filesystem REQUIRED)
find_package(cppnetlib REQUIRED)

set(OPENCV_DNN_REQUIRED_DEPS
  opencv_core
  opencv_imgproc
  opencv_dnn
  opencv_imgcodecs
)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(PhotilsLr ${SRC})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${OpenCV_LIBS} ${CPPNETLIB_LIBRARIES} Threads::Threads)

add_custom_command(
 TARGET ${PROJECT_NAME} POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/res/opencv_model_v2.pb ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/opencv_model_v2.pb
)
