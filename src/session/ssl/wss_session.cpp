//
// Created by kamil on 30.10.23.
//

#include "wss_session.h"

void wss_session::run()
{
    net::dispatch(_wss_stream.get_executor(),
                  beast::bind_front_handler(
                          [self= shared_from_this()]() {
                              self->on_run();
                          })
                          );
}

void wss_session::on_run()
{
    beast::get_lowest_layer(_wss_stream).expires_after(std::chrono::seconds(30));

    _wss_stream.next_layer().async_handshake(
            ssl::stream_base::server,
            beast::bind_front_handler(
                    [self= shared_from_this()](beast::error_code er) {
                        self->on_handshake(er);
                    })
    );
}

void wss_session::on_handshake(beast::error_code er)
{
    //Throwing error with test certificate
    /*if(er)
        return fail(er, "handshake");*/

    beast::get_lowest_layer(_wss_stream).expires_never();

    _wss_stream.set_option(
            websocket::stream_base::timeout::suggested(
                    beast::role_type::server));

    _wss_stream.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                        std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-async-ssl");
            }));


    _wss_stream.async_accept(
            beast::bind_front_handler(
                    [self= shared_from_this()](beast::error_code er) {
                        self->on_accept(er);
                    })
                    );
}

void wss_session::on_accept(beast::error_code er)
{
    std::cout<<"wss accept"<<std::endl;
    if(er)
        return fail(er, "accept");

    do_read();
}

void wss_session::do_read()
{
    _wss_stream.async_read(
            _buffer,
            beast::bind_front_handler(
                    [self= shared_from_this()](beast::error_code er,std::size_t transfered) {
                        self->on_read(er,transfered);
                    }));
}

void wss_session::on_read(
        beast::error_code er,
        std::size_t transferred)
{
    boost::ignore_unused(transferred);

    if(er == websocket::error::closed)
        return;

    if(er)
        fail(er, "read");

    _wss_stream.text(_wss_stream.got_text());
    _wss_stream.async_write(
            _buffer.data(),
            beast::bind_front_handler(
                    [self= shared_from_this()](beast::error_code er,std::size_t transfered) {
                        self->on_write(er,transfered);
                    })
                    );

}

                    std::shared_ptr<session> wss_session::create_session(tcp::socket socket,ssl::context &ssl_ctx) {
    return std::static_pointer_cast<session>(std::make_shared<wss_session>(std::move(socket),ssl_ctx));
}
