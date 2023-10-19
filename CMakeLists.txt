cmake_minimum_required(VERSION 3.6)
project(WebServer)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
find_package(Curses REQUIRED)
find_package(OpenSSL REQUIRED)  # Add this line to find OpenSSL

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
        src/session/https_session.cpp
        src/test_ssl/server_cert.hpp
)

target_link_libraries(WebServer ${Boost_LIBRARIES})
target_link_libraries(WebServer pthread)
target_link_libraries(WebServer ${CURSES_LIBRARY})
target_link_libraries(WebServer OpenSSL::SSL OpenSSL::Crypto)  # Link against OpenSSL libraries
