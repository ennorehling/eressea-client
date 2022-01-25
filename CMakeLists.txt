cmake_minimum_required(VERSION 3.15)

project (mapper C)

set(CMAKE_C_STANDARD 11)

if (WIN32)
file(TO_CMAKE_PATH "${CMAKE_TOOLCHAIN_FILE}" CMAKE_TOOLCHAIN_FILE )
file(TO_CMAKE_PATH "${CMAKE_MODULE_PATH}" CMAKE_MODULE_PATH )
file(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}" CMAKE_PREFIX_PATH )
endif(WIN32)

find_package(raylib 3.0 REQUIRED)
find_package(cJSON 1.7 REQUIRED)

add_subdirectory(crpat)

message(STATUS "CJSON ${CJSON_INCLUDE_DIRS}")
add_executable(mapper mapper.c log.c crfile.c crdata.c crjson.c)
target_link_libraries(mapper PRIVATE
    ${CRPAT_LIBRARIES}
    ${raylib_LIBRARIES}
    ${CJSON_LIBRARIES}
)

target_include_directories(mapper PRIVATE
    ${CRPAT_INCLUDE_DIR}
    ${raylib_INCLUDE_DIRS}
    ${CJSON_INCLUDE_DIRS}
)

