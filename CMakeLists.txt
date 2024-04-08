cmake_minimum_required(VERSION 3.0)
project(ecRAT)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries/libwebsockets/build/include)

file(GLOB SOURCES "src/server/*.c")
add_executable(server ${SOURCES})

file(GLOB SOURCES "src/client/*.c")
add_executable(client ${SOURCES})

target_link_libraries(server
    ${CMAKE_CURRENT_SOURCE_DIR}/libraries/libwebsockets/build/lib/libwebsockets.a
    ssl
    crypto)

target_link_libraries(client
    ${CMAKE_CURRENT_SOURCE_DIR}/libraries/libwebsockets/build/lib/libwebsockets.a
    ssl
    crypto)