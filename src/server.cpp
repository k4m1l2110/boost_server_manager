#include "server.h"

void server::run_http(std::string doc_root) {
    try{
        std::make_shared<listener>(
                _ioc,
                tcp::endpoint{_ip, _port},
                std::make_shared<std::string>(doc_root))->run();
        std::vector<std::thread> v;
        _threads.reserve(_io_threads_num - 1);
        for (auto i = _io_threads_num - 1; i > 0; --i)
            v.emplace_back(
                    [this] {
                        _ioc.run();
                    });
        _ioc.run();
    }
    catch(std::exception &er){
        throw er;
    }
}