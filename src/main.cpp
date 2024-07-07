#include <iostream>
#include <stdlib.h>
#include <vector>
#include <memory>
#include <boost/version.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "server/server.h"

int main(int argc, char *argv[]) {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json("config.json", pt);


    try {
        boost::property_tree::read_json("config.json", pt);

        std::vector<std::shared_ptr<server>> servers;

        // Iterate over each server object in the servers array
        for (auto& s : pt.get_child("servers")) {
            std::string type = s.second.get<std::string>("type");
            int port = s.second.get<int>("port");
            std::string address = s.second.get<std::string>("address");
            std::string name = s.second.get<std::string>("name");

            SESSION_TYPE type_;
            if (type == "HTTP") {
                type_ = SESSION_TYPE::HTTP;
            } else if (type == "HTTPS") {
                type_ = SESSION_TYPE::HTTPS;
            } else if (type == "WS") {
                type_ = SESSION_TYPE::WS;
            } else if (type == "WSS") {
                type_ = SESSION_TYPE::WSS;
            } else {
                type_ = SESSION_TYPE::HTTP;
            }

            std::string doc_root = s.second.get<std::string>("doc_root");

            std::shared_ptr<server> server_ptr = std::make_shared<server>("MAIN", "0.0.0.0", 1);
            servers.push_back(server_ptr);
            servers.back()->create_listener(port, type_, doc_root);
            servers.back()->start_listeing();
            std::cout << "Server started on address: " << address << " port: " << port <<
                      "\nType exit to close server" << std::endl;
        }

        std::string line;
        while (std::getline(std::cin, line)) {
            if (line == "exit") {
                for (auto& s : servers) {
                    s->stop_listenening();
                }
                break;
            }
        }

    } catch (beast::error_code& er) {
        std::cerr << "Boost.Beast error: " + er.message() << std::endl;
        return EXIT_FAILURE;
    } catch (std::exception& er) {
        std::cerr << "Error: " + std::string(er.what()) << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}