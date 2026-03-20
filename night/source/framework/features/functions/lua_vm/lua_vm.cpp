#include "lua_vm.hpp"
#include <source/framework/overlay/misc/widgets.h>
#include <source/utils/console/console.hpp>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <chrono>
#include <random>
#include <fstream>
#include <iostream>
#include <thread>
#include <cmath>

// Link Lua libraries - replace "lua54.lib" with your actual library name
#pragma comment(lib, "lua54.lib")
// If that doesn't work, try:
// #pragma comment(lib, "liblua.lib")
// #pragma comment(lib, "lua.lib")

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace features {
    std::unique_ptr<LuaVM> lua_vm = std::make_unique<LuaVM>();

    LuaVM::LuaVM() : L(nullptr), initialized(false) {
    }

    LuaVM::~LuaVM() {
        Shutdown();
    }

    bool LuaVM::Initialize() {
        try {
            if (initialized) {
                return true;
            }

            L = luaL_newstate();
            if (!L) {
                SetError("Failed to create Lua state");
                return false;
            }

            // Open standard libraries
            luaL_openlibs(L);

            // Register all our custom functions
            RegisterGameFunctions();
            RegisterUtilityFunctions();
            RegisterMathFunctions();
            RegisterInputFunctions();

            initialized = true;
            return true;
        }
        catch (...) {
            SetError("Exception during initialization");
            return false;
        }
    }

    void LuaVM::Shutdown() {
        if (L) {
            lua_close(L);
            L = nullptr;
        }
        initialized = false;
        lastError.clear();
    }

    bool LuaVM::ExecuteScript(const std::string& script) {
        try {
            if (!initialized) {
                SetError("Lua VM not initialized");
                return false;
            }

            int result = luaL_dostring(L, script.c_str());
            if (result != LUA_OK) {
                SetError(lua_tostring(L, -1));
                lua_pop(L, 1);
                return false;
            }

            return true;
        }
        catch (...) {
            SetError("Exception during script execution");
            return false;
        }
    }

    bool LuaVM::ExecuteFile(const std::string& filename) {
        if (!initialized) {
            SetError("Lua VM not initialized");
            return false;
        }

        int result = luaL_dofile(L, filename.c_str());
        if (result != LUA_OK) {
            SetError(lua_tostring(L, -1));
            lua_pop(L, 1);
            return false;
        }

        return true;
    }

    void LuaVM::ClearScripts() {
        if (L) {
            lua_settop(L, 0);
        }
    }

    std::string LuaVM::GetLastError() const {
        return lastError;
    }

    bool LuaVM::IsInitialized() const {
        return initialized;
    }

    void LuaVM::SetError(const std::string& error) {
        lastError = error;
        Widgets::LogInfo(("Lua VM Error: " + error).c_str());
    }

    void LuaVM::RegisterGameFunctions() {
        // Print functions
        lua_register(L, "print", lua_print);
        lua_register(L, "warn", lua_warn);
        lua_register(L, "error", lua_error);

        // Game object functions
        lua_register(L, "get_workspace", lua_get_workspace);
        lua_register(L, "get_players", lua_get_players);
        lua_register(L, "get_camera", lua_get_camera);
        lua_register(L, "get_mouse", lua_get_mouse);
        lua_register(L, "get_renderview", lua_get_renderview);
        lua_register(L, "get_visual_engine", lua_get_visual_engine);
        lua_register(L, "get_datamodel", lua_get_datamodel);

        // Player functions
        lua_register(L, "get_local_player", lua_get_local_player);
        lua_register(L, "get_players_list", lua_get_players_list);
        lua_register(L, "get_player_by_name", lua_get_player_by_name);
        lua_register(L, "get_player_character", lua_get_player_character);
        lua_register(L, "get_player_humanoid", lua_get_player_humanoid);

        // Vector3 functions
        lua_register(L, "Vector3", lua_vector3_new);
        lua_register(L, "vector3_add", lua_vector3_add);
        lua_register(L, "vector3_sub", lua_vector3_sub);
        lua_register(L, "vector3_mul", lua_vector3_mul);
        lua_register(L, "vector3_div", lua_vector3_div);
        lua_register(L, "vector3_magnitude", lua_vector3_magnitude);
        lua_register(L, "vector3_normalize", lua_vector3_normalize);
        lua_register(L, "vector3_dot", lua_vector3_dot);
        lua_register(L, "vector3_cross", lua_vector3_cross);
        lua_register(L, "vector3_distance", lua_vector3_distance);

        // CFrame functions
        lua_register(L, "CFrame", lua_cframe_new);
        lua_register(L, "cframe_look_at", lua_cframe_look_at);
        lua_register(L, "cframe_angles", lua_cframe_angles);
        lua_register(L, "cframe_position", lua_cframe_position);
        lua_register(L, "cframe_rotation", lua_cframe_rotation);
        lua_register(L, "cframe_to_world_space", lua_cframe_to_world_space);
        lua_register(L, "cframe_to_object_space", lua_cframe_to_object_space);

        // Camera functions
        lua_register(L, "camera_get_position", lua_camera_get_position);
        lua_register(L, "camera_get_cframe", lua_camera_get_cframe);
        lua_register(L, "camera_get_field_of_view", lua_camera_get_field_of_view);
        lua_register(L, "camera_set_field_of_view", lua_camera_set_field_of_view);
        lua_register(L, "camera_get_near_clip", lua_camera_get_near_clip);
        lua_register(L, "camera_set_near_clip", lua_camera_set_near_clip);
        lua_register(L, "camera_get_far_clip", lua_camera_get_far_clip);
        lua_register(L, "camera_set_far_clip", lua_camera_set_far_clip);

        // Mouse functions
        lua_register(L, "mouse_get_position", lua_mouse_get_position);
        lua_register(L, "mouse_get_world_ray", lua_mouse_get_world_ray);
        lua_register(L, "mouse_get_unit_ray", lua_mouse_get_unit_ray);
        lua_register(L, "mouse_get_x", lua_mouse_get_x);
        lua_register(L, "mouse_get_y", lua_mouse_get_y);
    }

    void LuaVM::RegisterUtilityFunctions() {
        lua_register(L, "wait", lua_wait);
        lua_register(L, "tick", lua_tick);
        lua_register(L, "time", lua_time);
        lua_register(L, "random", lua_random);
        lua_register(L, "randomseed", lua_randomseed);
    }

    void LuaVM::RegisterMathFunctions() {
        lua_register(L, "math_abs", lua_math_abs);
        lua_register(L, "math_acos", lua_math_acos);
        lua_register(L, "math_asin", lua_math_asin);
        lua_register(L, "math_atan", lua_math_atan);
        lua_register(L, "math_atan2", lua_math_atan2);
        lua_register(L, "math_ceil", lua_math_ceil);
        lua_register(L, "math_cos", lua_math_cos);
        lua_register(L, "math_deg", lua_math_deg);
        lua_register(L, "math_exp", lua_math_exp);
        lua_register(L, "math_floor", lua_math_floor);
        lua_register(L, "math_fmod", lua_math_fmod);
        lua_register(L, "math_log", lua_math_log);
        lua_register(L, "math_max", lua_math_max);
        lua_register(L, "math_min", lua_math_min);
        lua_register(L, "math_pow", lua_math_pow);
        lua_register(L, "math_rad", lua_math_rad);
        lua_register(L, "math_sin", lua_math_sin);
        lua_register(L, "math_sqrt", lua_math_sqrt);
        lua_register(L, "math_tan", lua_math_tan);
    }

    void LuaVM::RegisterInputFunctions() {
        // Input functions can be added here
    }

    // Helper function to get VM from Lua state
    LuaVM* GetVMFromState(lua_State* L) {
        return lua_vm.get();
    }

    // Static function implementations
    int LuaVM::lua_print(lua_State* L) {
        int n = lua_gettop(L);
        std::string output;
        for (int i = 1; i <= n; i++) {
            if (i > 1) output += "\t";
            output += lua_tostring(L, i);
        }
        Widgets::LogInfo(output.c_str());
        return 0;
    }

    int LuaVM::lua_warn(lua_State* L) {
        int n = lua_gettop(L);
        std::string output;
        for (int i = 1; i <= n; i++) {
            if (i > 1) output += "\t";
            output += lua_tostring(L, i);
        }
        Widgets::LogWarn(output.c_str());
        return 0;
    }

    int LuaVM::lua_error(lua_State* L) {
        int n = lua_gettop(L);
        std::string output;
        for (int i = 1; i <= n; i++) {
            if (i > 1) output += "\t";
            output += lua_tostring(L, i);
        }
        Widgets::LogInfo(output.c_str());
        return 0;
    }

    // Utility functions
    int LuaVM::lua_wait(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float seconds = luaL_checknumber(L, 1);
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(seconds * 1000)));
        }
        return 0;
    }

    int LuaVM::lua_tick(lua_State* L) {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        lua_pushnumber(L, milliseconds / 1000.0);
        return 1;
    }

    int LuaVM::lua_time(lua_State* L) {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        lua_pushnumber(L, seconds);
        return 1;
    }

    int LuaVM::lua_random(lua_State* L) {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        if (lua_gettop(L) >= 2) {
            float min = luaL_checknumber(L, 1);
            float max = luaL_checknumber(L, 2);
            std::uniform_real_distribution<float> dis(min, max);
            lua_pushnumber(L, dis(gen));
        }
        else if (lua_gettop(L) >= 1) {
            float max = luaL_checknumber(L, 1);
            std::uniform_real_distribution<float> dis(0, max);
            lua_pushnumber(L, dis(gen));
        }
        else {
            std::uniform_real_distribution<float> dis(0, 1);
            lua_pushnumber(L, dis(gen));
        }
        return 1;
    }

    int LuaVM::lua_randomseed(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            int seed = luaL_checkinteger(L, 1);
            srand(seed);
        }
        return 0;
    }

    // Vector3 functions
    int LuaVM::lua_vector3_new(lua_State* L) {
        if (lua_gettop(L) >= 3) {
            float x = luaL_checknumber(L, 1);
            float y = luaL_checknumber(L, 2);
            float z = luaL_checknumber(L, 3);
            sdk::vector3_t vec;
            vec.m_x = x;
            vec.m_y = y;
            vec.m_z = z;
            GetVMFromState(L)->PushVector3(vec);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_vector3_add(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            auto vec1 = GetVMFromState(L)->GetVector3(1);
            auto vec2 = GetVMFromState(L)->GetVector3(2);
            auto result = vec1 + vec2;
            GetVMFromState(L)->PushVector3(result);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_vector3_sub(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            auto vec1 = GetVMFromState(L)->GetVector3(1);
            auto vec2 = GetVMFromState(L)->GetVector3(2);
            auto result = vec1 - vec2;
            GetVMFromState(L)->PushVector3(result);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_vector3_mul(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            auto vec = GetVMFromState(L)->GetVector3(1);
            float scalar = luaL_checknumber(L, 2);
            auto result = vec * scalar;
            GetVMFromState(L)->PushVector3(result);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_vector3_div(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            auto vec = GetVMFromState(L)->GetVector3(1);
            float scalar = luaL_checknumber(L, 2);
            auto result = vec / scalar;
            GetVMFromState(L)->PushVector3(result);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_vector3_magnitude(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            auto vec = GetVMFromState(L)->GetVector3(1);
            float magnitude = vec.magnitude();
            lua_pushnumber(L, magnitude);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_vector3_normalize(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            auto vec = GetVMFromState(L)->GetVector3(1);
            auto normalized = vec.normalize();
            GetVMFromState(L)->PushVector3(normalized);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_vector3_dot(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            auto vec1 = GetVMFromState(L)->GetVector3(1);
            auto vec2 = GetVMFromState(L)->GetVector3(2);
            float dot = vec1.m_x * vec2.m_x + vec1.m_y * vec2.m_y + vec1.m_z * vec2.m_z;
            lua_pushnumber(L, dot);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_vector3_cross(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            auto vec1 = GetVMFromState(L)->GetVector3(1);
            auto vec2 = GetVMFromState(L)->GetVector3(2);
            auto result = vec1.cross(vec2);
            GetVMFromState(L)->PushVector3(result);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_vector3_distance(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            auto vec1 = GetVMFromState(L)->GetVector3(1);
            auto vec2 = GetVMFromState(L)->GetVector3(2);
            auto diff = vec1 - vec2;
            float distance = diff.magnitude();
            lua_pushnumber(L, distance);
            return 1;
        }
        return 0;
    }

    // CFrame functions
    int LuaVM::lua_cframe_new(lua_State* L) {
        if (lua_gettop(L) >= 3) {
            float x = luaL_checknumber(L, 1);
            float y = luaL_checknumber(L, 2);
            float z = luaL_checknumber(L, 3);
            sdk::cframe_t cframe;
            cframe.m_translation.m_x = x;
            cframe.m_translation.m_y = y;
            cframe.m_translation.m_z = z;
            GetVMFromState(L)->PushCFrame(cframe);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_cframe_look_at(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            auto pos = GetVMFromState(L)->GetVector3(1);
            auto look_at = GetVMFromState(L)->GetVector3(2);
            sdk::cframe_t cframe;
            cframe.m_translation = pos;
            GetVMFromState(L)->PushCFrame(cframe);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_cframe_angles(lua_State* L) {
        if (lua_gettop(L) >= 3) {
            float x = luaL_checknumber(L, 1);
            float y = luaL_checknumber(L, 2);
            float z = luaL_checknumber(L, 3);
            sdk::cframe_t cframe;
            cframe.m_translation.m_x = 0;
            cframe.m_translation.m_y = 0;
            cframe.m_translation.m_z = 0;
            GetVMFromState(L)->PushCFrame(cframe);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_cframe_position(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            auto cframe = GetVMFromState(L)->GetCFrame(1);
            GetVMFromState(L)->PushVector3(cframe.m_translation);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_cframe_rotation(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            auto cframe = GetVMFromState(L)->GetCFrame(1);
            GetVMFromState(L)->PushCFrame(cframe);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_cframe_to_world_space(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            auto cframe = GetVMFromState(L)->GetCFrame(1);
            GetVMFromState(L)->PushCFrame(cframe);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_cframe_to_object_space(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            auto cframe = GetVMFromState(L)->GetCFrame(1);
            GetVMFromState(L)->PushCFrame(cframe);
            return 1;
        }
        return 0;
    }

    // Camera functions
    int LuaVM::lua_camera_get_position(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto camera = vm->GetCamera();
        if (camera) {
            auto position = camera->get_camera_translation();
            vm->PushVector3(position);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_camera_get_cframe(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto camera = vm->GetCamera();
        if (camera) {
            sdk::cframe_t cframe;
            cframe.m_translation = camera->get_camera_translation();
            cframe.m_rotation = camera->get_camera_rotation();
            vm->PushCFrame(cframe);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_camera_get_field_of_view(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto camera = vm->GetCamera();
        if (camera) {
            float fov = camera->get_fov();
            lua_pushnumber(L, fov);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_camera_set_field_of_view(lua_State* L) {
        if (lua_gettop(L) < 1) return 0;

        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        float fov = luaL_checknumber(L, 1);
        auto camera = vm->GetCamera();
        if (camera) {
            camera->set_fov(fov);
        }
        return 0;
    }

    int LuaVM::lua_camera_get_near_clip(lua_State* L) {
        lua_pushnumber(L, 0.1f);
        return 1;
    }

    int LuaVM::lua_camera_set_near_clip(lua_State* L) {
        return 0;
    }

    int LuaVM::lua_camera_get_far_clip(lua_State* L) {
        lua_pushnumber(L, 1000.0f);
        return 1;
    }

    int LuaVM::lua_camera_set_far_clip(lua_State* L) {
        return 0;
    }

    // Mouse functions
    int LuaVM::lua_mouse_get_position(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        sdk::vector3_t position;
        position.m_x = 0;
        position.m_y = 0;
        position.m_z = 0;
        vm->PushVector3(position);
        return 1;
    }

    int LuaVM::lua_mouse_get_world_ray(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        lua_newtable(L);
        sdk::vector3_t origin;
        origin.m_x = 0; origin.m_y = 0; origin.m_z = 0;
        lua_pushstring(L, "origin");
        vm->PushVector3(origin);
        lua_settable(L, -3);

        sdk::vector3_t direction;
        direction.m_x = 0; direction.m_y = 0; direction.m_z = 1;
        lua_pushstring(L, "direction");
        vm->PushVector3(direction);
        lua_settable(L, -3);
        return 1;
    }

    int LuaVM::lua_mouse_get_unit_ray(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        lua_newtable(L);
        sdk::vector3_t origin;
        origin.m_x = 0; origin.m_y = 0; origin.m_z = 0;
        lua_pushstring(L, "origin");
        vm->PushVector3(origin);
        lua_settable(L, -3);

        sdk::vector3_t direction;
        direction.m_x = 0; direction.m_y = 0; direction.m_z = 1;
        lua_pushstring(L, "direction");
        vm->PushVector3(direction);
        lua_settable(L, -3);
        return 1;
    }

    int LuaVM::lua_mouse_get_x(lua_State* L) {
        lua_pushnumber(L, 0.0f);
        return 1;
    }

    int LuaVM::lua_mouse_get_y(lua_State* L) {
        lua_pushnumber(L, 0.0f);
        return 1;
    }

    // Math functions
    int LuaVM::lua_math_abs(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::abs(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_acos(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::acos(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_asin(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::asin(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_atan(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::atan(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_atan2(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            float y = luaL_checknumber(L, 1);
            float x = luaL_checknumber(L, 2);
            lua_pushnumber(L, std::atan2(y, x));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_ceil(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::ceil(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_cos(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::cos(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_deg(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, value * 180.0f / M_PI);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_exp(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::exp(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_floor(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::floor(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_fmod(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            float x = luaL_checknumber(L, 1);
            float y = luaL_checknumber(L, 2);
            lua_pushnumber(L, std::fmod(x, y));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_log(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::log(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_max(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            float a = luaL_checknumber(L, 1);
            float b = luaL_checknumber(L, 2);
            lua_pushnumber(L, std::max(a, b));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_min(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            float a = luaL_checknumber(L, 1);
            float b = luaL_checknumber(L, 2);
            lua_pushnumber(L, std::min(a, b));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_pow(lua_State* L) {
        if (lua_gettop(L) >= 2) {
            float base = luaL_checknumber(L, 1);
            float exponent = luaL_checknumber(L, 2);
            lua_pushnumber(L, std::pow(base, exponent));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_rad(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, value * M_PI / 180.0f);
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_sin(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::sin(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_sqrt(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::sqrt(value));
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_math_tan(lua_State* L) {
        if (lua_gettop(L) >= 1) {
            float value = luaL_checknumber(L, 1);
            lua_pushnumber(L, std::tan(value));
            return 1;
        }
        return 0;
    }

    // Helper functions
    void LuaVM::PushVector3(const sdk::vector3_t& vec) {
        lua_newtable(L);
        lua_pushstring(L, "x");
        lua_pushnumber(L, vec.m_x);
        lua_settable(L, -3);
        lua_pushstring(L, "y");
        lua_pushnumber(L, vec.m_y);
        lua_settable(L, -3);
        lua_pushstring(L, "z");
        lua_pushnumber(L, vec.m_z);
        lua_settable(L, -3);
    }

    sdk::vector3_t LuaVM::GetVector3(int index) {
        if (lua_istable(L, index)) {
            lua_getfield(L, index, "x");
            float x = lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, index, "y");
            float y = lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, index, "z");
            float z = lua_tonumber(L, -1);
            lua_pop(L, 1);

            sdk::vector3_t vec;
            vec.m_x = x;
            vec.m_y = y;
            vec.m_z = z;
            return vec;
        }
        sdk::vector3_t vec;
        vec.m_x = 0;
        vec.m_y = 0;
        vec.m_z = 0;
        return vec;
    }

    void LuaVM::PushCFrame(const sdk::cframe_t& cframe) {
        lua_newtable(L);
        lua_pushstring(L, "position");
        PushVector3(cframe.m_translation);
        lua_settable(L, -3);
        lua_pushstring(L, "rotation");
        PushCFrame(cframe);
        lua_settable(L, -3);
    }

    sdk::cframe_t LuaVM::GetCFrame(int index) {
        if (lua_istable(L, index)) {
            lua_getfield(L, index, "position");
            auto position = GetVector3(-1);
            lua_pop(L, 1);

            sdk::cframe_t cframe;
            cframe.m_translation = position;
            return cframe;
        }
        sdk::cframe_t cframe;
        cframe.m_translation.m_x = 0;
        cframe.m_translation.m_y = 0;
        cframe.m_translation.m_z = 0;
        return cframe;
    }

    // Game object getters
    std::shared_ptr<sdk::c_workspace> LuaVM::GetWorkspace() {
        if (!workspace) {
            workspace = std::make_shared<sdk::c_workspace>(0); // Need valid address
        }
        return workspace;
    }

    std::shared_ptr<sdk::c_players> LuaVM::GetPlayers() {
        if (!players) {
            players = std::make_shared<sdk::c_players>(0); // Need valid address
        }
        return players;
    }

    std::shared_ptr<sdk::c_camera> LuaVM::GetCamera() {
        if (!camera) {
            camera = std::make_shared<sdk::c_camera>(0); // Need valid address
        }
        return camera;
    }

    std::shared_ptr<sdk::c_mouse_service> LuaVM::GetMouse() {
        if (!mouse_service) {
            mouse_service = std::make_shared<sdk::c_mouse_service>(0); // Need valid address
        }
        return mouse_service;
    }

    std::shared_ptr<sdk::c_renderview> LuaVM::GetRenderView() {
        if (!renderview) {
            renderview = std::make_shared<sdk::c_renderview>(0); // Need valid address
        }
        return renderview;
    }

    std::shared_ptr<sdk::c_visual_engine> LuaVM::GetVisualEngine() {
        if (!visual_engine) {
            visual_engine = std::make_shared<sdk::c_visual_engine>(0); // Need valid address
        }
        return visual_engine;
    }

    std::shared_ptr<sdk::c_datamodel> LuaVM::GetDataModel() {
        if (!datamodel) {
            datamodel = std::make_shared<sdk::c_datamodel>(0); // Need valid address
        }
        return datamodel;
    }

    // Game object functions
    int LuaVM::lua_get_workspace(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto workspace = vm->GetWorkspace();
        if (workspace) {
            lua_pushlightuserdata(L, workspace.get());
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_get_players(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto players = vm->GetPlayers();
        if (players) {
            lua_pushlightuserdata(L, players.get());
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_get_camera(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto camera = vm->GetCamera();
        if (camera) {
            lua_pushlightuserdata(L, camera.get());
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_get_mouse(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto mouse = vm->GetMouse();
        if (mouse) {
            lua_pushlightuserdata(L, mouse.get());
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_get_renderview(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto renderview = vm->GetRenderView();
        if (renderview) {
            lua_pushlightuserdata(L, renderview.get());
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_get_visual_engine(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto visual_engine = vm->GetVisualEngine();
        if (visual_engine) {
            lua_pushlightuserdata(L, visual_engine.get());
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_get_datamodel(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto datamodel = vm->GetDataModel();
        if (datamodel) {
            lua_pushlightuserdata(L, datamodel.get());
            return 1;
        }
        return 0;
    }

    int LuaVM::lua_get_local_player(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto players = vm->GetPlayers();
        if (players) {
            try {
                auto local_player = players->get_local_player();
                if (local_player) {
                    lua_pushlightuserdata(L, static_cast<void*>(local_player.get()));
                    return 1;
                }
            }
            catch (...) {
                // Handle any exceptions
            }
        }
        return 0;
    }

    int LuaVM::lua_get_players_list(lua_State* L) {
        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        auto players = vm->GetPlayers();
        if (players) {
            try {
                auto players_list = players->get_players();
                lua_newtable(L);
                for (size_t i = 0; i < players_list.size(); i++) {
                    lua_pushinteger(L, i + 1);
                    lua_pushlightuserdata(L, static_cast<void*>(players_list[i].get()));
                    lua_settable(L, -3);
                }
                return 1;
            }
            catch (...) {
                // Handle any exceptions
            }
        }
        return 0;
    }

    int LuaVM::lua_get_player_by_name(lua_State* L) {
        if (lua_gettop(L) < 1 || !lua_isstring(L, 1)) {
            lua_pushnil(L);
            return 1;
        }

        auto vm = GetVMFromState(L);
        if (!vm) return 0;

        std::string name = lua_tostring(L, 1);
        auto players = vm->GetPlayers();
        if (players) {
            try {
                auto players_list = players->get_players();
                for (auto& player : players_list) {
                    if (player->get_name() == name) {
                        lua_pushlightuserdata(L, static_cast<void*>(player.get()));
                        return 1;
                    }
                }
            }
            catch (...) {
                // Handle any exceptions
            }
        }
        lua_pushnil(L);
        return 1;
    }

    int LuaVM::lua_get_player_character(lua_State* L) {
        if (lua_gettop(L) < 1) {
            lua_pushnil(L);
            return 1;
        }

        auto player = static_cast<sdk::c_player*>(lua_touserdata(L, 1));
        if (player) {
            try {
                auto character = player->get_character();
                if (character) {
                    lua_pushlightuserdata(L, static_cast<void*>(character.get()));
                    return 1;
                }
            }
            catch (...) {
                // Handle any exceptions
            }
        }
        lua_pushnil(L);
        return 1;
    }

    int LuaVM::lua_get_player_humanoid(lua_State* L) {
        if (lua_gettop(L) < 1) {
            lua_pushnil(L);
            return 1;
        }

        auto player = static_cast<sdk::c_player*>(lua_touserdata(L, 1));
        if (player) {
            auto character = player->get_character();
            if (character) {
                // Simplified - just return nil for now since humanoid finding is complex
                lua_pushnil(L);
                return 1;
            }
        }
        lua_pushnil(L);
        return 1;
    }
}