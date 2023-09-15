#ifndef SERVER_H
#define SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include "../listener/listener.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#define _HTTP_PORT 8080
#define _ADDR "localhost"


class server {
    net::ip::address _ip;
    unsigned short _port;
    net::io_context _ioc;
    unsigned _io_threads_num;
    std::vector<std::thread> _threads;

public:
    server(std::string addr = _ADDR, unsigned short port = _HTTP_PORT, int io_threads = 1) :
            _ip(net::ip::make_address(addr)), _port(port), _ioc(std::max<int>(1, io_threads)), _io_threads_num(io_threads) {}

    void run_http(std::string doc_root);

};

#endif


