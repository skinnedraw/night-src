#pragma once
#include <cstdint>

#define CLIENT_VERSION "version-80c7b8e578f241ff"

namespace sdk::offsets {

    static inline uintptr_t physicsbandwidth = 0x5FF7EE4;
    static inline uintptr_t largesenderbandwidth = 0x5F3DEE0;

    struct raknet {
        static inline std::uint32_t PingTraceThrottle = 0x6830284;
        static inline std::uint32_t RecvOverloadCooldown = 0x5f43218;
        static inline std::uint32_t PacketTracingInit = 0x5f92c70;
        static inline std::uint32_t StoppedProduction = 0x5f92a10;
        static inline std::uint32_t DecoupleRecvAndUpdate = 0x5f929f0;
        static inline std::uint32_t DetectRecvThreadOverloadPCT = 0x5f4320c;
    };

    struct instance {
        static inline std::uint16_t size = 0x20;
        static inline std::uint16_t string_size = 0x10;
        static inline std::uint16_t name = 0xb0;
        static inline std::uint16_t class_descriptor = 0x18;
        static inline std::uint16_t class_name = 0x8;
        static inline std::uint16_t children = 0x70;
        static inline std::uint16_t children_start = 0x70;
        static inline std::uint16_t children_end = 0x8;
        static inline std::uint16_t parent = 0x68;
        static inline std::uint16_t value = 0xd0;
        static inline std::uint16_t attribute_container = 0x48;
        static inline std::uint16_t attribute_list = 0x18;
        static inline std::uint16_t attribute_to_next = 0x58;
        static inline std::uint16_t attribute_to_value = 0x18;
        static inline std::uint16_t class_base = 0xc58;
    };

    struct datamodel {
        static inline std::uintptr_t pointer = 0x7C75728;
        static inline std::uint16_t realdatamodel = 0x1c0;
        static inline std::uint16_t workspace = 0x178;
        static inline std::uint16_t gameid = 0x190;
        static inline std::uint16_t is_loaded = 0x5F8;
        static inline std::uint16_t game_loaded = 0x5F8;
        static inline std::uint16_t creator_id = 0x188;
        static inline std::uint16_t job_id = 0x138;
        static inline std::uint16_t place_id = 0x198;
        static inline std::uint16_t place_version = 0x1b4;
        static inline std::uint16_t primitive_count = 0x440;
        static inline std::uint16_t server_ip = 0x5E0;
        static inline std::uint16_t script_context = 0x3f0;
    };

    struct primitives {
        static inline std::uint16_t primitive = 0x148;
        static inline std::uint16_t rotation = 0xc0;
        static inline std::uint16_t orentation = 0xc0;
        static inline std::uint16_t translation = 0xE4;
        static inline std::uint16_t position = 0xE4;
        static inline std::uint16_t velocity = 0xf0;
        static inline std::uint16_t assembly_linear_velocity = 0xf0;
        static inline std::uint16_t size = 0x1b0;
        static inline std::uint16_t cframe = 0xc0;
        static inline std::uint16_t angular_velocity = 0xfc;
        static inline std::uint16_t assembly_angular_velocity = 0xfc;
        static inline std::uint16_t color3 = 0x194;
        static inline std::uint16_t shape = 0x1b1;
        static inline std::uint16_t material = 0x246;
        static inline std::uint16_t primitive_owner = 0x1f0;
        static inline std::uint16_t primitive_flags = 0x1ae;
        static inline std::uint16_t transparency = 0xf0;
        static inline std::uint16_t validate_primitive = 0x6;
    };

    struct guiobject {
        static inline std::uint16_t background_color3 = 0x548;
        static inline std::uint16_t border_color3 = 0x554;
        static inline std::uint16_t image = 0xa20;
        static inline std::uint16_t layout_order = 0x584;
        static inline std::uint16_t position = 0x518;
        static inline std::uint16_t rich_text = 0xae0;
        static inline std::uint16_t rotation = 0x188;
        static inline std::uint16_t screen_gui_enabled = 0x4d4;
        static inline std::uint16_t size = 0x538;
        static inline std::uint16_t text = 0xe40;
        static inline std::uint16_t text_color3 = 0xef0;
        static inline std::uint16_t visible = 0x5B1;
        static inline std::uint16_t frame_position_x = 0x518;
        static inline std::uint16_t frame_position_y = 0x520;
    };

    struct lighting {
        static inline std::uint16_t ambient = 0xd8;
        static inline std::uint16_t brightness = 0x120;
        static inline std::uint16_t clock_time = 0x1b8;
        static inline std::uint16_t color_shift_bottom = 0xE4;
        static inline std::uint16_t color_shift_top = 0xF0;
        static inline std::uint16_t exposure_compensation = 0x12c;
        static inline std::uint16_t fog_color = 0xfc;
        static inline std::uint16_t fog_end = 0x134;
        static inline std::uint16_t fog_start = 0x138;
        static inline std::uint16_t geographic_latitude = 0x190;
        static inline std::uint16_t outdoor_ambient = 0x108;
    };

