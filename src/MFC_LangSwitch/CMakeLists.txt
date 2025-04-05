cmake_minimum_required(VERSION 3.10)
project(LanguageSwitcher)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
add_executable(LanguageSwitcher src/LanguageSwitcher.cpp)
