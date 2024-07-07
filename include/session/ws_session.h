//
// Created by kamil on 25.09.23.
//

#ifndef WEBSERVER_WS_SESSION_H
#define WEBSERVER_WS_SESSION_H

#include "session.h"
#include "../utils/http_utils.h"
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/strand.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;

class ws_session : public session {
    websocket::stream<beast::tcp_stream> _ws_stream;
public:
    ws_session(tcp::socket &&socket)
            : session(std::move(boost::asio::ip::tcp::socket(boost::asio::io_context(1).get_executor()))),_ws_stream(std::move(socket)){;}

    void run() override;

    void on_run() override;

    void do_read() override;

    void on_read(beast::error_code er,
                 std::size_t bytes) override;

    void on_write(beast::error_code er,
                  std::size_t bytes) override;

    void on_accept(beast::error_code er) override;

    void do_close() override { ; }

    static std::shared_ptr<session> create_session(tcp::socket socket);

    void on_handshake(beast::error_code er) override {;}
};


#endif //WEBSERVER_WS_SESSION_H
