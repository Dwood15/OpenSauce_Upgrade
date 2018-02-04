/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

#include <blamlib/Halo1/game/players.hpp>
#include <blamlib/Halo1/sound/sound_environment_definitions.hpp>

namespace Yelo {
	namespace Scenario {
		struct s_scenario_player_atmospheric_fog // made up name
		{
			bool is_inside; PAD24; //0x0
			UNKNOWN_TYPE(real_vector3d); //0x4
			UNKNOWN_TYPE(real_vector3d); //0x10
			UNKNOWN_TYPE(real_rgb_color); //0x1C
			UNKNOWN_TYPE(real); //0x28
		}; BOOST_STATIC_ASSERT( sizeof(s_scenario_player_atmospheric_fog) == 0x2C );

		struct s_scenario_player_environment_sound {
			bool copy_environment_tag;  //0x0
			PAD24; // never seen this set to true //0x1
			TagGroups::sound_environment environment[Enums::k_maximum_number_of_local_players]; //0x4
		};	BOOST_STATIC_ASSERT(sizeof(s_scenario_player_environment_sound) == (0x4 + (sizeof(TagGroups::sound_environment) * Enums::k_maximum_number_of_local_players)));

		struct s_scenario_globals
		{
			int16 current_structure_bsp_index; //0x0
			PAD16;								//0x2
			s_scenario_player_atmospheric_fog player_fog[Enums::k_maximum_number_of_local_players]; //0x4
			s_scenario_player_environment_sound sound;
		}; 	BOOST_STATIC_ASSERT(sizeof(s_scenario_globals) == ( 0x4 + (Enums::k_maximum_number_of_local_players * sizeof(s_scenario_player_atmospheric_fog) + sizeof(s_scenario_player_environment_sound))));
	};
};