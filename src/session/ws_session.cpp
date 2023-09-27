//
// Created by kamil on 25.09.23.
//

#include "ws_session.h"

void ws_session::run() {
    net::dispatch(_ws_stream.get_executor(),
                  beast::bind_front_handler(
                          [self= shared_from_this()]() {
                              self->on_run();
                          })
    );
}

void ws_session::on_run() {
    // Set suggested timeout settings for the websocket
    _ws_stream.set_option(
            websocket::stream_base::timeout::suggested(
                    beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    _ws_stream.set_option(websocket::stream_base::decorator(
            [](websocket::response_type &res) {
                res.set(http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-async");
            }));
    // Accept the websocket handshake
    _ws_stream.async_accept(
            beast::bind_front_handler(
                    [self = shared_from_this()](beast::error_code er) {
                        self->on_accept(er);
                    })
    );

}

void ws_session::on_accept(beast::error_code er)
{
    if(er)
        return fail(er, "accept");

    // Read a message
    do_read();
}


void ws_session::do_read() {

    // Read a message into our buffer
    _ws_stream.async_read(
            _buffer,
            beast::bind_front_handler(
                    [self = shared_from_this()](beast::error_code er, std::size_t bytes) {
                        self->on_read(er, bytes);
                    })
                    );

}

void ws_session::on_read(beast::error_code er, std::size_t bytes) {
    boost::ignore_unused(bytes);

    if (er == websocket::error::closed)
        return;// do_close();
    if (er)
        return fail(er, "read");

    _ws_stream.text(_ws_stream.got_text());
    _ws_stream.async_write(
            _buffer.data(),
            beast::bind_front_handler(
                    [self = shared_from_this()](beast::error_code er, std::size_t bytes) {
                        self->on_write(er, bytes);
                    })
                    );

}

void ws_session::on_write(beast::error_code er, std::size_t bytes) {
    boost::ignore_unused(bytes);

    if (er)
        return fail(er, "write");

    _buffer.consume(_buffer.size());

    do_read();
}

/*void ws_session::do_close() {
    beast::error_code er;
    _ws_stream..socket().shutdown(tcp::socket::shutdown_send, er);
}*/

/*std::shared_ptr<session> ws_session::create_session(tcp::socket socket) {
    return std::make_shared<ws_session>(std::move(socket));
}*/


