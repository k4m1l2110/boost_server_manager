#ifndef _MAIN_SERVER_HPP
#define _MAIN_SERVER_HPP

#include "server.h"

class ServerManager {
public:
    ServerManager() {
        gui_server_ = std::make_shared<server>("MAIN", "0.0.0.0");
        setupServer();
    }

    void startServer() {
        gui_server_->start_listeing();
    }

    void stopServer() {
        gui_server_->stop_listenening();
    }

private:
    std::shared_ptr<server> gui_server_;
    std::vector<std::shared_ptr<server>> servers;

    void setupServer() {
        gui_server_->create_listener(8080, SESSION_TYPE::HTTPS, "/home/kamil/Projekty/C_C++/CPP/WebServer/webtool_manager");
        //gui_server_->create_listener(8090, SESSION_TYPE::WSS);
        setupHandlers();
    }

    void setupHandlers() {
        gui_server_->insert_handlers(8080, "/api/get_servers",
                                    [this](beast::basic_string_view<char> request_type,
                                               std::string req) -> http::response<http::string_body> {

                                        std::vector<std::string> t(servers.size());
                                        for (int i = 0; i < servers.size(); i++) {
                                            t[i] = servers[i]->get_ID();
                                        }

                                        nlohmann::json jsonResponse;
                                        jsonResponse["servers"] = t; // Add the list of numbers to the JSON response

                                        // Create an HTTP response object with content
                                        http::response<http::string_body> res{http::status::ok, 11};
                                        res.set(http::field::content_type, "application/json");
                                        res.body() = jsonResponse.dump(); // Serialize the JSON to a string
                                        res.prepare_payload();
                                        return res;
                                    });
        gui_server_->insert_handlers(8080, "/api/get_server",
                                    [this](beast::basic_string_view<char> request_type,
                                               std::string req) -> http::response<http::string_body> {
                                        std::cout << "Received Request Body: " << req << std::endl;
                                        nlohmann::json request_id = nlohmann::json::parse(req);
                                        std::cout << "ID:" << request_id["id"] << std::endl;
                                        auto req_server=std::find_if(servers.begin(),servers.end(),
                                                                     [&request_id](const std::shared_ptr<server> &s){
                                                                         return s->get_ID()==request_id["id"];
                                                                     });
                                        if(req_server!=servers.end()){
                                            nlohmann::json res_data;
                                            res_data["address"]=(*req_server)->get_ip().to_string();
                                            res_data["ports"]=(*req_server)->get_ports();
                                            http::response<http::string_body> res{http::status::ok, 11};
                                            res.set(http::field::content_type, "application/json");
                                            res.body() = res_data.dump();
                                            res.prepare_payload();
                                            return res;
                                        }
                                        else {
                                            http::response<http::string_body> res{http::status::not_found, 11};
                                            res.set(http::field::content_type, "application/json");
                                            res.prepare_payload();
                                            return res;
                                        }
                                    });
        gui_server_->insert_handlers(8080, "/api/create_server",
                                    [this](beast::basic_string_view<char> request_type,
                                               std::string req) -> http::response<http::string_body> {

                                        nlohmann::json server_data = nlohmann::json::parse(req.c_str());
                                        const std::string address = server_data["address"], id = server_data["id"];
                                        std::cout << address <<" "<< id << std::endl;
                                        servers.emplace_back(
                                                std::make_shared<server>(id, address)
                                        );

                                        http::response<http::string_body> res{http::status::ok, 11};


                                        return res;
                                    });
    }
};

#endif