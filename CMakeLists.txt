cmake_minimum_required(VERSION 3.6)
project(WebServer)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
find_package(Curses REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CURSES_INCLUDE_DIR})

include_directories(src/client_test)
include_directories(src/wss)
include_directories(src/argon2)
include_directories(src/data)
include_directories(src/data/flatbuffer)

add_executable(WebServer
        src/server.cpp
        src/main.cpp
        src/session/http_session.cpp
        src/listener/listener.cpp
        src/session/ws_session.cpp
)

target_link_libraries(WebServer ${Boost_LIBRARIES})
target_link_libraries(WebServer pthread)
target_link_libraries(WebServer ${CURSES_LIBRARY})