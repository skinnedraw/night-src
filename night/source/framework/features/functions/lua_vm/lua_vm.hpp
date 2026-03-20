#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

// Forward declarations to avoid circular dependencies
namespace sdk {
    class c_instance;
    class c_player;
    class c_players;
    class c_workspace;
    class c_camera;
    class c_mouse_service;
    class c_humanoid;
    class c_gui_object;
    class c_renderview;
    class c_visual_engine;
    class c_datamodel;
    struct vector3_t;
    struct cframe_t;
}

// Include necessary SDK files
#include <source/framework/sdk/classes/c_instance.hpp>
#include <source/framework/sdk/classes/c_player.hpp>
#include <source/framework/sdk/classes/c_players.hpp>
#include <source/framework/sdk/classes/c_workspace.hpp>
#include <source/framework/sdk/classes/c_camera.hpp>
#include <source/framework/sdk/classes/c_mouse_service.hpp>
#include <source/framework/sdk/classes/c_humanoid.hpp>
#include <source/framework/sdk/classes/c_gui_object.hpp>
#include <source/framework/sdk/classes/c_renderview.hpp>
#include <source/framework/sdk/classes/c_visual_engine.hpp>
#include <source/framework/sdk/classes/c_datamodel.hpp>
#include <source/framework/sdk/math/vector3_t.hpp>
#include <source/framework/sdk/math/cframe_t.hpp>
#include <source/framework/globals/globals.hpp>

namespace features {
    class LuaVM {
    public:
        LuaVM();
        ~LuaVM();

        // Core VM functions
        bool Initialize();
        void Shutdown();
        bool ExecuteScript(const std::string& script);
        bool ExecuteFile(const std::string& filename);

        // Script management
        void ClearScripts();
        std::string GetLastError() const;
        bool IsInitialized() const;

        // Lua function registration
        void RegisterGameFunctions();
        void RegisterUtilityFunctions();
        void RegisterMathFunctions();
        void RegisterInputFunctions();

    private:
        lua_State* L;
        bool initialized;
        std::string lastError;

        // Game object references
        std::shared_ptr<sdk::c_workspace> workspace;
        std::shared_ptr<sdk::c_players> players;
        std::shared_ptr<sdk::c_camera> camera;
        std::shared_ptr<sdk::c_mouse_service> mouse_service;
        std::shared_ptr<sdk::c_renderview> renderview;
        std::shared_ptr<sdk::c_visual_engine> visual_engine;
        std::shared_ptr<sdk::c_datamodel> datamodel;

        // Static Lua function wrappers
        static int lua_print(lua_State* L);
        static int lua_warn(lua_State* L);
        static int lua_error(lua_State* L);

        // Game object functions
        static int lua_get_workspace(lua_State* L);
        static int lua_get_players(lua_State* L);
        static int lua_get_camera(lua_State* L);
        static int lua_get_mouse(lua_State* L);
        static int lua_get_renderview(lua_State* L);
        static int lua_get_visual_engine(lua_State* L);
        static int lua_get_datamodel(lua_State* L);

        // Player functions
        static int lua_get_local_player(lua_State* L);
        static int lua_get_players_list(lua_State* L);
        static int lua_get_player_by_name(lua_State* L);
        static int lua_get_player_character(lua_State* L);
        static int lua_get_player_humanoid(lua_State* L);

        // Vector3 functions
        static int lua_vector3_new(lua_State* L);
        static int lua_vector3_add(lua_State* L);
        static int lua_vector3_sub(lua_State* L);
        static int lua_vector3_mul(lua_State* L);
        static int lua_vector3_div(lua_State* L);
        static int lua_vector3_magnitude(lua_State* L);
        static int lua_vector3_normalize(lua_State* L);
        static int lua_vector3_dot(lua_State* L);
        static int lua_vector3_cross(lua_State* L);
        static int lua_vector3_distance(lua_State* L);

        // CFrame functions
        static int lua_cframe_new(lua_State* L);
        static int lua_cframe_look_at(lua_State* L);
        static int lua_cframe_angles(lua_State* L);
        static int lua_cframe_position(lua_State* L);
        static int lua_cframe_rotation(lua_State* L);
        static int lua_cframe_to_world_space(lua_State* L);
        static int lua_cframe_to_object_space(lua_State* L);

        // Camera functions
        static int lua_camera_get_position(lua_State* L);
        static int lua_camera_get_cframe(lua_State* L);
        static int lua_camera_get_field_of_view(lua_State* L);
        static int lua_camera_set_field_of_view(lua_State* L);
        static int lua_camera_get_near_clip(lua_State* L);
        static int lua_camera_set_near_clip(lua_State* L);
        static int lua_camera_get_far_clip(lua_State* L);
        static int lua_camera_set_far_clip(lua_State* L);

        // Mouse functions
        static int lua_mouse_get_position(lua_State* L);
        static int lua_mouse_get_world_ray(lua_State* L);
        static int lua_mouse_get_unit_ray(lua_State* L);
        static int lua_mouse_get_x(lua_State* L);
        static int lua_mouse_get_y(lua_State* L);

        // Utility functions
        static int lua_wait(lua_State* L);
        static int lua_tick(lua_State* L);
        static int lua_time(lua_State* L);
        static int lua_random(lua_State* L);
        static int lua_randomseed(lua_State* L);

        // Math functions
        static int lua_math_abs(lua_State* L);
        static int lua_math_acos(lua_State* L);
        static int lua_math_asin(lua_State* L);
        static int lua_math_atan(lua_State* L);
        static int lua_math_atan2(lua_State* L);
        static int lua_math_ceil(lua_State* L);
        static int lua_math_cos(lua_State* L);
        static int lua_math_deg(lua_State* L);
        static int lua_math_exp(lua_State* L);
        static int lua_math_floor(lua_State* L);
        static int lua_math_fmod(lua_State* L);
        static int lua_math_log(lua_State* L);
        static int lua_math_max(lua_State* L);
        static int lua_math_min(lua_State* L);
        static int lua_math_pow(lua_State* L);
        static int lua_math_rad(lua_State* L);
        static int lua_math_sin(lua_State* L);
        static int lua_math_sqrt(lua_State* L);
        static int lua_math_tan(lua_State* L);

        // Helper functions
        void SetError(const std::string& error);
        void PushVector3(const sdk::vector3_t& vec);
        sdk::vector3_t GetVector3(int index);
        void PushCFrame(const sdk::cframe_t& cframe);
        sdk::cframe_t GetCFrame(int index);

        // Game object getters
        std::shared_ptr<sdk::c_workspace> GetWorkspace();
        std::shared_ptr<sdk::c_players> GetPlayers();
        std::shared_ptr<sdk::c_camera> GetCamera();
        std::shared_ptr<sdk::c_mouse_service> GetMouse();
        std::shared_ptr<sdk::c_renderview> GetRenderView();
        std::shared_ptr<sdk::c_visual_engine> GetVisualEngine();
        std::shared_ptr<sdk::c_datamodel> GetDataModel();
    };

    // Global Lua VM instance
    extern std::unique_ptr<LuaVM> lua_vm;
}