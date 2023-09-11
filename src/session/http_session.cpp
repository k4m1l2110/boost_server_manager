//
// Created by kamil on 10.09.23.
//

#include "http_session.h"

void http_session::run(){
    net::dispatch(_tcp_stream.get_executor(),
                  beast::bind_front_handler(
                          &http_session::do_read,
                          shared_from_this()
                          ));
}

void http_session::do_read() {
    _request = {};

    _tcp_stream.expires_after(std::chrono::seconds(30));

    http::async_read(_tcp_stream,
                     _buffer,
                     _request,
                     beast::bind_front_handler(
                             &http_session::on_read,
                             shared_from_this()
                             ));
}

void http_session::on_read(beast::error_code er, std::size_t bytes) {
    boost::ignore_unused(bytes);

    if(er == http::error::end_of_stream)
        return do_close();
    if(er)
        return fail(er,"read");

    send_response(handle_request(*_doc_root,
                                 std::move(_request)));
}

void http_session::on_write(beast::error_code er, std::size_t bytes) {
    boost::ignore_unused(bytes);

    if(er)
        return fail(er,"write");

    do_read();
}

void http_session::do_close() {
    beast::error_code er;
    _tcp_stream.socket().shutdown(tcp::socket::shutdown_send, er);
}

void http_session::send_response(http::message_generator &&ms) {
    bool keep_alive = ms.keep_alive();

    beast::async_write(
            _tcp_stream,
            std::move(ms),
            beast::bind_front_handler(
                    &http_session::on_write,
                    shared_from_this()
                    )
            );
}


