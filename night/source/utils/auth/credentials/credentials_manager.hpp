#pragma once

#include <string>
#include <windows.h>
#include <shlobj.h>
#include <fstream>
#include <source/utils/json/json.hpp>

class credentials_manager {
private:
    std::string config_path;

    std::string get_appdata_path() {
        char path[MAX_PATH];
        if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, path) == S_OK) {
            return std::string(path) + "\\night_auth\\credentials.json";
        }
        return "";
    }

public:
    credentials_manager() {
        config_path = get_appdata_path();

        // Create directory if it doesn't exist
        if (!config_path.empty()) {
            std::string dir = config_path.substr(0, config_path.find_last_of("\\"));
            CreateDirectoryA(dir.c_str(), NULL);
        }
    }

    bool save_credentials(const std::string& username, const std::string& password) {
        if (config_path.empty()) return false;

        try {
            nlohmann::json config;
            config["username"] = username;
            config["password"] = password;
            config["saved_at"] = time(nullptr);

            std::ofstream file(config_path);
            if (file.is_open()) {
                file << config.dump(4);
                file.close();
                return true;
            }
        }
        catch (...) {
            return false;
        }

        return false;
    }

    bool load_credentials(std::string& username, std::string& password) {
        if (config_path.empty()) return false;

        try {
            std::ifstream file(config_path);
            if (!file.is_open()) return false;

            nlohmann::json config;
            file >> config;
            file.close();

            if (config.contains("username") && config.contains("password")) {
                username = config["username"];
                password = config["password"];
                return true;
            }
        }
        catch (...) {
            return false;
        }

        return false;
    }

    bool has_saved_credentials() {
        if (config_path.empty()) return false;

        std::ifstream file(config_path);
        return file.good();
    }

    bool clear_credentials() {
        if (config_path.empty()) return false;

        return DeleteFileA(config_path.c_str()) != 0;
    }

    std::string get_config_path() const {
        return config_path;
    }
};
