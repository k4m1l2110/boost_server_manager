#include "../server/server.h"

#include "../argon2/hash_utils.hpp"

void wss_server::db_connect(boost::asio::yield_context yield)
{

    // This error_code and diagnostics will be filled if an
    // operation fails. We will check them for every operation we perform.
    mysql::error_code er;

    // Hostname resolution
    auto endpoints = this->_resolver->async_resolve("mysql", boost::mysql::default_port_string, yield[er]);
    mysql::throw_on_error(er);
    // Connect to server
    this->database.async_connect(*endpoints.begin(), this->params, this->diag, yield[er]);
    if (er)
    {
        // Handle error
        std::cerr << "Failed to connect to the server: " << er.message() << std::endl;
        mysql::throw_on_error(er, this->diag);
    }
    std::cout << "Server has been connected to database successfully" << std::endl;
}

void wss_server::query(boost::asio::yield_context yield)
{
    mysql::error_code er;

    mysql::statement stmt = this->database.async_prepare_statement(
        "SELECT name, email FROM user WHERE user_id = ?",
        this->diag,
        yield[er]);
    if (er)
    {
        // Handle error
        std::cerr << "Failed to prepare statement: " << er.message() << std::endl;
        mysql::throw_on_error(er, this->diag);
    }

    mysql::results result;

    this->database.async_execute_statement(stmt, std::make_tuple("1"), result, this->diag, yield[er]);
    if (er)
    {
        std::cerr << "Failed to execute a statement: " << er.message() << std::endl;
        mysql::throw_on_error(er, this->diag);
    }

    for (mysql::row_view employee : result.rows())
    {
        std::cerr << employee.at(0) << std::endl;
    }

    this->database.async_close(diag, yield[er]);
    mysql::throw_on_error(er, diag);
}

bool wss_server::check_email_duplicate(const std::string &email, boost::asio::yield_context yield)
{
    mysql::error_code er;

    mysql::statement stmt = this->database.async_prepare_statement(
        "SELECT email FROM user WHERE email = ?",
        this->diag,
        yield[er]);
    if (er)
    {
        std::cerr << "Failed to prepare statement: " << er.message() << std::endl;
        mysql::throw_on_error(er, this->diag);
        return 0;
    }

    mysql::results result;

    this->database.async_execute_statement(stmt, std::make_tuple(email), result, this->diag, yield[er]);
    if (er)
    {
        std::cerr << "Failed to execute a statement: " << er.message() << std::endl;
        mysql::throw_on_error(er, this->diag);
        return 0;
    }

    bool duplicateFound = !result.rows().empty();

    return duplicateFound;
}

bool wss_server::check_user_duplicate(const std::string &name, boost::asio::yield_context yield)
{
    mysql::error_code er;

    mysql::statement stmt = this->database.async_prepare_statement(
        "SELECT name FROM user WHERE name = ?",
        this->diag,
        yield[er]);
    if (er)
    {
        std::cerr << "Failed to prepare statement: " << er.message() << std::endl;
        mysql::throw_on_error(er, this->diag);
        return 0;
    }

    mysql::results result;

    this->database.async_execute_statement(stmt, std::make_tuple(name), result, this->diag, yield[er]);
    if (er)
    {
        std::cerr << "Failed to execute a statement: " << er.message() << std::endl;
        mysql::throw_on_error(er, this->diag);
        return 0;
    }

    bool duplicateFound = !result.rows().empty();

    return duplicateFound;
}

http::status wss_server::create_user(
    const std::string &name,
    const std::string &email,
    const std::string &password,
    roles role,
    boost::asio::yield_context yield
    )
{
    try
    {
        mysql::error_code er;

        if (check_user_duplicate(name, yield))
        {
            std::cerr << "Username " << name << " already exist" << std::endl;
            return http::status::conflict;
        }
        if (check_email_duplicate(email, yield))
        {
            std::cerr << "Email address " << email << " already exist" << std::endl;
            return http::status::conflict;
        }

        // Prepare the INSERT statement
        mysql::statement stmt = this->database.async_prepare_statement(
            "INSERT INTO user (name, email, password, salt) VALUES (?, ?, ?,?)",
            this->diag,
            yield[er]);
        if (er)
        {
            std::cerr << "Failed to prepare statement: " << er.message() << std::endl;
            mysql::throw_on_error(er, this->diag);
            return http::status::conflict;
        }

        uint8_t *hashed_password = new uint8_t[HASHLEN];
        uint8_t *salt = reinterpret_cast<uint8_t*>(generate_random(SALTLEN));
        // Generate the hashed password
        if (get_hash(password, salt, hashed_password))
        {

            boost::mysql::results result;
            std::string hashed_password_str(reinterpret_cast<char *>(hashed_password), HASHLEN);
            std::string salt_str(reinterpret_cast<char *>(salt), SALTLEN);

            // Execute the INSERT statement with the user's information
            this->database.async_execute_statement(
                stmt,
                std::make_tuple(name, email, hashed_password_str, salt_str),
                result,
                this->diag,
                yield[er]);

            if (er)
            {
                std::cerr << "Failed to execute the INSERT statement: " << er.message() << std::endl;
                mysql::throw_on_error(er, this->diag);
            }

            std::cout << "User " + name + " successfully created" << std::endl;
            return http::status::ok;
        }
        else
        {
            std::cerr << "There was an error with creating a user" << std::endl;
            return http::status::conflict;
        }
        delete[] hashed_password;
        delete[] salt;
    }
    catch (std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return http::status::conflict;
    }
}

