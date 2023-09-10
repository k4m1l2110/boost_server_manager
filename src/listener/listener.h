//
// Created by kamil on 11.09.23.
//

#ifndef WEBSERVER_LISTENER_H
#define WEBSERVER_LISTENER_H

#include "../session/http_session.h"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>


namespace net = boost::asio;
namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

class listener{
protected:
    net::io_context& _ioc;
    tcp::acceptor _acceptor;
public:
    listener(net::io_context& ioc, tcp::endpoint endpoint)
        :_ioc(ioc), _acceptor(ioc){
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
};

#endif //WEBSERVER_LISTENER_H
