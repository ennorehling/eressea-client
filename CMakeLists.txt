cmake_minimum_required(VERSION 3.12)

project (mapper C)

enable_testing()
set(CMAKE_C_STANDARD 11)
set(OpenGL_GL_PREFERENCE "GLVND") 

if (WIN32)
file(TO_CMAKE_PATH "${CMAKE_TOOLCHAIN_FILE}" CMAKE_TOOLCHAIN_FILE )
file(TO_CMAKE_PATH "${CMAKE_MODULE_PATH}" CMAKE_MODULE_PATH )
file(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}" CMAKE_PREFIX_PATH )
endif(WIN32)

find_package(cJSON)
add_subdirectory(raylib)
set(RAYLIB_LIBRARIES raylib)
set(RAYLIB_INCLUDE_DIRS raylib/src)
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
