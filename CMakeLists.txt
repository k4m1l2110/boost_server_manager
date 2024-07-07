cmake_minimum_required(VERSION 3.6)
project(WebServer)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
find_package(Curses REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CURSES_INCLUDE_DIR})

include_directories(include)
include_directories(include/argon2)
include_directories(include/data)

add_library(my_server
        src/server/server.cpp
        src/session/http_session.cpp
        src/listener/listener.cpp
        src/session/ws_session.cpp
        src/session/ssl/https_session.cpp
        src/session/ssl/wss_session.cpp
)

add_executable(WebServer
        src/main.cpp
)

target_link_libraries(WebServer my_server ${Boost_LIBRARIES} pthread ${CURSES_LIBRARY} OpenSSL::SSL OpenSSL::Crypto)