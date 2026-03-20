#include "entry.hpp"
#include <source/framework/features/cache/wallcheck/wallcheck.hpp>
#include <source/framework/features/functions/triggerbot/triggerbot.hpp>
#include <source/utils/console/console.hpp>

namespace features
{
    void refresh_globals()
    {
        try {
            if (!g_memory || !globals) {
                return;
            }

            uint64_t dm_ptr = g_memory->read<uint64_t>(globals->base + sdk::offsets::datamodel::pointer);
            
            if (!dm_ptr) {
                return;
            }

            uint64_t real_dm = g_memory->read<uint64_t>(dm_ptr + sdk::offsets::datamodel::realdatamodel);
            uint64_t visual_ptr = g_memory->read<uint64_t>(globals->base + sdk::offsets::visualengine::pointer);
            
            if (!real_dm || !visual_ptr) {
                return;
            }

            globals->datamodel = std::make_shared<sdk::c_datamodel>(real_dm);
            globals->visual_engine = std::make_shared<sdk::c_visual_engine>(visual_ptr);
            
            auto workspace = globals->datamodel->find_first_child_of_class("Workspace");
            auto players = globals->datamodel->find_first_child_of_class("Players");
            auto mouse_service = globals->datamodel->find_first_child_of_class("MouseService");
            auto lighting = globals->datamodel->find_first_child_of_class("Lighting");
            
            if (workspace && players && mouse_service) {
                globals->workspace = std::make_shared<sdk::c_workspace>(workspace->address);
                globals->players = std::make_shared<sdk::c_players>(players->address);
                globals->mouse_service = std::make_shared<sdk::c_mouse_service>(mouse_service->address);
                
                // Camera is accessed via offset, not as a child
                if (globals->workspace->address) {
                    uint64_t camera_addr = g_memory->read<uint64_t>(globals->workspace->address + sdk::offsets::workspace::camera);
                    
                    if (camera_addr) {
                        globals->camera = std::make_shared<sdk::c_camera>(camera_addr);
                    } else {
                    }
                } else {
                }
                
                // Lighting is optional, only assign if found
                if (lighting && lighting->address) {
                    globals->lighting = std::make_shared<sdk::c_instance>(lighting->address);
                } else {
                }
            } else {
            }
        }
        catch (...) {
            // swallow invalid reads safely
        }
    }

    void update_gui_aim()
    {
     //   if (!globals->players || !globals->visual_engine) return;

        POINT cursor_point;
        GetCursorPos(&cursor_point);
        ScreenToClient(FindWindowA(nullptr, "Roblox"), &cursor_point);

        float new_mouse_x = static_cast<float>(cursor_point.x);
        float screen_height = globals->visual_engine->get_view_port().m_y;
        float new_mouse_y = static_cast<float>(screen_height - std::abs(screen_height - cursor_point.y));

        static std::shared_ptr<sdk::c_gui_object> aim_obj1;
        static std::shared_ptr<sdk::c_gui_object> aim_obj2;

        auto local_player = globals->players->get_local_player();
      //  if (!local_player) return;

        auto aim_instance1 = local_player
            ->find_first_child("PlayerGui")
            ->find_first_child("MainScreenGui")
            ->find_first_child("Aim");

        auto aim_instance2 = local_player
            ->find_first_child("PlayerGui")
            ->find_first_child("Main Screen")
            ->find_first_child("Aim");

        if (!aim_obj1 && aim_instance1 && aim_instance1->address)
            aim_obj1 = std::make_shared<sdk::c_gui_object>(aim_instance1->address);

        if (!aim_obj2 && aim_instance2 && aim_instance2->address)
            aim_obj2 = std::make_shared<sdk::c_gui_object>(aim_instance2->address);

        if (globals->silent_aim_enabled) {
            if (aim_obj1)
            {
                aim_obj1->set_frame_x(new_mouse_x);
                aim_obj1->set_frame_y(new_mouse_y);
            }
            else if (aim_obj2)
            {
                aim_obj2->set_frame_x(new_mouse_x);
                aim_obj2->set_frame_y(new_mouse_y);
            }
        }
    }

    void c_entry::initialize() 
    {
        DWORD process_id = 0;
        while (!(process_id = g_memory->get_process_id("RobloxPlayerBeta.exe")))
            std::this_thread::sleep_for(std::chrono::seconds(1));

        globals->pid = process_id;
        globals->handle = g_memory->get_process_handle("RobloxPlayerBeta.exe");
        globals->base = g_memory->get_process_base("RobloxPlayerBeta.exe");

        while (true)
        {
            refresh_globals();
            if (globals->datamodel && globals->datamodel->get_class_name() != "LuaApp")
                break;

            console->print(c_console::log_level::info, "waiting for game to load...");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        console->print(c_console::log_level::info, "starting cache");

//        g_memory->write<std::int32_t>(globals->base + 0x5ffaec0, 0);

        //g_memory->write<uint32_t>(globals->base + 0x5FFB5A4, -99999); // WDWORorldStepMax
        //g_memory->write<uint32_t>(globals->base + 0x5FFB5A8, -99999); // WorldStepsOffsetAdjustRate

        uint64_t rbx_ptr = g_memory->read<uint64_t>(globals->datamodel->address + 0x38); // RBX::OnDemandInstance
        auto rbx_instance = std::make_shared<sdk::c_instance>(rbx_ptr);

        std::cout << std::hex << rbx_instance->address << std::endl;

        auto children = rbx_instance->get_children();
        for (const auto& child : children) {
            std::cout << " - " << child->address << "\n";
        }

        features::cache->start_background_updates();

        std::thread([] {
            while (true) {
                update_gui_aim();
            }
            }).detach();

        std::thread([] {
            while (true) {
                refresh_globals();
                features::exploits->initialize();

                if (globals->performance_mode)
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            }).detach();

        std::thread([] {

            features::silent_aim->initialize();

            if (globals->performance_mode)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));

            }).detach();

        std::thread([] {
            while (true)
            {
                features::c_triggerbot::update();
                if (globals->performance_mode)
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            }).detach();

        std::thread([] {
            while (true)
            {
                features::c_player_lock::update();
                if (globals->performance_mode)
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            }).detach();

        // Start wallcheck thread
        std::thread(features::wallcheck::update_thread).detach();
    }
}
