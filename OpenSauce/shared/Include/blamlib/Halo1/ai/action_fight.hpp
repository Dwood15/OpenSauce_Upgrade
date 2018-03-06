/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

namespace Yelo
{
	namespace AI
	{
		struct s_action_fight_state
		{
			UNKNOWN_TYPE(int16); // timer
			PAD16;
		}; static_assert(sizeof(s_action_fight_state) == 0x4, STATIC_ASSERT_FAIL);
	};
};