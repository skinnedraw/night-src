#pragma once

#include "../../../utils/memory/memory.hpp"
#include "../offsets/offsets.hpp"
#include "../math/cframe_t.hpp"
#include "../math/matrix_t.hpp"
#include "../math/matrix3_t.hpp"
#include "../math/matrix4_t.hpp"
#include "../math/vector2_t.hpp"
#include "../math/vector3_t.hpp"
#include "../math/vector4_t.hpp"

#include <source/framework/sdk/classes/c_instance.hpp>

namespace sdk
{
	class c_gui_object : public c_instance
	{
	public:
		using c_instance::c_instance;

		inline uint64_t get_frame_x() const
		{
			return g_memory->read<uint64_t>(this->address + sdk::offsets::guiobject::frame_position_x);
		}

		inline uint64_t get_frame_y() const
		{
			return g_memory->read<uint64_t>(this->address + sdk::offsets::guiobject::frame_position_y);
		}

		inline sdk::vector2_t get_frame() const
		{
			return g_memory->read<sdk::vector2_t>(this->address + sdk::offsets::guiobject::position);
		}
		inline void set_frame_x(uint64_t position) const
		{
			g_memory->write<uint64_t>(this->address + sdk::offsets::guiobject::frame_position_x, position);
		}

		inline void set_frame_y(uint64_t position) const
		{
			g_memory->write<uint64_t>(this->address + sdk::offsets::guiobject::frame_position_y, position);
		}

		inline void set_frame(sdk::vector2_t position) const
		{
			g_memory->write<uint64_t>(this->address + sdk::offsets::guiobject::frame_position_x, position.m_x);
			g_memory->write<uint64_t>(this->address + sdk::offsets::guiobject::frame_position_y, position.m_y);
		}
	};
}
