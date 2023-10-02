#ifndef SERVER_H
#define SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include "../listener/listener.h"
#include <ncurses.h>

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#define _HTTP_PORT 8080
#define _ADDR "0.0.0.0"



class server : public std::enable_shared_from_this<server> {
    friend void edit_server(const std::shared_ptr<server> &s);
private:
    std::shared_ptr<session> _current_session;
    std::vector<std::shared_ptr<listener>> _listeners;
    std::string _ID;
    net::ip::address _ip;
    net::io_context _ioc;
    unsigned _io_threads_num;
    std::thread _main_thread;

    void run_listeners();
    void stop_listeners();

public:
    server(const std::string ID,const std::string addr = _ADDR, int io_threads = 1) :
            _ID(ID),_ip(net::ip::make_address(addr)), _ioc(std::max<int>(1, io_threads)), _io_threads_num
            (io_threads) {std::cout<<"Server created on address: "<<addr<<std::endl;}

    const ip::address &get_ip() const;
    const std::string &get_ID() const;
    std::vector<unsigned> get_ports();
    void set_doc_root(const std::string &s);
    void create_listener(unsigned _port,SESSION_TYPE _type,std::string doc_root="");
    void start_listeing();
    void stop_listenening();


};

void edit_server(const std::shared_ptr<server> &s);

#endif


