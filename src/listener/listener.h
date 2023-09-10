//
// Created by kamil on 11.09.23.
//

#ifndef WEBSERVER_LISTENER_H
#define WEBSERVER_LISTENER_H

#include <boost/asio.hpp>


namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class listener{
    net::io_context& _ioc;
    tcp::acceptor _acceptor;

};

#endif //WEBSERVER_LISTENER_H
