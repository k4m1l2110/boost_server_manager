//
// Created by kamil on 10.09.23.
//

#ifndef WEBSERVER_HTTP_SESSION_H
#define WEBSERVER_HTTP_SESSION_H

#include "session.h"
#include "../utils/http_utils.h"
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/beast/version.hpp>

namespace net = boost::asio;
namespace http = boost::beast::http;
namespace ip = boost::asio::ip;



class http_session: public session, public std::enable_shared_from_this<session> {
    std::shared_ptr<std::string const> _doc_root;
    http::request<http::string_body> _request;
public:
    http_session(tcp::socket &&socket,std::shared_ptr<std::string> const& doc_root)
        : session(std::move(socket)),_doc_root(doc_root) { ; }
    void run() override;
    void do_read() override;
    void on_read(beast::error_code er,
                 std::size_t bytes) override;
    void on_write(beast::error_code er,
                  std::size_t bytes) override;
    void do_close() override;
    void send_response(http::message_generator&& ms);
};


#endif //WEBSERVER_HTTP_SESSION_H
