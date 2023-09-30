//
// Created by kamil on 10.09.23.
//

#ifndef WEBSERVER_SESSION_H
#define WEBSERVER_SESSION_H

#include <memory>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>

namespace net = boost::asio;
namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;


class session : public std::enable_shared_from_this<session> {
protected:
    beast::tcp_stream _tcp_stream;
    beast::flat_buffer _buffer;
public:
    session(tcp::socket &&socket) : _tcp_stream(std::move(socket)) {}
    session();
    virtual void run() = 0;

    virtual void on_run() = 0;

    virtual void do_read() = 0;

    virtual void on_read(beast::error_code er,
                         std::size_t bytes) = 0;

    virtual void on_accept(beast::error_code er) = 0;

    virtual void on_write(beast::error_code er,
                          std::size_t bytes) = 0;

    virtual void do_close() = 0;

};


#endif //WEBSERVER_SESSION_H
