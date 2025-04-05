cmake_minimum_required(VERSION 3.10)
project(WinsockClientServerDemo)

set(CMAKE_CXX_STANDARD 17)

add_executable(WinsockServer src/Server.cpp)
add_executable(WinsockClient src/Client.cpp)

target_link_libraries(WinsockServer ws2_32)
target_link_libraries(WinsockClient ws2_32)
