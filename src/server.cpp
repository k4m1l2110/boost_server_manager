#include "server.h"



void server::run_http() {
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

void server::create_listeners(std::string doc_root) {
    auto new_listener = std::make_shared<listener>(
            _ioc,
            tcp::endpoint(_ip,_port),
            std::make_shared<std::string>(doc_root)
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
