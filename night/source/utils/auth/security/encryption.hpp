#pragma once

#include <string>
#include <array>
#include <iostream>
#include <vector>
#include <cstdint>

// Compile-time string encryption
template<size_t N>
class compile_time_string {
private:
    std::array<char, N> encrypted_data;

    static constexpr char encrypt_char(char c, size_t index) {
        return c ^ (0xAB + index % 256);
    }

public:
    template<size_t... I>
    constexpr compile_time_string(const char* str, std::index_sequence<I...>)
        : encrypted_data{ encrypt_char(str[I], I)... } {
    }

    std::string decrypt() const {
        std::string result;
        result.reserve(N);
        for (size_t i = 0; i < N; i++) {
            result += encrypted_data[i] ^ (0xAB + i % 256);
        }
        return result;
    }

    operator std::string() const {
        return decrypt();
    }
};

// Macro for compile-time encrypted strings
#define CT_ENCRYPT_STR(str) []() { \
    constexpr auto encrypted = compile_time_string<sizeof(str)-1>(str, std::make_index_sequence<sizeof(str)-1>{}); \
    return encrypted; \
}()

// Runtime string encryption with better obfuscation
class runtime_string {
private:
    std::vector<uint8_t> encrypted_data;
    static constexpr uint8_t KEY1 = 0xAB;
    static constexpr uint8_t KEY2 = 0xCD;
    static constexpr uint8_t KEY3 = 0xEF;

public:
    runtime_string(const char* str) {
        if (str) {
            size_t len = strlen(str);
            encrypted_data.resize(len);

            uint8_t key = KEY1;
            for (size_t i = 0; i < len; i++) {
                encrypted_data[i] = str[i] ^ key;
                key = (key + KEY2) ^ KEY3;
            }
        }
    }

    std::string decrypt() const {
        std::string result;
        result.reserve(encrypted_data.size());

        uint8_t key = KEY1;
        for (size_t i = 0; i < encrypted_data.size(); i++) {
            result += encrypted_data[i] ^ key;
            key = (key + KEY2) ^ KEY3;
        }
        return result;
    }

    operator std::string() const {
        return decrypt();
    }
};

// Macro for runtime encrypted strings
#define RT_ENCRYPT_STR(str) runtime_string(str)

// Advanced string obfuscation
class obfuscated_string {
private:
    std::vector<uint8_t> data;
    std::vector<uint8_t> keys;

public:
    obfuscated_string(const char* str) {
        if (str) {
            size_t len = strlen(str);
            data.resize(len);
            keys.resize(len);

            // Generate random-looking keys based on string content
            uint32_t seed = 0x12345678;
            for (size_t i = 0; i < len; i++) {
                seed = seed * 0x41C64E6D + 12345;
                keys[i] = (seed >> 16) & 0xFF;
                data[i] = str[i] ^ keys[i];
            }
        }
    }

    std::string decrypt() const {
        std::string result;
        result.reserve(data.size());

        uint32_t seed = 0x12345678;
        for (size_t i = 0; i < data.size(); i++) {
            seed = seed * 0x41C64E6D + 12345;
            uint8_t key = (seed >> 16) & 0xFF;
            result += data[i] ^ key;
        }
        return result;
    }

    operator std::string() const {
        return decrypt();
    }

    // Conversion to const char* for Windows API calls
    const char* c_str() const {
        static thread_local std::string decrypted;
        decrypted = decrypt();
        return decrypted.c_str();
    }

    // Stream operator for std::cout
    friend std::ostream& operator<<(std::ostream& os, const obfuscated_string& s) {
        return os << s.decrypt();
    }
};

// Macro for obfuscated strings
#define OBFUSCATE_STR(str) obfuscated_string(str)
