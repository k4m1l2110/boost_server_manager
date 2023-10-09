#include "./listener.h"

void listener::do_accept() {
    // The new connection gets its own strand
    _acceptor.async_accept(
            net::make_strand(_ioc),
            beast::bind_front_handler(
                    &listener::on_accept,
                    shared_from_this()));

}

void listener::insert_handlers(std::unordered_map<std::string,request_handler> &handlers) {
    http_request_handlers=handlers;
}

void listener::on_accept(beast::error_code ec, tcp::socket socket) {

    if (ec) {
        fail(ec, "accept");
        return; // To avoid infinite loop
    } else {

        // Create the session and run it
        auto session=get_session(std::move(socket));
        if(!http_request_handlers.empty())
            session->request_handlers=http_request_handlers;
            session->run();
        std::cout<<"Start accepting\n";
    }
    if (!stop_requested.load()) {
        do_accept();
    } else {
        // If stopping, cancel any pending operations in the session
        socket.cancel();
        // Close the socket to initiate the close sequence
        socket.close();
    }
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

std::shared_ptr<session> listener::get_session(tcp::socket socket){
    std::string doc_root;
    switch(_current_type){
        case SESSION_TYPE::HTTP:
            //std::cout<<"Set document root: ";std::cin>>doc_root;
            return http_session::create_session(std::move(socket),doc_root_);
        default:
            return nullptr;
    }
}

void listener::set_doc_root(std::string &docRoot) {
    doc_root_ = docRoot;
}
