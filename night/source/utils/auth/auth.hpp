#pragma once

#include <string>
#include <windows.h>
#include <conio.h>
#include "security/security.hpp"
#include "security/continuous_validator.hpp"

#include "security/encryption.hpp"

class night_auth {
private:
    std::string username;
    std::string password;
    std::string token;
    bool logged_in;
    std::string api_url;
    std::string hwid;
    std::unique_ptr<continuous_validator> validator;

    // execute powershell command and get result
    std::string execute_powershell(const std::string& command);

    // simple json parsing
    std::string extract_value(const std::string& json, const std::string& key);

    // escape quotes for powershell
    std::string escape_for_powershell(const std::string& input);

    // generate hardware id
    std::string generate_hwid();

public:
    // constructor
    night_auth(const std::string& api_url = "https://getnight.wtf/api");
    night_auth(const secure_string& api_url);
    night_auth(const obfuscated_string& api_url);

    // destructor
    ~night_auth();

    // main authentication method
    bool start();

    // set credentials
    void set_credentials(const std::string& user, const std::string& pass);

    // check if user is logged in
    bool is_logged_in() const;

    // get authentication token
    std::string get_token() const;

    // get username
    std::string get_username() const;

    // logout
    void logout();

    // set api url
    void set_api_url(const std::string& url);

    // get api url
    std::string get_api_url() const;

    // interactive login (with console input)
    bool interactive_login();

    // silent login (credentials must be set first)
    bool silent_login();

    // check token validity
    bool is_token_valid() const;

    // get user info from token
    std::string get_user_info() const;

    // get password (for saving credentials)
    std::string get_password() const;

    // hwid management
    std::string get_hwid() const;
    void set_hwid(const std::string& hwid);
    bool validate_hwid() const;

    // security methods
    bool perform_security_checks() const;
    void start_continuous_validation();
    void stop_continuous_validation();
    bool is_continuously_valid() const;
};
