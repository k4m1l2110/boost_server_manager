//
// Created by kamil on 10.09.23.
//

#ifndef WEBSERVER_HTTP_SESSION_H
#define WEBSERVER_HTTP_SESSION_H

#include "session.h"
#include "../utils/http_utils.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>


namespace net = boost::asio;
namespace http = boost::beast::http;
namespace ip = boost::asio::ip;
namespace beast = boost::beast;         // from <boost/beast.hpp>
using tcp = boost::asio::ip::tcp;

using request_handler = std::function<http::response<http::string_body>(
        beast::basic_string_view<char> reques_type,
        std::string request_body
        )>;

class http_session : public session {
    std::shared_ptr<std::string const> _doc_root;
    http::request<http::string_body> _request;
    std::unordered_map<std::string,request_handler> request_handlers;

    template<class Body, class Allocator>
    http::message_generator
    handle_get(
            beast::string_view doc_root,
            http::request<Body, http::basic_fields<Allocator>> &&req);

    http::message_generator
    handle_post(
            http::request<http::string_body> &&req);

public:
    http_session(tcp::socket &&socket, std::shared_ptr<std::string const> const &doc_root)
            : session(std::move(socket)), _doc_root(doc_root) { ; }


    void run() override;

    void do_read() override;

    void on_read(beast::error_code er,
                 std::size_t bytes) override;

    void on_write(beast::error_code er,
                  std::size_t bytes) override;

    void do_close() override;

    static std::shared_ptr<session> create_session(tcp::socket socket, std::string doc_root);

    void send_response(http::message_generator &&ms);

    //Dont use on http
    void on_accept(beast::error_code er) override { ; }

    //Dont use on http
    void on_run() override { ; }
};


#endif //WEBSERVER_HTTP_SESSION_H
