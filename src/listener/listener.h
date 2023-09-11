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

namespace net = boost::asio;
namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

class listener : public std::enable_shared_from_this<listener>{
protected:
    net::io_context& _ioc;
    tcp::acceptor _acceptor;
    std::shared_ptr<std::string const> doc_root_;
    void
    do_accept()
    {
        // The new connection gets its own strand
        _acceptor.async_accept(
                net::make_strand(_ioc),
                beast::bind_front_handler(
                        &listener::on_accept,
                        shared_from_this()));
    }

    void
    on_accept(beast::error_code ec, tcp::socket socket)
    {
        if(ec)
        {
            fail(ec, "accept");
            return; // To avoid infinite loop
        }
        else
        {
            // Create the session and run it
            std::make_shared<http_session>(
                    std::move(socket),
                    doc_root_)->run();
        }

        // Accept another connection
        do_accept();
    }
public:
    listener(net::io_context& ioc, tcp::endpoint endpoint, std::shared_ptr<std::string const> const& doc_root)
        :_ioc(ioc), _acceptor(net::make_strand(ioc)),doc_root_(doc_root){
    beast::error_code er;

    _acceptor.open(endpoint.protocol(), er);
    if(er)
    {
        fail(er, "open");
        return;
    }

    _acceptor.set_option(net::socket_base::reuse_address(true), er);
    if(er)
    {
        fail(er, "set_option");
        return;
    }

    _acceptor.bind(endpoint, er);
    if(er)
    {
        fail(er, "bind");
        return;
    }

    // Start listening for connections
    _acceptor.listen(
            net::socket_base::max_listen_connections, er);
    if(er)
    {
        fail(er, "listen");
        return;
    }
    }
    void run(){
        do_accept();
    }
};

#endif //WEBSERVER_LISTENER_H
