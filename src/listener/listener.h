//
// Created by kamil on 11.09.23.
//

#ifndef WEBSERVER_LISTENER_H
#define WEBSERVER_LISTENER_H

#include "../session/ssl/https_session.h"
#include "../session/ssl/wss_session.h"
#include "../test_ssl/server_cert.hpp"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/config.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>


enum SESSION_TYPE {
    HTTP,
    HTTPS,
    WS,
    WSS,
    TCP,
    UDP
};

namespace net = boost::asio;
namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

class listener : public std::enable_shared_from_this<listener> {

protected:
    unsigned _port;
    net::io_context &_ioc;
    ssl::context ssl_ctx;
    tcp::acceptor _acceptor;
    std::string doc_root_=".";
    std::atomic<bool> stop_requested{false};
    std::unordered_map<std::string,request_handler> http_request_handlers;
    SESSION_TYPE _current_type;

    void do_accept();

    void on_accept(beast::error_code ec, tcp::socket socket);


protected:

    std::shared_ptr<session> get_session(tcp::socket socket);

public:
    listener(net::io_context &ioc, tcp::endpoint endpoint, SESSION_TYPE _type = SESSION_TYPE::HTTP,ssl::context s_ctx = ssl::context(ssl::context::tlsv12))
            : _port(endpoint.port()), _ioc(ioc), _acceptor(net::make_strand(ioc)), _current_type(_type), ssl_ctx(ssl::context(ssl::context::tlsv13)) {
        beast::error_code er;

        if(_type==SESSION_TYPE::HTTPS||_type==SESSION_TYPE::WSS)
            load_server_certificate(ssl_ctx);

        _acceptor.open(endpoint.protocol(), er);
        if (er) {
            fail(er, "open");
            return;
        }

        _acceptor.set_option(net::socket_base::reuse_address(true), er);
        if (er) {
            fail(er, "set_option");
            return;
        }

        _acceptor.bind(endpoint, er);
        if (er) {
            fail(er, "bind");
            return;
        }

        // Start listening for connections
        _acceptor.listen(
                net::socket_base::max_listen_connections, er);

        if (er) {
            fail(er, "listen");
            return;
        }
    }

    void set_doc_root(std::string &docRoot);

    void run();

    void insert_handler(std::string path,request_handler handler);

    void stop();

    unsigned int get_port() const;

};

#endif //WEBSERVER_LISTENER_H
