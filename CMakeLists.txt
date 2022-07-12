cmake_minimum_required(VERSION 3.14)
include(FeatureSummary)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build.")

project(photils-cli)
project(${PROJECT_NAME} VERSION "0.4.1")

OPTION(BUILD_TESTS "Build test" ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

message("Runtime DIr: ${OUTPUT_REL}")

if(APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
  set(MACOSX_BUNDLE_ICON_FILE photils.icns)
  set(OSX_APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/appimage_resources/photils.icns)
  set_source_files_properties(${OSX_APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()


file(GLOB_RECURSE PHOTILS_SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)

find_package(Threads REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

configure_file(${PROJECT_SOURCE_DIR}/include/version.h.in ${PROJECT_SOURCE_DIR}/include/version.h)

if(APPLE)
  set(EXTRA_LIBS "")
  add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${OSX_APP_ICON} ${PHOTILS_SRC})
else()
  set(EXTRA_LIBS "stdc++fs")
  add_executable(${PROJECT_NAME} ${PHOTILS_SRC})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
set_target_properties( ${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin
)

get_target_property( RESOURCE_FOLDER ${PROJECT_NAME} RUNTIME_OUTPUT_DIRECTORY)
get_filename_component(RESOURCE_FOLDER ${RESOURCE_FOLDER} DIRECTORY)



message("conan: ${CONAN_LIBS}")
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

message("COPY Target: ${RESOURCE_FOLDER}")

file(MAKE_DIRECTORY ${RESOURCE_FOLDER}/share/photils/)
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${RESOURCE_FOLDER}/share/photils
)

include(Install)

if(${BUILD_TESTS})
  add_subdirectory(tests)
endif()
