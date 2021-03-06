/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

//#include <blamlib/Halo1/bungie_net/common/message_header.hpp>
#include <blamlib/Halo1/game/game_configuration.hpp>
#include <blamlib/Halo1/game/player_action.hpp>
#include <blamlib/Halo1/game/players.hpp>
#include <blamlib/Halo1/networking/network_game_globals.hpp>
#include <blamlib/Halo1/networking/network_game_manager.hpp>
#include "network_game_manager_structures.hpp"

namespace Yelo {
	namespace Networking {
		struct message_server_new_client_challenge {
			char challenge[Enums::k_network_game_challenge_length+1];
			bool sv_public;
			PAD24;
			int16 machine_index;
			PAD16;
			s_network_game_map map;
		}; static_assert(sizeof(message_server_new_client_challenge) == 0x94, STATIC_ASSERT_FAIL);

		struct message_client_join_game_request {
			char token[Enums::k_network_game_join_token_size];
			wchar_t password[Enums::k_network_server_password_length+1];
			char unused[73];
			byte_enum connection_class;										// 0x6B Enums::network_connection_class
			PAD16;
			s_network_game_player player;									// 0x6E
			PAD16;
			uint32 map_crc_xor_challenge;									// 0x90
		}; static_assert(sizeof(message_client_join_game_request) == 0x94, STATIC_ASSERT_FAIL);

		struct message_server_game_update {
			int32 update_id; //0x0
			int32 random_seed; //0x4

			game_ticks_t ticks_to_apply_update_to; //0x8
			UNKNOWN_TYPE(int16); //0xC - unused?
			int16 action_count; //0xE
			Players::s_player_action actions[Enums::k_multiplayer_maximum_players]; //0x10
		}; static_assert(sizeof(message_server_game_update) == 0x10 + (sizeof(Players::s_player_action) * Enums::k_multiplayer_maximum_players), STATIC_ASSERT_FAIL);

		struct message_client_game_update {
			game_ticks_t ticks_to_apply_update_to; //0x0
			UNKNOWN_TYPE(int16); // 0x4 unused?
			int16 action_count; //0x6 should be equal to local_player_count
			Players::s_player_action actions[Enums::k_maximum_number_of_local_players]; //0x8
		}; static_assert(sizeof(message_client_game_update) == (0x8 + (sizeof(Players::s_player_action) * Enums::k_maximum_number_of_local_players)), STATIC_ASSERT_FAIL);
	};
};