#include "./listener.h"

void listener::do_accept() {
    // The new connection gets its own strand
    _acceptor.async_accept(
            net::make_strand(_ioc),
            beast::bind_front_handler(
                    &listener::on_accept,
                    shared_from_this()));
}

void listener::on_accept(beast::error_code ec, tcp::socket socket) {
    if (ec) {
        fail(ec, "accept");
        return; // To avoid infinite loop
    } else {
        // Create the session and run it

        std::make_shared<http_session>(
                std::move(socket),
                doc_root_)->run();
    }

    // Accept another connection
    do_accept();
}

void listener::run() {
    do_accept();
}

void listener::stop() {
    stop_requested.store(true);
    _acceptor.close();
}