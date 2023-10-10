
#include <iostream>
#include <fstream>
#include "server.h"
#include <stdlib.h>
#include <ncurses.h>
#include <vector>

#include <boost/version.hpp>

/*void wait(const std::chrono::duration<int, std::milli>& duration)
{
    std::this_thread::sleep_for(duration);
}*/

void log_message(WINDOW *log_window, const std::string &message) {
    wprintw(log_window, "%s\n", message.c_str());
    wrefresh(log_window);
}


int main(int argc, char *argv[]) {

    std::ifstream _icon("icon.txt");
    std::string icon;
    std::shared_ptr<server> gui_server = std::make_shared<server>("MAIN", "0.0.0.0");
    gui_server->create_listener(8080, SESSION_TYPE::HTTP);
    try {


        std::vector<std::shared_ptr<server>> servers;
        gui_server->insert_handlers(8080, "/test",
                                    [](beast::basic_string_view<char> request_type, std::string req)-> http::response<http::string_body> {
                                        std::cout << "Received Request Body: " << req << std::endl;
                                        http::response<http::string_body> res;
                                        res.version(11);
                                        return res;
                                    });


        gui_server->start_listeing();
        char ch;
        while (ch != 'e') {
            std::cin >> ch;
        }
        gui_server->stop_listenening();
    }
    catch (beast::error_code &er) {
        std::cerr << "Boost.Beast error: " + er.message() << std::endl;
        return EXIT_FAILURE;
    }
    catch (std::exception &er) {
        std::cerr << "Error: " + std::string(er.what()) << std::endl;
        return EXIT_FAILURE;
    }
    //endwin();
    return EXIT_SUCCESS;
}