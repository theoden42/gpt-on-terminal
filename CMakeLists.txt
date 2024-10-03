cmake_minimum_required(VERSION 3.20)

project(GptOnTerminal VERSION 1.0 LANGUAGES CXX)

find_package(CURL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(gpt-on-terminal 
    src/application_main.cc
    src/gtot_api.cc
)

target_link_libraries(gpt-on-terminal CURL::libcurl)
