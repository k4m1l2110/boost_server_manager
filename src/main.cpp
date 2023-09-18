
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

void log_message(WINDOW* log_window, const std::string& message) {
    wprintw(log_window, "%s\n", message.c_str());
    wrefresh(log_window);
}


int main(int argc, char *argv[]) {

    std::ifstream _icon("icon.txt");
    std::string icon;

    std::vector<std::shared_ptr<server>> servers;

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
               "Choose option:",
               "1. Create server instance",
               "2. List servers",
               "3. Errors",
               "e. Exit");

                    /* Print it on to the real screen */
        std::string id,adr,ch2;
        int i=0;
        unsigned port;
        std::cin>>ch;
        try {
            switch (ch) {
                // The io_context is required for all I/O
                case '1':
                    system("clear");

                    std::cout<<"Server ID: ";std::cin>>id;
                    std::cout<<"IP address: ";std::cin>>adr;
                    servers.push_back(std::make_shared<server>(id,adr));
                    break;
                case '2':
                    while(ch2!="<") {

                        for(const auto &s:servers){
                        std::cout<<i++<<". Server: "<<s->get_ID()
                        <<" address: "<<s->get_ip()
                        <<" open ports: ";
                        for(auto p:s->get_ports())
                            std::cout<<p<<" | ";
                        std::cout<<std::endl;
                        }

                        std::cout << "Choose server to edit or go back(<): ";
                        std::cin >> ch2;
                        if(ch2=="<")
                            break;
                        system("clear");
                        if(atoi(ch2.c_str())>=0&&atoi(ch2.c_str())<servers.size()) {
                            std::cout << "Listener port: ";
                            std::cin >> port;
                            servers[atoi(ch2.c_str())]->create_listener(port,SESSION_TYPE::HTTP);
                        }
                        else {
                            std::cout <<"Wrong option or server number" <<std::endl;
                            getchar();
                        }
                        system("clear");
                        i=0;
                    }

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