//
// Created by kamil on 19.10.23.
//
//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

#ifndef SERVER_CERT_HPP
#define SERVER_CERT_HPP

#include <boost/asio/ssl.hpp>
#include <fstream>

inline void load_server_certificate(boost::asio::ssl::context& ctx) {
    // Load the certificate file
    std::ifstream cert_file("cert.pem");
    if (!cert_file) {
        throw std::runtime_error("Failed to open certificate file");
    }
    std::string cert_contents(
            (std::istreambuf_iterator<char>(cert_file)),
            std::istreambuf_iterator<char>());

    // Load the private key file
    std::ifstream key_file("key.pem");
    if (!key_file) {
        throw std::runtime_error("Failed to open private key file");
    }
    std::string key_contents(
            (std::istreambuf_iterator<char>(key_file)),
            std::istreambuf_iterator<char>());

    // Set the password callback if needed
    ctx.set_password_callback([](std::size_t, boost::asio::ssl::context_base::password_purpose) {
        return "test";
    });

    // Configure SSL context with certificate and private key
    ctx.set_options(
            boost::asio::ssl::context::default_workarounds |
            boost::asio::ssl::context::no_sslv2 |
            boost::asio::ssl::context::single_dh_use);

    ctx.use_certificate_chain(boost::asio::buffer(cert_contents.data(), cert_contents.size()));
    ctx.use_private_key(
            boost::asio::buffer(key_contents.data(), key_contents.size()),
            boost::asio::ssl::context::file_format::pem);

    // Load and set DH parameters from file
    std::ifstream dh_file("dh.pem");
    if (dh_file) {
        std::string dh_contents(
                (std::istreambuf_iterator<char>(dh_file)),
                std::istreambuf_iterator<char>());
        ctx.use_tmp_dh(boost::asio::buffer(dh_contents.data(), dh_contents.size()));
    } else {
        // Handle the case where DH parameters file is missing
        // You can log an error or take other appropriate action
    }
}

#endif