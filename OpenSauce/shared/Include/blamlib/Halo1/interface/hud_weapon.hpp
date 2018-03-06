/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

#include <blamlib/Halo1/game/game_configuration.hpp>
#include <blamlib/Halo1/interface/weapon_hud_interface_definition.hpp>

namespace Yelo {
	namespace Flags {
		enum weapon_interface_show {
			_weapon_interface_show_crosshair_bit,
		};
	};

	namespace GameUI {
		struct s_hud_weapon_interface_weapon_state {
			game_ticks_t first_render_times[Enums::k_number_of_weapon_hud_states];
			datum_index weapon_index;
			game_ticks_t grenades_first_render_time;
		}; static_assert(sizeof(s_hud_weapon_interface_weapon_state) == 0x28, STATIC_ASSERT_FAIL);

		struct s_hud_weapon_interface_crosshairs_state
		{
			game_ticks_t first_render_times[Enums::k_number_of_weapon_crosshair_types];
			long_flags render_types_mask[BIT_VECTOR_SIZE_IN_DWORDS(Enums::k_number_of_weapon_crosshair_types)];
		}; static_assert(sizeof(s_hud_weapon_interface_crosshairs_state) == 0x50, STATIC_ASSERT_FAIL);

		struct s_local_player_hud_weapon_interface {
			s_hud_weapon_interface_weapon_state weapon;
			s_hud_weapon_interface_crosshairs_state crosshairs;
		}; static_assert(sizeof(s_local_player_hud_weapon_interface) == 0x78, STATIC_ASSERT_FAIL);

		struct s_hud_weapon_interface {
			s_local_player_hud_weapon_interface local_players[Enums::k_maximum_number_of_local_players];
			long_flags show_flags;
		}; static_assert(sizeof(s_hud_weapon_interface) == 0x4 + (sizeof(s_local_player_hud_weapon_interface) * Enums::k_maximum_number_of_local_players));
		s_hud_weapon_interface*		HudWeaponInterface();
	};
};