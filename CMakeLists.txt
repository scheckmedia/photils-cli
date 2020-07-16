cmake_minimum_required(VERSION 3.10)
project(photils-cli)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")

if(APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
  set(MACOSX_BUNDLE_ICON_FILE photils.icns)
  set(OSX_APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/appimage_resources/photils.icns)
  set_source_files_properties(${OSX_APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin)


include_directories(libs/cxxopts/include)
include_directories(libs/filesystem/include)

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
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_directories(${JSONCPP_LIBRARY_DIRS})

get_filename_component(RESOURCE_FOLDER ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} DIRECTORY)

include_directories(
  ${OpenCV_INCLUDE_DIRS}
  ${JSONCPP_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
  )

file(GLOB_RECURSE SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
if(APPLE)
  set(EXTRA_LIBS "")
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${OSX_APP_ICON} ${SRC})
else()
  set(EXTRA_LIBS "stdc++fs")
  add_executable(${PROJECT_NAME} ${SRC})
endif()
message(${TFLITE_LIBRARY})
message("Extra ${EXTRA_LIBS}")

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBRARIES} ${JSONCPP_LIBRARIES} ${TFLITE_LIBRARY} Threads::Threads ${EXTRA_LIBS})

file(MAKE_DIRECTORY ${RESOURCE_FOLDER}/share/photils/)
  add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/res/* ${RESOURCE_FOLDER}/share/photils/
  )

if(APPLE)
  set(APPS "\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app")
  set(DIRS "\${CMAKE_INSTALL_PREFIX}/share")
  LIST(APPEND DIRS "${OpenCV_DIR}/lib")

  install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION . COMPONENT Runtime
    RUNTIME DESTINATION bin COMPONENT Runtime
  )
  install(DIRECTORY  ${RESOURCE_FOLDER}/share/photils/ DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}.app/Contents/share/photils)

  install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\"  \"\"  \"${DIRS}\")
  " COMPONENT Runtime)
  # set(CPACK_GENERATOR "DRAGNDROP")
  # include(CPack)
else()
  install(TARGETS ${PROJECT_NAME} DESTINATION bin)
  install(FILES appimage_resources/photils-cli.desktop DESTINATION share/applications/)
  install(FILES appimage_resources/photils.svg DESTINATION share/icons/)
  install(DIRECTORY  ${RESOURCE_FOLDER}/share/photils/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/photils)
endif()
