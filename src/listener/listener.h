//
// Created by kamil on 11.09.23.
//

#ifndef WEBSERVER_LISTENER_H
#define WEBSERVER_LISTENER_H

#include "../session/http_session.h"
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

enum SESSION_TYPE{
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
    tcp::acceptor _acceptor;
    std::shared_ptr<std::string const> doc_root_;
    std::atomic<bool> stop_requested{false};
    SESSION_TYPE _current_type=SESSION_TYPE::HTTP;
    void do_accept();
    void on_accept(beast::error_code ec, tcp::socket socket);
    std::shared_ptr<session> get_session(SESSION_TYPE _type,tcp::socket socket);

public:
    listener(net::io_context &ioc, tcp::endpoint endpoint)
            : _port(endpoint.port()),_ioc(ioc), _acceptor(net::make_strand(ioc)) {
        beast::error_code er;

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

    void run();
    void stop();
    unsigned int get_port() const;

};

#endif //WEBSERVER_LISTENER_H
