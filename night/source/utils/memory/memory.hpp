#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

class c_memory
{
public:
    c_memory() = default;

    static std::unique_ptr<c_memory>& instance()
    {
        static std::unique_ptr<c_memory> singleton{ std::make_unique<c_memory>() };
        return singleton;
    }

    DWORD get_process_id(const std::string& process_name)
    {
        const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE)
            return 0;

        PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
        while (Process32Next(snapshot, &pe))
        {
            if (_stricmp(pe.szExeFile, process_name.c_str()) == 0)
            {
                CloseHandle(snapshot);
                m_process_id = pe.th32ProcessID;
                return m_process_id;
            }
        }

        CloseHandle(snapshot);
        return 0;
    }

    HANDLE get_process_handle(const std::string& process_name)
    {
        const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };

        while (Process32Next(snapshot, &pe))
        {
            if (_stricmp(pe.szExeFile, process_name.c_str()) == 0)
            {
                m_process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
                CloseHandle(snapshot);
                return m_process_handle;
            }
        }

        CloseHandle(snapshot);
        return nullptr;
    }

    uintptr_t get_process_base(const std::string& process_name)
    {
        const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_process_id);
        MODULEENTRY32 me = { sizeof(MODULEENTRY32) };

        while (Module32Next(snapshot, &me))
        {
            if (_stricmp(me.szModule, process_name.c_str()) == 0)
            {
                CloseHandle(snapshot);
                m_process_base = reinterpret_cast<uintptr_t>(me.modBaseAddr);
                return m_process_base;
            }
        }

        CloseHandle(snapshot);
        return 0;
    }

    uintptr_t get_module_base(const std::string& module_name)
    {
        const HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_process_id);
        MODULEENTRY32 me = { sizeof(MODULEENTRY32) };

        while (Module32Next(snapshot, &me))
        {
            if (_stricmp(me.szModule, module_name.c_str()) == 0)
            {
                CloseHandle(snapshot);
                return reinterpret_cast<uintptr_t>(me.modBaseAddr);
            }
        }

        CloseHandle(snapshot);
        return 0;
    }

    uintptr_t find_region(SIZE_T region_size)
    {
        SYSTEM_INFO si;
        GetSystemInfo(&si);

        uintptr_t current = reinterpret_cast<uintptr_t>(si.lpMinimumApplicationAddress);
        uintptr_t end = reinterpret_cast<uintptr_t>(si.lpMaximumApplicationAddress);
        MEMORY_BASIC_INFORMATION mbi;

        while (current < end)
        {
            if (VirtualQueryEx(m_process_handle, reinterpret_cast<LPCVOID>(current), &mbi, sizeof(mbi)) == sizeof(mbi))
            {
                if (mbi.Type == MEM_PRIVATE && mbi.State == MEM_COMMIT &&
                    mbi.Protect == PAGE_READWRITE && mbi.RegionSize == region_size)
                {
                    return reinterpret_cast<uintptr_t>(mbi.BaseAddress);
                }
                current += mbi.RegionSize;
            }
            else break;
        }

        return 0;
    }

    std::string read_string(uintptr_t address)
    {
        const size_t length = read<size_t>(address + 0x10);
        const uintptr_t str_ptr = (length >= 16) ? read<uintptr_t>(address) : address;

        if (length == 0 || length > 150)
            return "NULL";

        std::vector<char> buffer(length + 1, 0);
        ReadProcessMemory(m_process_handle, reinterpret_cast<LPCVOID>(str_ptr), buffer.data(), length, nullptr);
        return std::string(buffer.data(), length);
    }

    template <typename T>
    T read(uintptr_t address)
    {
        EnsureSyscallInit();
        T value{};
        SIZE_T bytesRead{};
        if (m_process_handle && m_ntReadVirtualMemory)
            m_ntReadVirtualMemory(m_process_handle, reinterpret_cast<PVOID>(address), &value, sizeof(T), &bytesRead);
        return value;
    }

    template <typename T>
    void write(uintptr_t address, const T& value)
    {
        EnsureSyscallInit();
        if (!m_process_handle || !m_ntWriteVirtualMemory) return;

        SIZE_T bytesWritten{};
        m_ntWriteVirtualMemory(m_process_handle, reinterpret_cast<PVOID>(address), const_cast<T*>(&value), sizeof(T), &bytesWritten);
    }

    template <typename T>
    void write_array(uintptr_t address, const T* buffer, size_t count)
    {
        EnsureSyscallInit();
        if (!m_process_handle || !m_ntWriteVirtualMemory) return;

        SIZE_T bytesWritten{};
        m_ntWriteVirtualMemory(m_process_handle, reinterpret_cast<PVOID>(address), const_cast<T*>(buffer), count * sizeof(T), &bytesWritten);
    }

    std::string read_cstring(uintptr_t address, size_t max_length = 256)
    {
        const size_t length = read<size_t>(address + 0x10);
        const uintptr_t str_ptr = (length >= 16) ? read<uintptr_t>(address) : address;

        if (length == 0 || length > 150)
            return "NULL";

        std::vector<char> buffer(length + 1, 0);
        ReadProcessMemory(m_process_handle, reinterpret_cast<LPCVOID>(str_ptr), buffer.data(), length, nullptr);
        return std::string(buffer.data(), length);
    }
    bool writestring(HANDLE process_handle, uintptr_t baseAddress, uintptr_t stringOffset, const std::string& value) {
        if (!process_handle || baseAddress == 0)
            return false;

        uintptr_t stringAddress = baseAddress + stringOffset;

        if (value.size() >= 16) {
            uintptr_t realPtr = 0;
            if (!ReadProcessMemory(process_handle, reinterpret_cast<LPCVOID>(stringAddress), &realPtr, sizeof(realPtr), nullptr))
                return false;
            stringAddress = realPtr;
        }

        for (size_t i = 0; i < value.size() && i < 200; ++i) {
            if (!WriteProcessMemory(process_handle, reinterpret_cast<LPVOID>(stringAddress + i), &value[i], sizeof(char), nullptr))
                return false;
        }

        char zero = '\0';
        WriteProcessMemory(process_handle, reinterpret_cast<LPVOID>(stringAddress + value.size()), &zero, sizeof(char), nullptr);

        int64_t length = static_cast<int64_t>(value.size());
        WriteProcessMemory(process_handle, reinterpret_cast<LPVOID>(baseAddress + stringOffset + 0x10), &length, sizeof(length), nullptr);

        return true;
    }

    // New syscall-based methods
    template <typename T>
    T syscall_read(uint64_t address) noexcept
    {
        EnsureSyscallInit();
        T buffer{};
        SIZE_T bytesRead{};
        if (m_process_handle && m_syscall_NtReadVirtualMemory)
            m_syscall_NtReadVirtualMemory(m_process_handle, reinterpret_cast<PVOID>(address), &buffer, sizeof(T), reinterpret_cast<PULONG>(&bytesRead));
        return buffer;
    }

    template <typename T>
    bool syscall_write(uint64_t address, const T& buffer) noexcept
    {
        EnsureSyscallInit();
        if (!m_process_handle || !m_syscall_NtWriteVirtualMemory) return false;

        SIZE_T bytesWritten{};
        NTSTATUS status = m_syscall_NtWriteVirtualMemory(m_process_handle, reinterpret_cast<PVOID>(address),
            const_cast<T*>(&buffer), sizeof(T), reinterpret_cast<PULONG>(&bytesWritten));
        return (status >= 0 && bytesWritten == sizeof(T));
    }

    template <typename T>
    T zw_read(uint64_t address) noexcept
    {
        EnsureSyscallInit();
        T buffer{};
        SIZE_T bytesRead{};
        if (m_process_handle && m_zwReadVirtualMemory)
            m_zwReadVirtualMemory(m_process_handle, reinterpret_cast<PVOID>(address), &buffer, sizeof(T), &bytesRead);
        return buffer;
    }

    template <typename T>
    bool zw_write(uint64_t address, const T& buffer) noexcept
    {
        EnsureSyscallInit();
        if (!m_process_handle || !m_zwWriteVirtualMemory) return false;

        SIZE_T bytesWritten{};
        NTSTATUS status = m_zwWriteVirtualMemory(m_process_handle, reinterpret_cast<PVOID>(address),
            const_cast<T*>(&buffer), sizeof(T), &bytesWritten);
        return (status >= 0 && bytesWritten == sizeof(T));
    }

    bool write_bytes(uint64_t address, const void* buffer, size_t size) noexcept
    {
        EnsureSyscallInit();
        if (!m_process_handle || !m_ntWriteVirtualMemory) return false;

        SIZE_T bytesWritten{};
        NTSTATUS status = m_ntWriteVirtualMemory(m_process_handle, reinterpret_cast<PVOID>(address), const_cast<void*>(buffer), size, &bytesWritten);
        return (status >= 0 && bytesWritten == size);
    }

    HANDLE get_handle() const { return m_process_handle; }

