/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\OpenSauce for specific license information
*/
#pragma once

#include <blamlib/memory/data_base.hpp>
#include <blamlib/Halo1/game/game_configuration.hpp>
#include <blamlib/Halo1/hs/hs_runtime.hpp>
#include <blamlib/Halo1/hs/hs_threads.hpp>

namespace Yelo
{
	namespace TagGroups
	{
		struct hs_script;
	};

	namespace Scripting
	{
		struct s_hs_globals_datum : Memory::s_datum_base
		{
			word_flags yelo_flags;

			s_hs_value_union value;
		}; static_assert(sizeof(s_hs_globals_datum) == 0x8, STATIC_ASSERT_FAIL);

		struct s_hs_runtime_globals
		{
			bool enabled;
			PAD24;
			int16 executing_thread_index;
			bool run_game_scripts;
			PAD8;
		}; static_assert(sizeof(s_hs_runtime_globals) == 0x8, STATIC_ASSERT_FAIL);
	};
};