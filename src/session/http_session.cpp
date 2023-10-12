//
// Created by kamil on 10.09.23.
//

#include "http_session.h"


template<class Body, class Allocator>
http::message_generator
http_session::handle_get(
        beast::string_view doc_root,
        http::request<Body, http::basic_fields<Allocator>> &&req) {

    // Returns a bad request response
    auto const bad_request =
            [&req](beast::string_view why) {
                http::response<http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = std::string(why);
                res.prepare_payload();
                return res;
            };

    // Returns a not found response
    auto const not_found =
            [&req](beast::string_view target) {
                http::response<http::string_body> res{http::status::not_found, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "The resource '" + std::string(target) + "' was not found.";
                res.prepare_payload();
                return res;
            };

    // Returns a server error response
    auto const server_error =
            [&req](beast::string_view what) {
                http::response<http::string_body> res{http::status::internal_server_error, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "An error occurred: '" + std::string(what) + "'";
                res.prepare_payload();
                return res;
            };

    // Make sure we can handle the method
    if (req.method() != http::verb::get &&
        req.method() != http::verb::head)
        return bad_request("Unknown HTTP-method");

    // Request path must be absolute and not contain "..".
    if (req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != beast::string_view::npos)
        return bad_request("Illegal request-target");

    // Build the path to the requested file
    std::string path = path_cat(doc_root, req.target());
    if (req.target().back() == '/')
        path.append("index.html");

    // Attempt to open the file
    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if (ec == beast::errc::no_such_file_or_directory)
        return not_found(req.target());

    // Handle an unknown error
    if (ec)
        return server_error(ec.message());

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if (req.method() == http::verb::head) {
        http::response<http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }

    // Respond to GET request
    http::response<http::file_body> res{
            std::piecewise_construct,
            std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version())};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return res;
}

http::message_generator
http_session::handle_post(
        http::request<http::string_body> &&req) {
    // Returns a bad request response
    auto const bad_request =
            [&req](beast::string_view why) {
                http::response<http::string_body> res{http::status::bad_request, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = std::string(why);
                res.prepare_payload();
                return res;
            };

    // Returns a not found response
    auto const not_found =
            [&req](beast::string_view target) {
                http::response<http::string_body> res{http::status::not_found, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "The resource '" + std::string(target) + "' was not found.";
                res.prepare_payload();
                return res;
            };

    // Returns a server error response
    auto const server_error =
            [&req](beast::string_view what) {
                http::response<http::string_body> res{http::status::internal_server_error, req.version()};
                res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
                res.set(http::field::content_type, "text/html");
                res.keep_alive(req.keep_alive());
                res.body() = "An error occurred: '" + std::string(what) + "'";
                res.prepare_payload();
                return res;
            };
    auto handler = request_handlers.find(req.target());
    if (handler != request_handlers.end()) {


        // Make sure we can handle the method
        if (req.method() != http::verb::post &&
            req.method() != http::verb::head)
            return bad_request("Unknown HTTP-method");

        // Attempt to open the file
        beast::error_code er;

        // Handle an unknown error
        if (er)
            return server_error(er.message());

        /*// Respond to HEAD request
        boost::beast::http::response<boost::beast::http::string_body> res;
        // Parse the POST data into a map
        std::unordered_map<std::string, std::string> postData;
        std::string data = req.body();
        std::istringstream iss(data);
        // Parse the data into key-value pairs
        std::string key, value;
        while (std::getline(iss, key, '&')) {
            size_t equalsPos = key.find('=');
            if (equalsPos != std::string::npos) {
                value = key.substr(equalsPos + 1);
                key = key.substr(0, equalsPos);
                std::cout << key << " " << value << std::endl;
                // URL decode the value if needed
                // (e.g., replace '+' with ' ' and percent-encoded characters)
                // You can use a URL decoding library for this.
                postData[key] = value;
            }
        }

        res.version(req.version());
        res.result(boost::beast::http::status::ok);
        res.set(boost::beast::http::field::server, "Boost HTTP Server");
        res.body() = "Response body";
        res.prepare_payload();*/
        return handler->second(req[http::field::content_type],req.body());
    } else {
        std::cout<<"Not found"<<std::endl;
        return not_found(req.target());
    }
}

void http_session::run() {
    net::dispatch(_tcp_stream.get_executor(),
                  beast::bind_front_handler(
                          [self = shared_from_this()]() {
                              self->do_read();
                          }
                  )
    );
}

void http_session::do_read() {
    _request = {};

    _tcp_stream.expires_after(std::chrono::seconds(30));

    http::async_read(_tcp_stream,
                     _buffer,
                     _request,
                     beast::bind_front_handler(
                             [self = shared_from_this()](beast::error_code er, std::size_t bytes) {
                                 self->on_read(er, bytes);
                             }
                     )
    );
}

void http_session::on_read(beast::error_code er, std::size_t bytes) {
    boost::ignore_unused(bytes);

    if (er == http::error::end_of_stream)
        return do_close();
    if (er)
        return fail(er, "read");
    if (_request.method() == http::verb::get) {
        std::cout<<"GET"<<std::endl;
        send_response(handle_get(*_doc_root,
                                 std::move(_request)));
    }
    else if (_request.method() == http::verb::post){
        std::cout<<"POST"<<std::endl;
        send_response(handle_post(std::move(_request)));
    }
}

void http_session::on_write(beast::error_code er, std::size_t bytes) {
    boost::ignore_unused(bytes);

    if (er)
        return fail(er, "write");

    do_read();
}

void http_session::do_close() {
    beast::error_code er;
    _tcp_stream.socket().shutdown(tcp::socket::shutdown_send, er);
}

void http_session::send_response(http::message_generator &&ms) {
    bool keep_alive = ms.keep_alive();

    beast::async_write(
            _tcp_stream,
            std::move(ms),
            beast::bind_front_handler(
                    [self = shared_from_this()](beast::error_code er, std::size_t bytes) {
                        self->on_write(er, bytes);
                    }
            )
    );
}

std::shared_ptr<session> http_session::create_session(tcp::socket socket, std::string doc_root,std::unordered_map<std::string,request_handler> &r_handlers) {
    return std::make_shared<http_session>(std::move(socket), std::make_shared<std::string const>(doc_root),r_handlers);
}


