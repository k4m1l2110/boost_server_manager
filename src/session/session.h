//
// Created by kamil on 10.09.23.
//

#ifndef WEBSERVER_SESSION_H
#define WEBSERVER_SESSION_H
#include <boost/beast/core.hpp>
namespace beast = boost::beast;         // from <boost/beast.hpp>
using tcp = boost::asio::ip::tcp;

class session{
protected:
    beast::tcp_stream _tcp_stream;
    beast::flat_buffer _buffer;
public:
    session(tcp::socket &&socket): _tcp_stream(std::move(socket)) { ; }
    virtual void run();
    virtual void do_read();
    virtual void on_read(beast::error_code er,
                         std::size_t bytes);
    virtual void on_write(beast::error_code er,
                          std::size_t bytes);
    virtual void do_close();
};


#endif //WEBSERVER_SESSION_H
