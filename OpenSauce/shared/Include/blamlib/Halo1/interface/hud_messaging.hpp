/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

namespace Yelo
{
	namespace GameUI
	{
		struct s_hud_messaging : TStructImpl(0x488) { };
		BOOST_STATIC_ASSERT(sizeof(s_hud_messaging) == 0x22 + 0x460 * Enums::k_maximum_number_of_local_players )
		
		s_hud_messaging*			HudMessaging();
	};

	namespace blam
	{
		void PLATFORM_API hud_print_message(int16 local_player_index, wcstring message);
	};
};