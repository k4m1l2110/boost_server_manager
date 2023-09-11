//
// Created by kamil on 10.09.23.
//

#ifndef WEBSERVER_HTTP_SESSION_H
#define WEBSERVER_HTTP_SESSION_H
#include "../utils/http_utils.h"
#include "session.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/beast/version.hpp>


namespace net = boost::asio;
namespace http = boost::beast::http;
namespace ip = boost::asio::ip;
namespace beast = boost::beast;         // from <boost/beast.hpp>
using tcp = boost::asio::ip::tcp;


class http_session: public std::enable_shared_from_this<http_session> {
    beast::tcp_stream _tcp_stream;
    beast::flat_buffer _buffer;
    std::shared_ptr<std::string const> _doc_root;
    http::request<http::string_body> _request;
public:
    http_session(tcp::socket &&socket,std::shared_ptr<std::string const> const& doc_root)
        : _tcp_stream(std::move(socket)),_doc_root(doc_root) { ; }
    void run();
    void do_read();
    void on_read(beast::error_code er,
                 std::size_t bytes);
    void on_write(beast::error_code er,
                  std::size_t bytes);
    void do_close();
    void send_response(http::message_generator&& ms);
};


#endif //WEBSERVER_HTTP_SESSION_H
