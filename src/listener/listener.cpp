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
        get_session(_current_type,std::move(socket))->run();
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

unsigned int listener::get_port() const {
    return _port;
}

std::shared_ptr<session> listener::get_session(SESSION_TYPE _type,tcp::socket socket){
    std::string doc_root;
    switch(_type){
        case SESSION_TYPE::HTTP:
            std::cout<<"Set document root: ";std::cin>>doc_root;
            return http_session::create_session(std::move(socket),doc_root);
        default:
            break;
    }
}
