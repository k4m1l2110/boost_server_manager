#include "server.h"



void server::run_listeners() {
    try {
        std::vector<std::thread> v;
        _threads.reserve(_io_threads_num - 1);
        for (auto i = _io_threads_num - 1; i > 0; --i) {
            v.emplace_back([this] {
                _ioc.run();
            });
        }
        _ioc.run();

    } catch (std::exception& er) {
        throw er;
    }
}

void server::create_listener(unsigned _port,SESSION_TYPE _type) {
    auto new_listener = std::make_shared<listener>(
            _ioc,
            tcp::endpoint(_ip,_port)
            );

    new_listener->run();

    _listeners.push_back(new_listener);
}

void server::stop_listeners() {
    for (auto& _listener : _listeners) {
        _listener->stop();  // Implement the stop method in listener
    }
    _listeners.clear();

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
