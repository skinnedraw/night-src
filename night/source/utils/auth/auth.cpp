#include "auth.hpp"
#include <iostream>
#include <sstream>
#include <windows.h>
#include <intrin.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")

night_auth::night_auth(const std::string& api_url)
    : logged_in(false), api_url(api_url) {
    // generate hwid on construction
    hwid = generate_hwid();

    // initialize continuous validator
    validator = std::make_unique<continuous_validator>(api_url, 30000); // 30 second intervals
}

night_auth::night_auth(const secure_string& api_url)
    : logged_in(false), api_url(api_url.decrypt()) {
    // generate hwid on construction
    hwid = generate_hwid();

    // initialize continuous validator
    validator = std::make_unique<continuous_validator>(api_url.decrypt(), 30000); // 30 second intervals
}

night_auth::night_auth(const obfuscated_string& api_url)
    : logged_in(false), api_url(api_url.decrypt()) {
    // generate hwid on construction
    hwid = generate_hwid();

    // initialize continuous validator
    validator = std::make_unique<continuous_validator>(api_url.decrypt(), 30000); // 30 second intervals
}

std::string night_auth::generate_hwid() {
    std::string hwid_data;

    // get cpu info
    int cpu_info[4];
    __cpuid(cpu_info, 0);
    hwid_data += std::to_string(cpu_info[0]) + std::to_string(cpu_info[1]) + std::to_string(cpu_info[2]) + std::to_string(cpu_info[3]);

    // get motherboard serial (if available)
    HKEY hkey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", 0, KEY_READ, &hkey) == ERROR_SUCCESS) {
        char buffer[256];
        DWORD buffer_size = sizeof(buffer);
        if (RegQueryValueExA(hkey, "BaseBoardSerialNumber", NULL, NULL, (LPBYTE)buffer, &buffer_size) == ERROR_SUCCESS) {
            hwid_data += std::string(buffer);
        }
        RegCloseKey(hkey);
    }

    // get mac address
    IP_ADAPTER_INFO adapter_info[16];
    DWORD adapter_info_size = sizeof(adapter_info);
    if (GetAdaptersInfo(adapter_info, &adapter_info_size) == ERROR_SUCCESS) {
        for (int i = 0; i < 16; i++) {
            if (adapter_info[i].Type == MIB_IF_TYPE_ETHERNET) {
                for (int j = 0; j < 6; j++) {
                    hwid_data += std::to_string(adapter_info[i].Address[j]);
                }
                break;
            }
        }
    }

    // get computer name
    char computer_name[256];
    DWORD computer_name_size = sizeof(computer_name);
    if (GetComputerNameA(computer_name, &computer_name_size)) {
        hwid_data += std::string(computer_name);
    }

    // create hash of all data
    std::hash<std::string> hasher;
    size_t hash_value = hasher(hwid_data);

    // convert to hex string
    std::stringstream ss;
    ss << std::hex << hash_value;
    std::string hwid_hex = ss.str();

    // pad with zeros if needed
    while (hwid_hex.length() < 16) {
        hwid_hex = "0" + hwid_hex;
    }

    return hwid_hex.substr(0, 16); // return first 16 characters
}

night_auth::~night_auth() {
    // cleanup if needed
}

