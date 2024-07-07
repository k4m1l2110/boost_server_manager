//
// Created by kamil on 19.10.23.
//

#include "session/ssl/https_session.h"

void https_session::run() {
    net::dispatch(_ssl_stream.get_executor(),
                  beast::bind_front_handler(
                          [self = shared_from_this()]() {
                              self->on_run();
                          }
                  )
    );
}

void
https_session::on_run() {
    // Set the timeout.
    beast::get_lowest_layer(_ssl_stream).expires_after(
            std::chrono::seconds(30));

    // Perform the SSL handshake
    _ssl_stream.async_handshake(
            ssl::stream_base::server,
            beast::bind_front_handler(
                    &session::on_handshake,
                    shared_from_this()));
}

void
https_session::on_handshake(beast::error_code ec)
{
    /*if(ec)
        return fail(ec, "handshake");*/

    do_read();
}

void https_session::do_read() {
    _request = {};

    // Set the timeout.
    beast::get_lowest_layer(_ssl_stream).expires_after(std::chrono::seconds(30));

    http::async_read(_ssl_stream,
                     _buffer,
                     _request,
                     beast::bind_front_handler(
                             [self = shared_from_this()](beast::error_code er, std::size_t bytes) {
                                 self->on_read(er, bytes);
                             }
                     )
    );
}

void https_session::send_response(http::message_generator &&ms) {
    bool keep_alive = ms.keep_alive();

    beast::async_write(
            _ssl_stream,
            std::move(ms),
            beast::bind_front_handler(
                    [self = shared_from_this()](beast::error_code er, std::size_t bytes) {
                        self->on_write(er, bytes);
                    }
            )
    );
}

std::shared_ptr<session> https_session::create_session(
        tcp::socket socket,
        std::string doc_root,
        std::unordered_map<std::string, request_handler> &r_handlers,
        ssl::context &ssl_ctx) {
    return std::static_pointer_cast<session>(std::make_shared<https_session>(std::move(socket), std::make_shared<std::string const>(doc_root), ssl_ctx,r_handlers));
}

