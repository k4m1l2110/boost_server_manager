#include "./listener.h"

void listener::do_accept() {
    _acceptor.async_accept(
            net::make_strand(_ioc),
            beast::bind_front_handler(
                    &listener::on_accept,
                    shared_from_this()));

}

void listener::insert_handler(std::string path,request_handler handler){
http_request_handlers.insert(std::make_pair(path,handler));
}

void listener::on_accept(beast::error_code ec, tcp::socket socket) {

    if (ec) {
        fail(ec, "accept");
        return;
    } else {
        auto session=get_session(std::move(socket));
        std::cout<<"Start accepting\n";
        session->run();
    }
    if (!stop_requested.load()) {
        do_accept();
    } else {
        socket.cancel();
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
            return http_session::create_session(std::move(socket),doc_root_, http_request_handlers);
        case SESSION_TYPE::HTTPS:
            return https_session::create_session(std::move(socket),doc_root_,http_request_handlers,ssl_ctx);
        case SESSION_TYPE::WS:
            return ws_session::create_session(std::move(socket));
        case SESSION_TYPE::WSS:
            return wss_session::create_session(std::move(socket),ssl_ctx);
        default:
            return nullptr;
    }
}

void listener::set_doc_root(std::string &docRoot) {
    doc_root_ = docRoot;
}
