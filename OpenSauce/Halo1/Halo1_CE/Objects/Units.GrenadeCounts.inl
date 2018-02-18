/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/

#include <blamlib/Halo1/units/unit_structures.hpp>

#include "Memory/MemoryInterface.hpp"

namespace Yelo { namespace Objects { namespace Units {

// DEBUG: Before each memory write when we're enabling, we assert we're overwriting the expected value or bytes
#include <YeloLib/Halo1/units/units_grenade_count_upgrade.inl>

static void InitializeGrenadeCounts_UnitGrenadeCounts(bool enabled)
{
	//////////////////////////////////////////////////////////////////////////
	// actor_died
	{
		typedef Memory::c_naked_func_writer_with_undo<	unit_grenade_counts_mods::actor_died__unit_grenade_count_word,
														unit_grenade_counts_mods::actor_died__unit_grenade_count_dword>
			code_writer;

		void* code_addr = GET_FUNC_VPTR(ACTOR_DIED_UNIT_GRENADE_COUNT_MOD);

		if(enabled)	code_writer::Write(code_addr DebugOnly(, "GrenadeCounts asm mismatch"));
		else		code_writer::Undo(code_addr);
	}
	//////////////////////////////////////////////////////////////////////////
	// player_add_equipment
	{
		typedef Memory::c_naked_func_writer_with_undo<	unit_grenade_counts_mods::player_add_equipment__unit_grenade_count_word,
														unit_grenade_counts_mods::player_add_equipment__unit_grenade_count_dword>
			code_writer;

		void* code_addr = GET_FUNC_VPTR(PLAYER_ADD_EQUIPMENT_UNIT_GRENADE_COUNT_MOD);

		if(enabled)	code_writer::Write(code_addr DebugOnly(, "GrenadeCounts asm mismatch"));
		else		code_writer::Undo(code_addr);
	}
	//////////////////////////////////////////////////////////////////////////
	// biped_new_from_network
	{
		typedef Memory::c_naked_func_writer_with_undo<	unit_grenade_counts_mods::biped_new_from_network__unit_grenade_count_word,
														unit_grenade_counts_mods::biped_new_from_network__unit_grenade_count_dword>
			code_writer;

		void* code_addr = GET_FUNC_VPTR(BIPED_NEW_FROM_NETWORK_UNIT_GRENADE_COUNT_MOD);

		if(enabled)	code_writer::Write(code_addr DebugOnly(, "GrenadeCounts asm mismatch"));
		else		code_writer::Undo(code_addr);
	}
	//////////////////////////////////////////////////////////////////////////
	// biped_update_baseline
	{
		typedef Memory::c_naked_func_writer_with_undo<	unit_grenade_counts_mods::biped_update_baseline__unit_grenade_count_word1,
														unit_grenade_counts_mods::biped_update_baseline__unit_grenade_count_dword1>
			code_writer1;
		typedef Memory::c_naked_func_writer_with_undo<	unit_grenade_counts_mods::biped_update_baseline__unit_grenade_count_word2,
														unit_grenade_counts_mods::biped_update_baseline__unit_grenade_count_dword2>
			code_writer2;

		void* code_addr = GET_FUNC_VPTR(BIPED_UPDATE_BASELINE_UNIT_GRENADE_COUNT_MOD1);

		if(enabled)	code_writer1::Write(code_addr DebugOnly(, "GrenadeCounts asm mismatch"));
		else		code_writer1::Undo(code_addr);

		code_addr = GET_FUNC_VPTR(BIPED_UPDATE_BASELINE_UNIT_GRENADE_COUNT_MOD2);

		if(enabled)	code_writer2::Write(code_addr DebugOnly(, "GrenadeCounts asm mismatch"));
		else		code_writer2::Undo(code_addr);
	}
	//////////////////////////////////////////////////////////////////////////
	// biped_build_update_delta
	{
		typedef Memory::c_naked_func_writer_with_undo<	unit_grenade_counts_mods::biped_build_update_delta__unit_grenade_count_word1,
														unit_grenade_counts_mods::biped_build_update_delta__unit_grenade_count_dword1>
			code_writer1;
		typedef Memory::c_naked_func_writer_with_undo<	unit_grenade_counts_mods::biped_build_update_delta__unit_grenade_count_word2,
														unit_grenade_counts_mods::biped_build_update_delta__unit_grenade_count_dword2>
			code_writer2;

		void* code_addr = GET_FUNC_VPTR(BIPED_BUILD_UPDATE_DELTA_UNIT_GRENADE_COUNT_MOD1);

		if(enabled)	code_writer1::Write(code_addr DebugOnly(, "GrenadeCounts asm mismatch"));
		else		code_writer1::Undo(code_addr);

		code_addr = GET_FUNC_VPTR(BIPED_BUILD_UPDATE_DELTA_UNIT_GRENADE_COUNT_MOD2);

		if(enabled)	code_writer2::Write(code_addr DebugOnly(, "GrenadeCounts asm mismatch"));
		else		code_writer2::Undo(code_addr);
	}
	//////////////////////////////////////////////////////////////////////////
	// biped_process_update_delta
	{
		typedef Memory::c_naked_func_writer_with_undo<	unit_grenade_counts_mods::biped_process_update_delta__unit_grenade_count_word,
														unit_grenade_counts_mods::biped_process_update_delta__unit_grenade_count_dword>
			code_writer;

		void* code_addr = GET_FUNC_VPTR(BIPED_PROCESS_UPDATE_DELTA_UNIT_GRENADE_COUNT_MOD);

		if(enabled)	code_writer::Write(code_addr DebugOnly(, "GrenadeCounts asm mismatch"));
		else		code_writer::Undo(code_addr);
	}

	// TODO: crosshairs_draw
}


static void InitializeGrenadeCounts_MessageDeltaGrenadeCounts(bool enabled) { }

}; 
}; 
};