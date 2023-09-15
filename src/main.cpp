
#include <iostream>
#include "server.h"

#include <ncurses.h>

#include <boost/version.hpp>

/*void wait(const std::chrono::duration<int, std::milli>& duration)
{
    std::this_thread::sleep_for(duration);
}*/



int main(int argc, char *argv[]) {
    // Check command line arguments.
    if (argc != 5) {
        std::cerr <<
                  "Usage: http-server-async <address> <port> <doc_root> <threads>\n" <<
                  "Example:\n" <<
                  "    http-server-async 0.0.0.0 8080 . 1\n";
        return EXIT_FAILURE;
    }

    // The io_context is required for all I/O
    try{
        server _server_ins(argv[1],std::atoi(argv[2]),std::atoi(argv[4]));
        _server_ins.run_http(static_cast<std::string>(argv[3]));
    }
    catch(beast::error_code &er){
        std::cerr << er.message() << "\n";
        return EXIT_FAILURE;
    }
    catch(std::exception &er){
        std::cerr << er.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}