#include "server/server.h"

void server::run_listeners() {
    try {
        std::vector<std::thread> v;
        v.reserve(_io_threads_num - 1);
        for (auto i = _io_threads_num - 1; i > 0; --i) {
            v.emplace_back([this] {
                _ioc.run();
            });
        }
        _ioc.run();

    } catch (std::exception& er) {
        std::cerr<<er.what()<<std::endl;
        throw er;
    }
}

void server::create_listener(unsigned _port,SESSION_TYPE _type,std::string doc_root) {
    auto new_listener = std::make_shared<listener>(
            _ioc,
            tcp::endpoint(_ip,_port),
            _type
            );
    if(!doc_root.empty())
        new_listener->set_doc_root(doc_root);
    new_listener->run();

    _listeners.push_back(new_listener);
}

void server::stop_listeners() {
    for (auto& _listener : _listeners) {
        _listener->stop();  //TODO: Implement the stop method in listener
    }
    _listeners.clear();

}

void server::start_listeing(){
    _main_thread=std::thread([this]{
        run_listeners();
    });

}

void server::stop_listenening() {
    stop_listeners();
    if (_main_thread.joinable()) {
        _main_thread.join();
    }
}

const ip::address &server::get_ip() const {
    return _ip;
}

const std::string &server::get_ID() const {
    return _ID;
}

std::vector<unsigned> server::get_ports(){
    std::vector<unsigned> _p;
    for(const auto& l:_listeners){
        _p.push_back(l->get_port());
    }
    return _p;
}

bool server::insert_handlers(unsigned listener_port,std::string path,request_handler handler) {
    auto c_listener=std::find_if(_listeners.begin(),_listeners.end(),[&listener_port](std::shared_ptr<listener> l){
        return listener_port==l->get_port();
    });
    if(c_listener!=_listeners.end()) {
        (*c_listener)->insert_handler(path, handler);
        return 1;
    }
    else {
        std::cout << "Path not found" << std::endl;//FIXME: change to log into error logger
        return 0;
        }
}