#include "server.h"

void edit_server(const std::shared_ptr<server> &s){
    char ch;
    while(ch!='<') {
        std::cout << "1. Create listen port\n"
                  << "2. Start listening\n"
                  << "3. Stop listening\n"
                  << "< Go back" << std::endl;
        std::cin>>ch;
        switch(ch){
            case '1':
                unsigned port;
                std::cout << "Listener port: ";
                std::cin >> port;
                s->create_listener(port,SESSION_TYPE::HTTP);
                break;
            case '2':
                s->start_listeing();
                break;
            case '3':
                s->stop_listenening();
                break;
            default:
                break;
        };
        //system("clear");
    }

}

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

void server::start_listeing(){
    _main_thread=std::thread([this]{
        run_listeners();
    });
}

void server::stop_listenening() {
    stop_listeners();
    if (_main_thread.joinable()) {
        _main_thread.join(); // Wait for the listening thread to finish
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
