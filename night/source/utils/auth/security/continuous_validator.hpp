#pragma once

#include <thread>
#include <atomic>
#include <chrono>
#include <string>
#include <windows.h>
#include "security.hpp"

class continuous_validator {
private:
    std::atomic<bool> running;
    std::atomic<bool> valid_token;
    std::thread validation_thread;
    std::string api_url;
    std::string current_token;
    std::string username;
    int validation_interval_ms;

    void validation_loop() {
        while (running.load()) {
            try {
                // Anti-debugging check
                anti_debug::crash_if_debugged();

                // Integrity check
                if (!integrity_check::verify_module_integrity()) {
                    valid_token.store(false);
                    break;
                }

                // Validate token with server
                if (!validate_token_with_server()) {
                    valid_token.store(false);
                    break;
                }

                // Random anti-analysis
                obfuscation::anti_analysis();

                // Sleep for validation interval
                std::this_thread::sleep_for(std::chrono::milliseconds(validation_interval_ms));

            }
            catch (...) {
                valid_token.store(false);
                break;
            }
        }
    }

    bool validate_token_with_server() {
        if (current_token.empty()) return false;

        // Create validation request
        std::string escaped_token = escape_for_powershell(current_token);
        std::string escaped_username = escape_for_powershell(username);

        std::string ps_command =
            "$body = @{token='" + escaped_token + "';username='" + escaped_username + "'} | convertto-json; " +
            "$response = invoke-restmethod -uri '" + api_url + "/validate_token.php' -method post -body $body -contenttype 'application/json'; " +
            "$response | convertto-json";

        std::string response = execute_powershell(ps_command);

        if (response.empty()) return false;

        // Parse response
        std::string success = extract_value(response, "success");
        return success == "true";
    }

    std::string execute_powershell(const std::string& command) {
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

    std::string extract_value(const std::string& json, const std::string& key) {
        size_t start = json.find("\"" + key + "\":");
        if (start == std::string::npos) return "";

        start = json.find(":", start);
        if (start == std::string::npos) return "";

        start++; // Skip ':'
        while (start < json.length() && (json[start] == ' ' || json[start] == '"')) start++;

        size_t end = start;
        if (json[start - 1] == '"') {
            // String value
            while (end < json.length() && json[end] != '"') end++;
        }
        else {
            // Number or boolean
            while (end < json.length() && json[end] != ',' && json[end] != '}') end++;
        }

        return json.substr(start, end - start);
    }

    std::string escape_for_powershell(const std::string& input) {
        std::string result = input;
        size_t pos = 0;
        while ((pos = result.find("'", pos)) != std::string::npos) {
            result.replace(pos, 1, "''");
            pos += 2;
        }
        return result;
    }

public:
    continuous_validator(const std::string& api_url, int interval_ms = 30000)
        : running(false), valid_token(false), api_url(api_url), validation_interval_ms(interval_ms) {
    }

    ~continuous_validator() {
        stop();
    }

    void start(const std::string& token, const std::string& user) {
        if (running.load()) return;

        current_token = token;
        username = user;
        valid_token.store(true);
        running.store(true);

        validation_thread = std::thread(&continuous_validator::validation_loop, this);
    }

    void stop() {
        if (!running.load()) return;

        running.store(false);
        if (validation_thread.joinable()) {
            validation_thread.join();
        }
    }

    bool is_token_valid() const {
        return valid_token.load();
    }

    void update_token(const std::string& new_token) {
        current_token = new_token;
    }
};
