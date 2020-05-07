cmake_minimum_required(VERSION 3.10)
project(photils-cli)


set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)


include_directories(libs/cxxopts/include)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

set(TF_DIR                  "" CACHE PATH "Tensorflow directory")
set(FLATBUFFER_DIR          ${TF_DIR}/tensorflow/lite/tools/make/downloads/flatbuffers/include)
set(ABSL_DIR                ${TF_DIR}/tensorflow/lite/tools/make/downloads/absl)

find_library(
    TFLITE_LIBRARY
    libtensorflow-lite.a
    PATHS
        ${TF_DIR}/tensorflow/lite/tools/make/gen/linux_x86_64/lib
        ${TF_DIR}/tensorflow/lite/tools/make/gen/osx_x86_64/lib
)

if(NOT TFLITE_LIBRARY)
  message("Maybe you have not compiled the tflite lib in TF_DIR/tensorflow/lite/tools/make/")
  message(FATAL_ERROR "tflite library not found")
endif()

include_directories(
    ${FLATBUFFER_DIR}
    ${ABSL_DIR}
    ${TF_DIR}
)

find_package(OpenCV 4.1.0 COMPONENTS core imgproc imgcodecs highgui REQUIRED)
find_package(CURL REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_directories(${JSONCPP_LIBRARY_DIRS})

get_filename_component(RESOURCE_FOLDER ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} DIRECTORY)
message("haaa ${RESOURCE_FOLDER}")

include_directories(
  ${OpenCV_INCLUDE_DIRS}
  ${JSONCPP_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  )

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${CURL_LIBRARIES} ${JSONCPP_LIBRARIES} ${TFLITE_LIBRARY} Threads::Threads)

file(MAKE_DIRECTORY ${RESOURCE_FOLDER}/share/photils/)
add_custom_command(
 TARGET ${PROJECT_NAME} POST_BUILD
 COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/res/model.* ${RESOURCE_FOLDER}/share/photils/
)

# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# install(DIRECTORY  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/../share/photils/ DESTINATION share/photils)
