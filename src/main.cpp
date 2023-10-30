
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <boost/version.hpp>
#include "server/main_server.hpp"

int main(int argc, char *argv[]) {

    try {
        ServerManager serverManager;
        serverManager.startServer();

        char ch;
        while (ch != 'e') {
            std::cin >> ch;
        }

        serverManager.stopServer();
    } catch (beast::error_code& er) {
        std::cerr << "Boost.Beast error: " + er.message() << std::endl;
        return EXIT_FAILURE;
    } catch (std::exception& er) {
        std::cerr << "Error: " + std::string(er.what()) << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
    //endwin();
    return EXIT_SUCCESS;
}