std::string night_auth::execute_powershell(const std::string& command) {
    std::string full_command = "powershell.exe -command \"" + command + "\"";

    HANDLE h_pipe_read, h_pipe_write;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    if (!CreatePipe(&h_pipe_read, &h_pipe_write, &sa, 0)) {
        return "";
    }

    STARTUPINFOA si = { sizeof(STARTUPINFOA) };
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = h_pipe_write;
    si.hStdError = h_pipe_write;

    PROCESS_INFORMATION pi;

    if (!CreateProcessA(NULL, (LPSTR)full_command.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(h_pipe_read);
        CloseHandle(h_pipe_write);
        return "";
    }

    CloseHandle(h_pipe_write);

    std::string result;
    char buffer[4096];
    DWORD bytes_read;

    while (ReadFile(h_pipe_read, buffer, sizeof(buffer) - 1, &bytes_read, NULL) && bytes_read > 0) {
        buffer[bytes_read] = '\0';
        result += buffer;
    }

    CloseHandle(h_pipe_read);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return result;
}

std::string night_auth::extract_value(const std::string& json, const std::string& key) {
    size_t start = json.find("\"" + key + "\":");
    if (start == std::string::npos) return "";

    start = json.find(":", start);
    if (start == std::string::npos) return "";

    start++; // skip ':'
    while (start < json.length() && (json[start] == ' ' || json[start] == '"')) start++;

    size_t end = start;
    if (json[start - 1] == '"') {
        // string value
        while (end < json.length() && json[end] != '"') end++;
    }
    else {
        // number or boolean
        while (end < json.length() && json[end] != ',' && json[end] != '}') end++;
    }

    return json.substr(start, end - start);
}

std::string night_auth::escape_for_powershell(const std::string& input) {
    std::string escaped = input;
    size_t pos = 0;
    while ((pos = escaped.find("'", pos)) != std::string::npos) {
        escaped.replace(pos, 1, "''");
        pos += 2;
    }
    return escaped;
}

bool night_auth::start() {
    return interactive_login();
}

void night_auth::set_credentials(const std::string& user, const std::string& pass) {
    username = user;
    password = pass;
}

bool night_auth::is_logged_in() const {
    return logged_in && !token.empty();
}

std::string night_auth::get_token() const {
    return token;
}

std::string night_auth::get_username() const {
    return username;
}

void night_auth::logout() {
    // stop continuous validation
    stop_continuous_validation();

    token.clear();
    username.clear();
    password.clear();
    logged_in = false;
}

void night_auth::set_api_url(const std::string& url) {
    api_url = url;
}

std::string night_auth::get_api_url() const {
    return api_url;
}

bool night_auth::interactive_login() {
    if (username.empty() || password.empty()) {
        std::cout << "enter username/email: ";
        std::cin >> username;

        std::cout << "enter password: ";
        std::string input_password;
        char ch;
        while ((ch = _getch()) != '\r') {
            if (ch == '\b') {
                if (!input_password.empty()) {
                    input_password.pop_back();
                    std::cout << "\b \b";
                }
            }
            else {
                input_password += ch;
                std::cout << '*';
            }
        }
        std::cout << std::endl;
        password = input_password;
    }

    return silent_login();
}

bool night_auth::silent_login() {
    if (username.empty() || password.empty()) {
        return false;
    }

    // perform security checks first
    if (!perform_security_checks()) {
        return false;
    }

    // escape quotes in username, password, and hwid for powershell
    std::string escaped_username = escape_for_powershell(username);
    std::string escaped_password = escape_for_powershell(password);
    std::string escaped_hwid = escape_for_powershell(hwid);

    // create powershell command to make http request (with HWID)
    std::string ps_command =
        "$body = @{username='" + escaped_username + "';password='" + escaped_password + "';hwid='" + escaped_hwid + "'} | convertto-json; " +
        "$response = invoke-restmethod -uri '" + api_url + "/login.php' -method post -body $body -contenttype 'application/json'; " +
        "$response | convertto-json";

    std::string response = execute_powershell(ps_command);

    if (response.empty()) {
        return false;
    }

    // parse response
    std::string success = extract_value(response, "success");

    if (success == "true") {
        token = extract_value(response, "token");
        logged_in = true;

        // start continuous validation
        start_continuous_validation();

        return true;
    }

    return false;
}

bool night_auth::is_token_valid() const {
    if (token.empty()) return false;

    // simple token validation - check if it's not empty and has reasonable length
    return token.length() > 10;
}

std::string night_auth::get_user_info() const {
    if (!is_logged_in()) return "";

    // extract user info from token (basic implementation)
    std::string user_info = "username: " + username;
    if (is_token_valid()) {
        user_info += " | token: " + token.substr(0, 20) + "...";
    }
    return user_info;
}

std::string night_auth::get_password() const {
    return password;
}

std::string night_auth::get_hwid() const {
    return hwid;
}

void night_auth::set_hwid(const std::string& new_hwid) {
    hwid = new_hwid;
}

bool night_auth::validate_hwid() const {
    return !hwid.empty();
}

bool night_auth::perform_security_checks() const {
   //  Anti-debugging check
    if (anti_debug::is_being_debugged()) {
        return false;
    }

    // Integrity check
    if (!integrity_check::verify_module_integrity()) {
        return false;
    }

    if (!integrity_check::check_pe_headers()) {
        return false;
    }

    return true;
}

void night_auth::start_continuous_validation() {
    if (logged_in && validator) {
        validator->start(token, username);
    }
}

void night_auth::stop_continuous_validation() {
    if (validator) {
        validator->stop();
    }
}

bool night_auth::is_continuously_valid() const {
    if (!validator) return false;
    return validator->is_token_valid();
}