private:
    DWORD m_process_id{ 0 };
    HANDLE m_process_handle{ nullptr };
    uintptr_t m_process_base{ 0 };

    // Syscall functionality from new driver
    void* m_syscallReadStubPtr = nullptr;
    void* m_syscallWriteStubPtr = nullptr;
    bool m_syscallInitialized = false;

    using SyscallReadFn = NTSTATUS(__fastcall*)(HANDLE, PVOID, PVOID, ULONG, PULONG);
    using SyscallWriteFn = NTSTATUS(__fastcall*)(HANDLE, PVOID, PVOID, ULONG, PULONG);

    SyscallReadFn m_syscall_NtReadVirtualMemory = nullptr;
    SyscallWriteFn m_syscall_NtWriteVirtualMemory = nullptr;

    NTSTATUS(NTAPI* m_ntReadVirtualMemory)(HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T) = nullptr;
    NTSTATUS(NTAPI* m_ntWriteVirtualMemory)(HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T) = nullptr;

    NTSTATUS(NTAPI* m_zwReadVirtualMemory)(HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T) = nullptr;
    NTSTATUS(NTAPI* m_zwWriteVirtualMemory)(HANDLE, PVOID, PVOID, SIZE_T, PSIZE_T) = nullptr;

    __forceinline bool IsValid(uintptr_t addr) noexcept
    {
        return addr >= 0x10000 && addr < 0x7FFFFFFFFFFF;
    }

    __forceinline void* CreateSyscallStub(uint32_t syscallId) noexcept
    {
        constexpr uint8_t stubTemplate[] = {
            0x4C, 0x8B, 0xD1,
            0xB8, 0x00, 0x00, 0x00, 0x00,
            0x0F, 0x05,
            0xC3
        };

        uint8_t stub[sizeof(stubTemplate)];
        memcpy(stub, stubTemplate, sizeof(stubTemplate));
        *reinterpret_cast<uint32_t*>(stub + 4) = syscallId;

        void* page = VirtualAlloc(nullptr, sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (!page) return nullptr;

        memcpy(page, stub, sizeof(stub));
        DWORD oldProtect;
        VirtualProtect(page, sizeof(stub), PAGE_EXECUTE_READ, &oldProtect);

        return page;
    }

    __forceinline uint32_t GetSyscallID(const char* funcName) noexcept
    {
        static HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if (!ntdll) return 0;

        const auto* funcAddr = reinterpret_cast<const uint8_t*>(GetProcAddress(ntdll, funcName));
        if (!funcAddr) return 0;

        for (int i = 0; i < 20; ++i)
            if (funcAddr[i] == 0xB8)
                return *reinterpret_cast<const uint32_t*>(funcAddr + i + 1);

        return 0;
    }

    inline void EnsureSyscallInit() noexcept
    {
        if (m_syscallInitialized || m_process_id == 0) return;

        if (!m_process_handle)
            m_process_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_process_id);

        if (!m_process_handle) return;

        static HMODULE ntdll = GetModuleHandleA("ntdll.dll");
        if (!ntdll) return;

        m_ntReadVirtualMemory = reinterpret_cast<decltype(m_ntReadVirtualMemory)>(GetProcAddress(ntdll, "NtReadVirtualMemory"));
        m_ntWriteVirtualMemory = reinterpret_cast<decltype(m_ntWriteVirtualMemory)>(GetProcAddress(ntdll, "NtWriteVirtualMemory"));
        m_zwReadVirtualMemory = reinterpret_cast<decltype(m_zwReadVirtualMemory)>(GetProcAddress(ntdll, "ZwReadVirtualMemory"));
        m_zwWriteVirtualMemory = reinterpret_cast<decltype(m_zwWriteVirtualMemory)>(GetProcAddress(ntdll, "ZwWriteVirtualMemory"));

        if (!m_ntReadVirtualMemory || !m_ntWriteVirtualMemory || !m_zwReadVirtualMemory || !m_zwWriteVirtualMemory)
            return;

        uint32_t readSyscallId = GetSyscallID("NtReadVirtualMemory");
        uint32_t writeSyscallId = GetSyscallID("NtWriteVirtualMemory");
        if (readSyscallId == 0 || writeSyscallId == 0) return;

        m_syscallReadStubPtr = CreateSyscallStub(readSyscallId);
        m_syscallWriteStubPtr = CreateSyscallStub(writeSyscallId);
        if (!m_syscallReadStubPtr || !m_syscallWriteStubPtr) return;

        m_syscall_NtReadVirtualMemory = reinterpret_cast<SyscallReadFn>(m_syscallReadStubPtr);
        m_syscall_NtWriteVirtualMemory = reinterpret_cast<SyscallWriteFn>(m_syscallWriteStubPtr);

        m_syscallInitialized = true;
    }
};

inline std::unique_ptr<c_memory>& g_memory = c_memory::instance();