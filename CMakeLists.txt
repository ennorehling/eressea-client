cmake_minimum_required(VERSION 3.15)

project (mapper C)

set(CMAKE_C_STANDARD 11)

if (WIN32)
file(TO_CMAKE_PATH "${CMAKE_TOOLCHAIN_FILE}" CMAKE_TOOLCHAIN_FILE )
file(TO_CMAKE_PATH "${CMAKE_MODULE_PATH}" CMAKE_MODULE_PATH )
file(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}" CMAKE_PREFIX_PATH )
endif(WIN32)

find_package(raylib 3.0 REQUIRED)

add_executable(mapper mapper.c log.c crfile.c crdata.c)
target_include_directories(mapper PRIVATE ${raylib_INCLUDE_DIRS})
target_link_libraries(mapper PRIVATE ${raylib_LIBRARIES})

