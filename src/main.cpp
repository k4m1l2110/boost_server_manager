
#include <iostream>
#include <fstream>
#include "server.h"
#include <stdlib.h>
#include <ncurses.h>

#include <boost/version.hpp>

/*void wait(const std::chrono::duration<int, std::milli>& duration)
{
    std::this_thread::sleep_for(duration);
}*/

void log_message(WINDOW* log_window, const std::string& message) {
    wprintw(log_window, "%s\n", message.c_str());
    wrefresh(log_window);
}


int main(int argc, char *argv[]) {

    std::ifstream _icon("icon.txt");
    std::string icon;

    // Check command line arguments.
    /*if (argc != 5) {
        std::cerr <<
                  "Usage: http-server-async <address> <port> <doc_root> <threads>\n" <<
                  "Example:\n" <<
                  "    http-server-async 0.0.0.0 8080 . 1\n";
        return EXIT_FAILURE;
    }*/
    char ch;

    /*initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    // Create a log window
    WINDOW* log_window = newwin(10, 80, 10, 0);
    scrollok(log_window, TRUE);

    // Log a message
    wprintw(log_window, "Log message 1\n");
    wrefresh(log_window);*/

    if (_icon.is_open()) {
        std::string line;
        while (std::getline(_icon, line)) {
            icon += line + "\n";
        }
        _icon.close();
    }

    refresh();
    while (ch != 'e') {
        // Print the icon above the menu
        if (!icon.empty()) {
            printw(icon.c_str());
            printw("\n\n"); // Move to the next line
        }

        printf("%s\n%s\n%s\n%s\n",
               "Choose option",
               "1. Create server instance",
               "2. List servers",
               "3. Errors",
               "e. Exit");
                    /* Print it on to the real screen */
        std::cin>>ch;
        try {
            server _server_ins;
            switch (ch) {
                // The io_context is required for all I/O
                case '1':
                    _server_ins.create_listeners(static_cast<std::string>("."));
                    _server_ins.run_http();
                    std::cout<<"Listener created"<<std::endl;
                    getchar();
                    break;
                case '2':
                    _server_ins.stop_listeners();
                    std::cout<<"Stop listening"<<std::endl;
                    getchar();
                    break;
                default:
                    break;
            };
            system("clear");
        }
        catch (beast::error_code &er) {
            std::cerr<<"Boost.Beast error: " + er.message() << std::endl;
            return EXIT_FAILURE;
        }
        catch (std::exception &er) {
            std::cerr<<"Error: " + std::string(er.what())<<std::endl;
            return EXIT_FAILURE;
        }
    }
    //endwin();
    return EXIT_SUCCESS;
}