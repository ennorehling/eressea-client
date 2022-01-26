cmake_minimum_required(VERSION 3.12)

project (mapper C)

set(CMAKE_C_STANDARD 11)

if (WIN32)
file(TO_CMAKE_PATH "${CMAKE_TOOLCHAIN_FILE}" CMAKE_TOOLCHAIN_FILE )
file(TO_CMAKE_PATH "${CMAKE_MODULE_PATH}" CMAKE_MODULE_PATH )
file(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}" CMAKE_PREFIX_PATH )
endif(WIN32)

find_package(CJSON 1.7 REQUIRED)
find_package(RAYLIB 3.0 REQUIRED)

add_subdirectory(crpat)

add_executable(mapper mapper.c log.c crfile.c crdata.c crjson.c)
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