    struct camera {
        static inline std::uint16_t rotation = 0xf8;
        static inline std::uint16_t translation = 0x11c;
        static inline std::uint16_t position = 0x11c;
        static inline std::uint16_t field_of_view = 0x160;
        static inline std::uint16_t offset = 0x158;
        static inline std::uint16_t camera_type = 0x158;
        static inline std::uint16_t subject = 0xe8;
        static inline std::uint16_t camera_subject = 0xe8;
    };

    struct workspace {
        static inline std::uint16_t camera = 0x460;
        static inline std::uint16_t current_camera = 0x460;
        static inline std::uint16_t distributed_game_time = 0x470;
        static inline std::uint16_t gravity = 0x26d;
        static inline std::uint16_t gravity_container = 0x70;
        static inline std::uint16_t primitives_ptr1 = 0x3c8;
        static inline std::uint16_t primitives_pointer1 = 0x3c8;
        static inline std::uint16_t primitives_ptr2 = 0x240;
        static inline std::uint16_t primitives_pointer2 = 0x240;
        static inline std::uint16_t read_only_gravity = 0x940;
        static inline std::uint16_t world = 0x3D8;
    };

    struct players {
        static inline std::uint16_t local_player = 0x130;
        static inline std::uint16_t max_players = 0x138;
    };

    struct player {
        static inline std::uint16_t display = 0x130;
        static inline std::uint16_t display_name = 0x130;
        static inline std::uint16_t team = 0x290;
        static inline std::uint16_t user_id = 0x2B8;
        static inline std::uint16_t character = 0x380;
        static inline std::uint16_t model_instance = 0x380;
        static inline std::uint16_t animation_id = 0xd0;
        static inline std::uint16_t camera_mode = 0x2f8;
        static inline std::uint16_t country = 0x110;
        static inline std::uint16_t gender = 0xe68;
        static inline std::uint16_t max_zoom_distance = 0x2f0;
        static inline std::uint16_t min_zoom_distance = 0x2f4;
        static inline std::uint16_t mouse = 0xcd8;
    };

    struct humanoid {
        static inline std::uint16_t health = 0x194;
        static inline std::uint16_t max_health = 0x1b4;
        static inline std::uint16_t hip_height = 0x1a0;
        static inline std::uint16_t jump_power = 0x1b0;
        static inline std::uint16_t walk_speed = 0x1d4;
        static inline std::uint16_t walkspeed = 0x1d4;
        static inline std::uint16_t walk_speed_check = 0x3c0;
        static inline std::uint16_t humanoid_state = 0x8d8;
        static inline std::uint16_t platform_stand = 0x1d9;
        static inline std::uint16_t auto_rotate = 0x1d9;
        static inline std::uint16_t move_direction = 0x158;
        static inline std::uint16_t humanoid_state_id = 0x20;
        static inline std::uint16_t jump_height = 0x1ac;
        static inline std::uint16_t max_slope_angle = 0x1b8;
        static inline std::uint16_t rig_type = 0x1c8;
        static inline std::uint16_t jump = 0x1dd;
        static inline std::uint16_t floor_material = 0x190;
    };

    struct animationtrack {
        static inline std::uint16_t animation = 0xd0;
        static inline std::uint16_t animator = 0x118;
        static inline std::uint16_t is_playing = 0x518;
        static inline std::uint16_t looped = 0xf5;
        static inline std::uint16_t speed = 0xe4;
    };

    struct renderview {
        static inline std::uint16_t device_d3d11 = 0x8;
        static inline std::uint16_t visual_engine = 0x10;
    };

    struct visualengine {
        static inline std::uint16_t view_matrix = 0x120;
        static inline std::uint16_t view_port = 0x720;
        static inline std::uint16_t dimensions = 0x720;
        static inline std::uintptr_t pointer = 0x775E8D0;
        static inline std::uint16_t to_datamodel1 = 0x700;
        static inline std::uint16_t to_datamodel2 = 0x1c0;
        static inline std::uint16_t render_view = 0x800;
    };

    struct taskscheduler {
        static inline std::uintptr_t pointer = 0x7e1bc88;
        static inline std::uint16_t job_end = 0x1d8;
        static inline std::uint16_t job_name = 0x18;
        static inline std::uint16_t job_start = 0x1d0;
        static inline std::uint16_t max_fps = 0x1b0;
        static inline std::uint16_t render_view = 0x218;
        static inline std::uint16_t render_job_to_render_view = 0x218;
        static inline std::uint16_t render_job_to_fakedm = 0x38;
        static inline std::uint16_t fake_dm_to_dm = 0x1b0;
        static inline std::uint16_t fake_datamodel_to_datamodel = 0x1b0;
        static inline std::uint16_t script_context = 0x3f0;
    };

    struct mouseservice {
        static inline std::uint16_t mouse_position_x = 0xf4;
        static inline std::uint16_t mouse_position_y = 0xf8;
        static inline std::uint16_t input_object = 0x110;
        static inline std::uint16_t mouse_position = 0xEC;
        static inline std::uintptr_t sensitivity_pointer = 0x7dae210;
    };

} // namespace sdk::offsets
