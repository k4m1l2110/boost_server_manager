//
// Created by kamil on 30.10.23.
//

#ifndef WEBSERVER_WSS_SESSION_H
#define WEBSERVER_WSS_SESSION_H

#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/beast/ssl.hpp>

#include "../ws_session.h"

namespace ssl = boost::asio::ssl;

class wss_session : public ws_session{
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> _wss_stream;
    void run() override;
    void on_run() override;
    void on_handshake(beast::error_code er) override;
    void on_accept(beast::error_code er) override;
    void do_read() override;
    void on_read(beast::error_code er, std::size_t bytes) override;
public:
    wss_session(tcp::socket &&socket,ssl::context &ssl_ctx)
            : ws_session(std::move(boost::asio::ip::tcp::socket(boost::asio::io_context(1).get_executor()))),
              _wss_stream(std::move(socket),ssl_ctx){;}

    static std::shared_ptr<session> create_session(tcp::socket socket,ssl::context &ssl_ctx);
};


#endif //WEBSERVER_WSS_SESSION_H
