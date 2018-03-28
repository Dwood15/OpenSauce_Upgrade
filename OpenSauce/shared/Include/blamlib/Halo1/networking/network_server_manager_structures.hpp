/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

#include <blamlib/Halo1/game/game_configuration.hpp>
#include <blamlib/Halo1/game/players.hpp>
#include <blamlib/Halo1/networking/network_game_globals.hpp>
#include <blamlib/Halo1/networking/network_game_manager_structures.hpp>
#include <blamlib/Halo1/networking/network_server_manager.hpp>

#include <YeloLib/Halo1/open_sauce/blam_memory_upgrades.hpp>

namespace Yelo
{
	namespace MessageDeltas
	{
		struct decoding_information_data;
	};

	namespace Networking
	{
		struct s_network_connection;


		struct s_network_client_machine_message_header
		{
			MessageDeltas::decoding_information_data* decoding_information;
			PAD128; PAD128; PAD128; // unknown bytes
		}; static_assert(sizeof(s_network_client_machine_message_header) == 0x34, STATIC_ASSERT_FAIL);

		struct s_countdown_timer
		{
			int32 time_remaining;
			int32 relative_to; // system tick when time remaining was set
			int32 start_time;
			UNKNOWN_TYPE(bool);
			bool pause_countdown;
			UNKNOWN_TYPE(bool);
			PAD8;
		}; static_assert(sizeof(s_countdown_timer) == 0x10, STATIC_ASSERT_FAIL);
		struct s_network_game_server
		{
			enum {
				//////////////////////////////////////////////////////////////////////////
				// flag bits
				_game_is_open = 0,
				_server_is_dedicated = 2, // doesn't show sv_status info when not set

				k_dedi_offset = s_network_client_machine_dedi::k_network_game_server_client_machines_offset_amount,
			};

			s_network_server_connection* connection;			// 0x0
			Enums::network_game_server_state state;				// 0x4
			word_flags flags;									// 0x6
			s_network_game game;								// 0x8
			s_network_client_machine client_machines	[Enums::k_maximum_network_machine_count];		// 0x3F8
			int32 next_update_number;							// 0x9F8+k_dedi_offset
			int32 time_of_last_keep_alive;						// 0x9FC+k_dedi_offset
			int32 time_of_last_ping;							// 0xA00+k_dedi_offset
			int32 time_of_first_client_loading_completion;		// 0xA04+k_dedi_offset
			s_countdown_timer countdown_timer;					// 0xA08+k_dedi_offset
			s_network_game_player queued_player;				// 0xA18+k_dedi_offset
			bool queued_player_valid;							// 0xA38+k_dedi_offset
			bool game_has_started;								// 0xA39+k_dedi_offset
			UNKNOWN_TYPE(bool);									// 0xA3A
			PAD8;
			wchar_t password
				[Enums::k_network_server_password_length+1];	// 0xA3C+k_dedi_offset
			UNKNOWN_TYPE(bool);									// 0xA4E
			UNKNOWN_TYPE(bool);									// 0xA4F

			bool IsDedi() const { return TEST_FLAG(flags, _server_is_dedicated); }

			s_network_client_machine* GetClient(int machine_index)
			{
				if (machine_index >= 0 && machine_index < NUMBEROF(client_machines))
				{
					for (auto& machine : client_machines)
						if (machine_index == machine.machine_index)
							return &machine;
				}

				return nullptr;
			}
		}; static_assert(sizeof(s_network_game_server) == (0xA50 + s_network_client_machine_dedi::k_network_game_server_client_machines_offset_amount), STATIC_ASSERT_FAIL);

		// For increased player counts game states
		struct s_network_game_server_yelo : s_network_game_server
		{
			s_network_game game_yelo;
			s_network_client_machine client_machines_yelo
				[Enums::k_maximum_network_machine_count_upgrade];

			s_network_client_machine* GetClientYelo(int machine_index)
			{
				if (machine_index >= 0 && machine_index < NUMBEROF(client_machines_yelo))
				{
					for (auto& machine : client_machines_yelo)
						if (machine_index == machine.machine_index)
							return &machine;
				}

				return nullptr;
			}
		};
	};
};