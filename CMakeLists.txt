cmake_minimum_required(VERSION 3.12)

project (mapper C)

enable_testing()
set(CMAKE_C_STANDARD 11)

if (WIN32)
file(TO_CMAKE_PATH "${CMAKE_TOOLCHAIN_FILE}" CMAKE_TOOLCHAIN_FILE )
file(TO_CMAKE_PATH "${CMAKE_MODULE_PATH}" CMAKE_MODULE_PATH )
file(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}" CMAKE_PREFIX_PATH )
endif(WIN32)

find_package(cJSON 1.7 REQUIRED)
find_package(raylib 3.0 QUIET)

if(raylib_FOUND)
  set(RAYLIB_FOUND 1)
  set(RAYLIB_LIBRARIES "${raylib_LIBRARIES}")
  set(RAYLIB_INCLUDE_DIRS "${raylib_INCLUDE_DIRS}")
endif(raylib_FOUND)

if(NOT RAYLIB_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/4.0.0.tar.gz
  )
  FetchContent_MakeAvailable(raylib)
  set(RAYLIB_LIBRARIES raylib)
  set(RAYLIB_INCLUDE_DIRS "${raylib_SOURCE_DIR}/src")
  set(OpenGL_GL_PREFERENCE "GLVND")
endif()

add_subdirectory(crpat)

add_executable(mapper mapper.c libs.c crjson.c crfile.c log.c mapdata.c viewport.c sds/sds.c)
target_link_libraries(mapper PRIVATE
    ${CRPAT_LIBRARIES}
    ${RAYLIB_LIBRARIES}
    ${CJSON_LIBRARIES}
)

target_include_directories(mapper PRIVATE
    ${CRPAT_INCLUDE_DIR}
    ${RAYLIB_INCLUDE_DIRS}
    ${CJSON_INCLUDE_DIRS}
)

add_executable(tests tests.c libs.c CuTest.c mapdata.c crfile.c log.c viewport.c sds/sds.c)
target_include_directories(tests PRIVATE
    ${CRPAT_INCLUDE_DIR}
    ${CJSON_INCLUDE_DIRS}
)

target_link_libraries(tests PRIVATE
    ${CRPAT_LIBRARIES}
    ${CJSON_LIBRARIES}
)

add_test(tests tests)
