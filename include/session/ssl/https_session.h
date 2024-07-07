//
// Created by kamil on 19.10.23.
//

#ifndef WEBSERVER_HTTPS_SESSION_H
#define WEBSERVER_HTTPS_SESSION_H

#include "../http_session.h"
#include "../../utils/http_utils.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <boost/beast/ssl.hpp>

namespace ssl = boost::asio::ssl;

class https_session : public http_session {
    friend class listener;

    beast::ssl_stream<beast::tcp_stream> _ssl_stream;

    void run() override;

    void on_run() override;

    void on_handshake(beast::error_code er) override;

    void do_read() override;

    void send_response(http::message_generator &&ms) override;

public:
    https_session(tcp::socket &&socket, std::shared_ptr<std::string const> const &doc_root, ssl::context &ssl_ctx,
                  std::unordered_map<std::string, request_handler> &r_handlers)
            : http_session(std::move(boost::asio::ip::tcp::socket(boost::asio::io_context(1).get_executor())), doc_root, r_handlers), _ssl_stream(std::move(socket), ssl_ctx) { ; }

    static std::shared_ptr<session> create_session(
            tcp::socket socket,
            std::string doc_root,
            std::unordered_map<std::string, request_handler> &r_handlers,
            ssl::context &ssl_ctx);

};


#endif //WEBSERVER_HTTPS_SESSION_H
