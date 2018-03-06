/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

#include <blamlib/Halo1/game/game_configuration.hpp>

namespace Yelo {
	namespace Enums {
		enum {
			k_maximum_number_of_hud_sounds = 12, // MAXIMUM_NUMBER_OF_HUD_SOUNDS
		};

		enum hud_auxilary_meter_type : __int16 { //Or _enum, if that fits your fancy
			_hud_auxilary_meter_type_integrated_light,

			k_number_of_hud_auxilary_meter_types,
		};
	};

	namespace Flags {
		enum unit_interface_flags : unsigned long { //long_flags
			_unit_interface_show_health_bit,
			_unit_interface_blink_health_bit,
			_unit_interface_show_shield_bit,
			_unit_interface_blink_shield_bit,
			_unit_interface_show_motion_sensor_bit,
			_unit_interface_blink_motion_sensor_bit,
		};
	};
	typedef float real;
	typedef unsigned long long_flags;
	
	namespace GameUI {
		struct s_first_render_time {
			game_ticks_t hud_background_element; //0x0
			game_ticks_t health_panel_background_element; //0x4
			game_ticks_t motion_sensor_elements; //0x8
		}; static_assert(sizeof(s_first_render_time) == (sizeof(game_ticks_t)* 0x3), STATIC_ASSERT_FAIL); //0xC

		struct s_hud_unit_interface_unit {
			real shields; //0x0
			real health; //0x4
			UNKNOWN_TYPE(real); //0x8 // shield related (for gauging how much the last damage dealt, I believe)
			game_ticks_t last_update_time; //0xC

			s_first_render_time first_render_time; //0x10

			datum_index unit_index; //0x1C
			word_flags auxilary_meters_active[BIT_VECTOR_SIZE_IN_WORDS(Enums::k_number_of_hud_auxilary_meter_types)]; //0x20
			game_time_t auxilary_meter_times[Enums::k_number_of_hud_auxilary_meter_types]; //0x1E
			word_flags active_sound_elements;
			PAD16;
			datum_index sound_elements[Enums::k_maximum_number_of_hud_sounds]; // sound cache index
		}; static_assert(sizeof(s_hud_unit_interface_unit) == 0x58, STATIC_ASSERT_FAIL);

		struct s_hud_unit_interface
		{
			s_hud_unit_interface_unit units[Enums::k_maximum_number_of_local_players]; // 0x0
			long_flags flags; // 0x58, unit_interface_flags
		}; static_assert(sizeof(s_hud_unit_interface) == 0x4 + (sizeof(s_hud_unit_interface_unit)* Enums::k_maximum_number_of_local_players), STATIC_ASSERT_FAIL); //should be == 0x5c
		s_hud_unit_interface*		HudUnitInterface();
	};
};