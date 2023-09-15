cmake_minimum_required(VERSION 3.26)
project(WebServer)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})


include_directories(src/client_test)
include_directories(src/wss)
include_directories(src/argon2)
include_directories(src/data)
include_directories(src/data/flatbuffer)

add_executable(WebServer
        #src/wss/server.cpp
        src/server.cpp
        src/main.cpp
        src/session/http_session.cpp
)

target_link_libraries(WebServer ${Boost_LIBRARIES})
target_link_libraries(WebServer pthread)
