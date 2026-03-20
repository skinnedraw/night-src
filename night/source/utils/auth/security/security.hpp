#pragma once

#include <string>
#include <vector>
#include <windows.h>
#include <iostream>

class secure_string {
private:
    std::vector<char> encrypted_data;
    static constexpr char XOR_KEY = 0xAB;

public:
    // Constructor for encrypted strings
    secure_string(const char* str) {
        if (str) {
            size_t len = strlen(str);
            encrypted_data.resize(len);
            for (size_t i = 0; i < len; i++) {
                encrypted_data[i] = str[i] ^ XOR_KEY ^ (i % 256);
            }
        }
    }

    // Decrypt and return string
    std::string decrypt() const {
        std::string result;
        result.reserve(encrypted_data.size());
        for (size_t i = 0; i < encrypted_data.size(); i++) {
            result += encrypted_data[i] ^ XOR_KEY ^ (i % 256);
        }
        return result;
    }

    // Implicit conversion to std::string
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
    friend std::ostream& operator<<(std::ostream& os, const secure_string& s) {
        return os << s.decrypt();
    }
};

// Macro for easy encrypted string creation - returns secure_string object
#define ENCRYPT_STR(str) secure_string(str)

// Anti-debugging functions
class anti_debug {
public:
    static bool is_debugger_present() {
        return IsDebuggerPresent() != 0;
    }

    static bool check_remote_debugger() {
        BOOL is_remote_debugger = FALSE;
        CheckRemoteDebuggerPresent(GetCurrentProcess(), &is_remote_debugger);
        return is_remote_debugger != 0;
    }

    static bool check_debugger_by_timing() {
        LARGE_INTEGER start, end, freq;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&start);

        // Simple operation
        volatile int dummy = 0;
        for (int i = 0; i < 1000; i++) {
            dummy += i;
        }

        QueryPerformanceCounter(&end);

        // If execution took too long, likely being debugged
        double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
        return elapsed > 0.001; // 1ms threshold
    }

    static bool is_being_debugged() {
        return is_debugger_present() ||
            check_remote_debugger() ||
            check_debugger_by_timing();
    }

    static void crash_if_debugged() {
        if (is_being_debugged()) {
            // Crash the program
            int* null_ptr = nullptr;
            *null_ptr = 0xDEADBEEF;
        }
    }
};

// Integrity checking
class integrity_check {
public:
    static bool verify_module_integrity() {
        HMODULE hModule = GetModuleHandle(nullptr);
        if (!hModule) return false;

        IMAGE_DOS_HEADER* dos_header = (IMAGE_DOS_HEADER*)hModule;
        if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) return false;

        IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)((BYTE*)hModule + dos_header->e_lfanew);
        if (nt_headers->Signature != IMAGE_NT_SIGNATURE) return false;

        return true;
    }

    static bool check_pe_headers() {
        HMODULE hModule = GetModuleHandle(nullptr);
        if (!hModule) return false;

        IMAGE_DOS_HEADER* dos_header = (IMAGE_DOS_HEADER*)hModule;
        IMAGE_NT_HEADERS* nt_headers = (IMAGE_NT_HEADERS*)((BYTE*)hModule + dos_header->e_lfanew);

        // Check if headers look tampered with
        if (nt_headers->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
            nt_headers->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
            return false;
        }

        return true;
    }
};

// Obfuscation helpers
class obfuscation {
public:
    static void sleep_random() {
        Sleep(rand() % 100 + 50); // Random sleep 50-150ms
    }

    static void dummy_operations() {
        volatile int dummy = 0;
        for (int i = 0; i < 100; i++) {
            dummy += rand() % 10;
        }
    }

    static void anti_analysis() {
        // Random operations to confuse analysis
        if (rand() % 2) {
            dummy_operations();
        }
        else {
            sleep_random();
        }
    }
};