http::status wss_server::authenticate_user(const std::string& name,
                                           const std::string& password,
                                           uint8_t *token,
                                           boost::asio::yield_context yield)
{
    bool isUserFound = false;
    bool isAuthenticated = false;
    std::string query;
    
    if (check_user_duplicate(name, yield))
    {
        query = "SELECT name, password, salt FROM user WHERE name = ?";
        isUserFound = true;
    }
    else if (check_email_duplicate(name, yield))
    {
        query = "SELECT name, password, salt FROM user WHERE email = ?";
        isUserFound = true;
    }
    
    if (!isUserFound)
        return http::status::not_found;
    
    mysql::error_code er;
    mysql::statement stmt = this->database.async_prepare_statement(query, this->diag, yield[er]);
    if (er)
    {
        std::cerr << "Failed to prepare statement: " << er.message() << std::endl;
        mysql::throw_on_error(er, this->diag);
        return http::status::conflict;
    }

    mysql::results result;
    this->database.async_execute_statement(stmt, std::make_tuple(name), result, this->diag, yield[er]);
    if (er)
    {
        std::cerr << "Failed to execute a statement: " << er.message() << std::endl;
        mysql::throw_on_error(er, this->diag);
        return http::status::conflict;
    }

    const std::string stored_name = result.rows().at(0).at(0).get_string();
    const std::string stored_password = result.rows().at(0).at(1).get_string();
    const std::string stored_salt = result.rows().at(0).at(2).get_string();
    
    uint8_t *salt = reinterpret_cast<uint8_t *>(const_cast<char *>(stored_salt.c_str()));
    uint8_t *hash = reinterpret_cast<uint8_t *>(const_cast<char *>(stored_password.c_str()));

    isAuthenticated = verify_password(password, hash, salt);
    delete []hash;
    delete []salt;
    if (isAuthenticated)
    {
        if (isUserFound)
        {
            // Insert a new secret into the database
            mysql::statement insertStmt = this->database.async_prepare_statement(
                "UPDATE user SET secret = ? WHERE name = ?", this->diag, yield[er]);
            if (er)
            {
                std::cerr << "Failed to prepare statement: " << er.message() << std::endl;
                mysql::throw_on_error(er, this->diag);
                return http::status::conflict;
            }
            
            
            const std::string secret_str = generate_random(32);
            uint8_t *secret = reinterpret_cast<uint8_t*>(const_cast<char *>(secret_str.c_str()));

            this->database.async_execute_statement(
                insertStmt,
                std::make_tuple(stored_name, secret_str),
                result,
                this->diag,
                yield[er]
            );
            
            if (er)
            {
                std::cerr << "Failed to execute the INSERT statement: " << er.message() << std::endl;
                mysql::throw_on_error(er, this->diag);
                return http::status::conflict;
            }

            session_data::session session_storage;
            session_storage.set_name(stored_name);
            session_storage.set_role(session_data::role::USER);

            std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
            std::chrono::seconds expiration_seconds(60/*seconds*/);
            session_storage.set_creation_time(std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch().count());
            session_storage.set_expiration_time(std::chrono::time_point_cast<std::chrono::seconds>(now + expiration_seconds).time_since_epoch().count());


            std::string data;
            session_storage.SerializeToString(&data);

            if(get_hash(data,secret,token)){
            std::cout << "User authenticated successfully" << std::endl;
            return http::status::ok;
            }
            else{
                std::cerr << "Error during token creation" <<std::endl;
                return http::status::conflict;
            }
        }
        
    }
    else
    {
        std::cerr << "Incorrect password" << std::endl;
        return http::status::unauthorized;
    }
}
