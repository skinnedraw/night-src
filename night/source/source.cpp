#include <iostream>
#include <Windows.h>
#include <string>
#include <conio.h>
#include <thread>
#include <atomic>

#include <source/utils/console/console.hpp>
#include <source/utils/memory/memory.hpp>
#include <source/framework/features/entry/entry.hpp>
#include <source/framework/globals/globals.hpp>
#include <source/framework/sdk/includes.hpp>
#include <source/framework/features/functions/lua_vm/lua_vm.hpp>
#include <source/utils/auth/auth.hpp>
#include <source/utils/auth/credentials/credentials_manager.hpp>
#include <source/utils/auth/security/encryption.hpp>
#include <source/framework/overlay/config/config.hpp>

night_auth* g_auth = nullptr;
std::atomic<bool> g_auth_valid(true);
std::atomic<bool> g_program_running(true);

void background_protection() {
    while (g_program_running) {
        if (g_auth && !g_auth->is_continuously_valid()) {
            console->print(c_console::log_level::error, ("token validation failed - shutting down"));
            g_auth_valid = false;
            break;
        }

        if (g_auth && !g_auth->perform_security_checks()) {
            console->print(c_console::log_level::error, ("security check failed - shutting down"));
            g_auth_valid = false;
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(30));
    }
}

bool authenticate_user() {
    g_auth = new night_auth(OBFUSCATE_STR("https://getnight.wtf/api"));


    if (!g_auth->perform_security_checks()) {
        console->print(c_console::log_level::error, ("error 0x1"));
        delete g_auth;
        g_auth = nullptr;
        return false;
    }

    credentials_manager cred_manager;
    std::string username, password;

    if (cred_manager.has_saved_credentials()) {
        console->print(c_console::log_level::info, ("attempting auto-login."));

        if (cred_manager.load_credentials(username, password)) {
            g_auth->set_credentials(username, password);

            if (g_auth->silent_login()) {
                console->print(c_console::log_level::info, ("auto-login successful!"));
                console->print(c_console::log_level::info, (("welcome, ") + g_auth->get_username()).c_str());

                if (cred_manager.save_credentials(g_auth->get_username(), g_auth->get_password())) {
                    console->print(c_console::log_level::info, ("credentials saved for auto-login"));
                }


                g_auth->start_continuous_validation();

               // console->print(c_console::log_level::info, "");
              //  console->print(c_console::log_level::info, ("press any key to continue..."));
               // _getch();

                return true;
            }
            else {
                console->print(c_console::log_level::warning, ("auto-login failed, clearing saved credentials"));
                cred_manager.clear_credentials();
            }
        }
    }

  //  console->print(c_console::log_level::info, ("starting manual login..."));
   // console->print(c_console::log_level::info, "");

    if (g_auth->start()) {
      //  console->print(c_console::log_level::info, "");
        console->print(c_console::log_level::info, ("login successful!"));
        console->print(c_console::log_level::info, (("welcome, ") + g_auth->get_username()).c_str());

        if (cred_manager.save_credentials(g_auth->get_username(), g_auth->get_password())) {
            console->print(c_console::log_level::info, ("credentials saved for auto-login"));
        }

        g_auth->start_continuous_validation();

    }
    else {
        console->print(c_console::log_level::info, "");
        console->print(c_console::log_level::error, ("login failed!"));
        console->print(c_console::log_level::error, ("please check your credentials and try again"));

        console->print(c_console::log_level::info, "");
        console->print(c_console::log_level::info, ("press any key to exit..."));
        _getch();

        delete g_auth;
        g_auth = nullptr;
        return false;
    }

    console->print(c_console::log_level::info, "");
    console->print(c_console::log_level::info, ("press any key to continue..."));
    _getch();

    return true;
}

int main()
{
    console->set_title("night");
    console->initialize();

    //if (!authenticate_user()) {
    //    return 1;
    //}

    //std::thread protection_thread(background_protection);
   // protection_thread.detach();



    console->print(c_console::log_level::welcome, "loading entry");

    features::entry->initialize();

    console->print(c_console::log_level::info, "initialized entry");

    console->print(c_console::log_level::debug, (std::string("pid: ") + std::to_string(globals->pid)).c_str());
    console->print(c_console::log_level::debug, (std::string("handle: ") + std::to_string(reinterpret_cast<uintptr_t>(globals->handle))).c_str());
    console->print(c_console::log_level::debug, (std::string("base: ") + std::to_string(globals->base)).c_str());

    if (globals->datamodel)
    {
        console->print(c_console::log_level::debug, "datamodel: 0x%llX", (unsigned long long)globals->datamodel->address);


        if (globals->visual_engine)
        {
            console->print(c_console::log_level::debug, "visual_engine: 0x%llX", (unsigned long long)globals->visual_engine->address);
        }

        if (globals->workspace)
        {
            console->print(c_console::log_level::debug, "workspace: 0x%llX", (unsigned long long)globals->workspace->address);
        }

        if (globals->lighting)
        {
            console->print(c_console::log_level::debug, "lighting: 0x%llX", (unsigned long long)globals->lighting->address);
        }

        if (globals->mouse_service)
        {
            console->print(c_console::log_level::debug, "mouse_service: 0x%llX", (unsigned long long)globals->mouse_service->address);
            auto input_obj = std::make_shared<sdk::c_mouse_service>(g_memory->read<uintptr_t>(globals->mouse_service->address + 0x118));

            console->print(c_console::log_level::debug, "mouse_service input: 0x%llX", (unsigned long long)input_obj->address);
        }
    }

    console->print(c_console::log_level::info, "loading overlay");
    console->print(c_console::log_level::info, "loading cache");
    console->print(c_console::log_level::info, "loading wallcheck");
    console->print(c_console::log_level::info, "loading lua-vm");
    //InitializeLuaVM();

    // Auto-load last config
    console->print(c_console::log_level::info, "loading config system");
    if (config::config_manager->auto_load_last_config())
    {
        console->print(c_console::log_level::info, "auto-loaded last config");
    }
    else
    {
        console->print(c_console::log_level::info, "no previous config found, using defaults");
    }

    overlay::menu->load();

    while (true)
    {
        if (!g_auth_valid) {
            console->print(c_console::log_level::error, ("failed"));
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    g_program_running = false;
    if (g_auth) {
        delete g_auth;
        g_auth = nullptr;
    }

    return 0;
}
