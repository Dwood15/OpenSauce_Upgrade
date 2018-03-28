/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#include "Common/Precompile.hpp"
#include "Game/EngineFunctions.hpp"

#include <blamlib/Halo1/cache/cache_files.hpp>
#include <blamlib/Halo1/camera/director.hpp>
#include <blamlib/Halo1/interface/hud_draw.hpp>
#include <blamlib/Halo1/main/console.hpp>
#include <blamlib/Halo1/main/main_structures.hpp>
#include <blamlib/Halo1/math/periodic_functions.hpp>
#include <blamlib/Halo1/memory/data.hpp>
#include <blamlib/Halo1/models/model_animation_definitions.hpp>
#include <blamlib/Halo1/objects/damage.hpp>
#include <blamlib/Halo1/objects/objects.hpp>
#include <blamlib/Halo1/tag_files/tag_groups.hpp>
#include <blamlib/Halo1/ai/actor_structures.hpp>
#include <blamlib/Halo1/units/unit_structures.hpp>
#include <blamlib/Halo1/units/unit_script.hpp>

#include "Game/Console.hpp"
#include "Game/GameState.hpp"
#include "Game/ScriptLibrary.hpp"
#include "Objects/Objects.hpp"

// comment the following to just use the parameters as-is 
// instead of copying to the stack. I believe there was some 
// concurrency issues originally which caused me to use local 
// buffers instead...
#define ENGINE_FUNCTIONS_USE_LOCAL
enum { k_engine_function_string_buffer_size = 512, };

#define API_FUNC_NAKED_END_NO_STACK_POP()	\
		__asm pop	ebp						\
		__asm retn							\
	}

namespace Yelo
{
	namespace Physics
	{
		struct s_collision_result;
	};

	namespace Players
	{
		struct s_unit_camera_info;
	};

	namespace TagGroups
	{
		struct s_bitmap_data;

		struct s_sound_permutation;

		struct predicted_resource;
	};

	namespace Engine
	{
#define __EL_INCLUDE_ID			__EL_INCLUDE_GAME
#define __EL_INCLUDE_FILE_ID	__EL_GAME_ENGINE_FUNCTIONS
#include "Memory/_EngineLayout.inl"

		#include "Game/EngineFunctions._Misc.inl"

		namespace Cache
		{
			API_FUNC_NAKED bool FileReadRequest(/*datum_index tag_index,*/
				uint32 offset_, uint32 size_, void* buffer, const Yelo::Cache::s_cache_file_request_params& params, 
				bool block, Enums::cache_file_request_source source)
			{
				static const uintptr_t FUNCTION = GET_FUNC_PTR(CACHE_FILE_READ_REQUEST);

				API_FUNC_NAKED_START()
					push	esi
					xor		eax, eax

					movzx	eax, source
					push	eax
					movzx	eax, block
					push	eax
					mov		esi, params
					push	buffer
					push	size_
					push	offset_
					call	FUNCTION
					add		esp, 4 * 5

					pop		esi
					API_FUNC_NAKED_END(6); // not including the unused tag_index
			}
		};

		namespace Effects
		{
			void API_FUNC_NAKED NewOnObjectMarker(datum_index effect_definition_index, datum_index object_index, cstring marker_name)
			{
				static const uintptr_t FUNCTION = GET_FUNC_PTR(EFFECT_NEW_ON_OBJECT_MARKER);

				API_FUNC_NAKED_START()
					push	esi
					push	edi
					push	ecx

					mov		ecx, marker_name
					push	ecx
					mov		esi, object_index
					mov		edi, effect_definition_index
					call	FUNCTION
					add		esp, 4 * 1

					pop		ecx
					pop		edi
					pop		esi
				API_FUNC_NAKED_END(3)
			}
		};

		namespace HS
		{
			void ObjectListAdd(datum_index object_list, datum_index object_index)
			{
				static const uintptr_t FUNCTION = GET_FUNC_PTR(OBJECT_LIST_ADD);

				if(object_list.IsNull()) return;

				__asm {
					push	object_index
					mov		eax, object_list
					call	FUNCTION
					add		esp, 4 * 1
				}
			}
		};

		namespace Objects
		{
			#include "Game/EngineFunctions.Objects.inl"
		};

		namespace Players
		{
			#include "Game/EngineFunctions.Players.inl"
		};

		namespace Scenario
		{
			bool TriggerVolumeTestPoint(int32 trigger_volume_index, const real_point3d& point)
			{
				static const uintptr_t FUNCTION = GET_FUNC_PTR(SCENARIO_TRIGGER_VOLUME_TEST_POINT);

				__asm {
					mov		ecx, point
					mov		eax, trigger_volume_index
					call	FUNCTION
				}
			}

			bool TriggerVolumeTestObject(int32 trigger_volume_index, datum_index object)
			{
				if(object.IsNull())
					return false;

				Yelo::Objects::s_object_data* obj = Yelo::Objects::ObjectHeader()[object]->_object;

				return TriggerVolumeTestPoint(trigger_volume_index, obj->center);
			}
		};

		namespace TagGroups
		{
			void PredictResources(TagBlock<Yelo::TagGroups::predicted_resource>& resources_block)
			{
#if !PLATFORM_IS_DEDI // dedi doesn't need to predict resources since it doesn't render bitmaps or play sounds
				static const uintptr_t FUNCTION = GET_FUNC_PTR(PHYSICAL_MEMORY_MAP_PREDICT_RESOURCES);

				__asm {
					push	esi

					mov		esi, resources_block
					call	FUNCTION

					pop		esi
				}
#endif
			}

			datum_index TagLoaded(tag group_tag, cstring name)
			{
				static const uintptr_t FUNCTION = GET_FUNC_PTR(TAG_LOADED);

#if defined(ENGINE_FUNCTIONS_USE_LOCAL)
				char local[256];
				memset(local, 0, sizeof(local));
				strcpy_s(local, name);
#else
				cstring local = name;
#endif

				__asm {
					push	edi

#if defined(ENGINE_FUNCTIONS_USE_LOCAL)
					lea		eax, local
#else
					mov		eax, local
#endif
					push	eax
					mov		edi, group_tag
					call	FUNCTION
					add		esp, 4 * 1

					pop		edi
				}
			}

			wcstring UnicodeStringListGetString(datum_index ustr, int32 index)
			{
				static const uintptr_t FUNCTION = GET_FUNC_PTR(UNICODE_STRING_LIST_GET_STRING);

				__asm {
					mov		edx, index
					mov		ecx, ustr
					call	FUNCTION
				}
			}
		};
	};

	//////////////////////////////////////////////////////////////////////////
	// ai
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// actors.c
		API_FUNC_NAKED void PLATFORM_API actor_delete(datum_index actor_index, bool is_dead)
		{
				static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(ACTOR_DELETE);

				API_FUNC_NAKED_START()
					pushad

					movzx	eax, is_dead
					push	eax
					mov		ebx, actor_index
					call	FUNCTION
					add		esp, 4

					popad
					pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED void PLATFORM_API actor_update(const datum_index actor_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(ACTOR_UPDATE);

			API_FUNC_NAKED_START()
				push	eax
				push	ecx
				push	edx
				push	esi

				mov		esi, actor_index
				call	FUNCTION

				pop		esi
				pop		edx
				pop		ecx
				pop		eax
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED void PLATFORM_API actor_customize_unit(const datum_index actor_variant, const datum_index unit_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(ACTOR_CUSTOMIZE_UNIT);

			API_FUNC_NAKED_START()
				push	eax
				push	ecx
				push	edx

				mov		eax, actor_variant
				push	unit_index
				call	FUNCTION
				add		esp, 4
				
				pop		edx
				pop		ecx
				pop		eax
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED void PLATFORM_API actor_set_active(const datum_index actor_index, const bool active)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(ACTOR_SET_ACTIVE);

			API_FUNC_NAKED_START()
				push	eax
				push	edi

				xor		eax, eax
				mov		al, active
				mov		edi, actor_index
				call	FUNCTION

				pop		edi
				pop		eax
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED void PLATFORM_API actor_set_dormant(const datum_index actor_index, const bool dormant)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(ACTOR_SET_DORMANT);

			API_FUNC_NAKED_START()
				pushad

				xor		ebx, ebx
				mov		bl, dormant
				mov		eax, actor_index
				call	FUNCTION

				popad
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED void PLATFORM_API actor_delete_props(const datum_index actor_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(ACTOR_DELETE_PROPS);

			_asm jmp	FUNCTION;
		}

		API_FUNC_NAKED void PLATFORM_API actor_freeze(const datum_index actor_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(ACTOR_FREEZE);

			API_FUNC_NAKED_START()
				push	eax
				push	ecx
				push	edx

				mov		eax, actor_index
				call	FUNCTION
				
				pop		edx
				pop		ecx
				pop		eax
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		void PLATFORM_API actor_braindead(const datum_index actor_index, const bool braindead)
		{
			const auto actor_data = AI::Actors()[actor_index];

			if(braindead)
			{
				actor_data->current_state = Enums::_actor_default_state_none;
				actor_data->state.action = Enums::_actor_action_none;

				actor_delete_props(actor_index);
				actor_freeze(actor_index);
				actor_set_dormant(actor_index, 0);
			}
			else
			{
				if(actor_data->current_state == Enums::_actor_default_state_none)
				{
					actor_data->current_state = Enums::_actor_default_state_alert;
				}
			}
		}

		API_FUNC_NAKED datum_index PLATFORM_API actor_create_for_unit(const bool is_swarm
			, const datum_index unit_index
			, const datum_index actor_variant
			, const datum_index encounter_index
			, const int32 squad_index
			, const int32 arg7
			, const int32 arg6
			, const bool magic_sight_after_timer
			, const Enums::actor_default_state initial_state
			, const Enums::actor_default_state return_state
			, const int32 command_list_index
			, const int32 sequence_id)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(ACTOR_CREATE_FOR_UNIT);

			_asm jmp	FUNCTION;
		}

		API_FUNC_NAKED void PLATFORM_API actor_action_change(const datum_index actor_index
			, const Enums::actor_action new_action_type
			, const byte* new_action_data)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(ACTOR_ACTION_CHANGE);

			_asm jmp	FUNCTION;
		}

		//////////////////////////////////////////////////////////////////////////
		// ai_script.c
		API_FUNC_NAKED void PLATFORM_API ai_scripting_attach_free(datum_index unit_index, datum_index actor_variant_definition_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(AI_SCRIPTING_ATTACH_FREE);

			API_FUNC_NAKED_START()
				push	eax
				
				mov		eax, actor_variant_definition_index
				push	unit_index
				call	FUNCTION
				add		esp, 4

				pop		eax
				pop		ebp
			API_FUNC_NAKED_END_()
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// bink
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// bink_playback.c
		API_FUNC_NAKED void PLATFORM_API bink_playback_start(cstring bik_path)
		{
#if !PLATFORM_IS_DEDI
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(BINK_PLAYBACK_START);

			API_FUNC_NAKED_START()
				push	bik_path
				call	FUNCTION
				add		esp, 4 * 1
			API_FUNC_NAKED_END_NO_STACK_POP()
#else
			API_FUNC_NAKED_START_()
			API_FUNC_NAKED_END_()
#endif
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// cache
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// cache_files_windows.c
		bool PLATFORM_API cache_file_read_request(/*datum_index tag_index,*/
			uint32 offset, uint32 size, void* buffer, const Cache::s_cache_file_request_params& params, 
			bool block, Enums::cache_file_request_source source)
		{
			return Engine::Cache::FileReadRequest(offset, size, buffer, params, block, source);
		}
		//////////////////////////////////////////////////////////////////////////
		// pc_sound_cache.c
		bool PLATFORM_API sound_cache_sound_request(TagGroups::s_sound_permutation* sound_perm, 
			bool block_thread, bool load, bool reference)
		{
			return Engine::SoundCacheRequestSound(sound_perm, block_thread, load, reference);
		}
		//////////////////////////////////////////////////////////////////////////
		// pc_texture_cache.c
		IDirect3DBaseTexture9** PLATFORM_API texture_cache_bitmap_get_hardware_format(TagGroups::s_bitmap_data* bitmap, 
			// block
			bool block_thread, bool load)
		{
			return Engine::TextureCacheBitmapGetHardwareFormat(bitmap, block_thread, load);
		}
		//////////////////////////////////////////////////////////////////////////
		// physical_memory_map.c
		void predicted_resources_precache(TagBlock<TagGroups::predicted_resource>& resources)
		{
			return Engine::TagGroups::PredictResources(resources);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// camera
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// director.c
		API_FUNC_NAKED void PLATFORM_API director_save_camera()
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DIRECTOR_SAVE_CAMERA);

			__asm {
				call	FUNCTION
				retn
			}
		}

		API_FUNC_NAKED void PLATFORM_API director_load_camera()
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DIRECTOR_LOAD_CAMERA);

			__asm {
				call	FUNCTION
				retn
			}
		}

		API_FUNC_NAKED int16 PLATFORM_API director_desired_perspective(const datum_index unit_index, Enums::game_perspective& desired_perspective)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DIRECTOR_DESIRED_PERSPECTIVE);

			API_FUNC_NAKED_START()
				mov		ecx, unit_index
				mov		edx, [desired_perspective]
				push	edx
				call	FUNCTION
				add		esp, 4
				pop		ebp
			API_FUNC_NAKED_END_()
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// game
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// cheats.c
		API_FUNC_NAKED void PLATFORM_API cheat_all_weapons()
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(CHEAT_ALL_WEAPONS);

			__asm {
				call	FUNCTION
				retn
			}
		}
		API_FUNC_NAKED void PLATFORM_API cheat_spawn_warthog()
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(CHEAT_SPAWN_WARTHOG);

			__asm {
				call	FUNCTION
				retn
			}
		}
		API_FUNC_NAKED void PLATFORM_API cheat_teleport_to_camera()
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(CHEAT_TELEPORT_TO_CAMERA);

			__asm {
				call	FUNCTION
				retn
			}
		}
		API_FUNC_NAKED void PLATFORM_API cheat_active_camouflage()
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(CHEAT_ACTIVE_CAMOFLAGE);

			__asm {
				call	FUNCTION
				retn
			}
		}
		API_FUNC_NAKED void PLATFORM_API cheat_active_camouflage_local_player()
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(CHEAT_ACTIVE_CAMOFLAGE_LOCAL_PLAYER);

			__asm {
				call	FUNCTION
				retn
			}
		}
		API_FUNC_NAKED datum_index PLATFORM_API cheat_local_player()
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(CHEAT_PLAYER_INDEX);

			__asm {
				call	FUNCTION
				retn
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// game_allegiance.c
		bool PLATFORM_API game_team_is_enemy(long_enum team, long_enum team_to_test)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(GAME_TEAM_IS_ENEMY);

			__asm {
				mov		ecx, team_to_test
				mov		edx, team
				call	FUNCTION
			}
		}
		//////////////////////////////////////////////////////////////////////////
		// game_engine.c
		datum_index PLATFORM_API find_closest_player_index(datum_index player_index)
		{
			return Engine::Players::FindClosestPlayerIndex(player_index);
		}


		API_FUNC_NAKED void PLATFORM_API game_engine_rasterize_message(wcstring message, real alpha)
		{
#if !PLATFORM_IS_DEDI
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(GAME_ENGINE_RASTERIZE_MESSAGE);

			API_FUNC_NAKED_START()
				push	alpha
				push	message
				call	FUNCTION
				add		esp, 4 * 2
			API_FUNC_NAKED_END_NO_STACK_POP()
#else
			API_FUNC_NAKED_START_()
			API_FUNC_NAKED_END_()
#endif
		}
		//////////////////////////////////////////////////////////////////////////
		// game_engine_multiplayer_sounds.c
		API_FUNC_NAKED void PLATFORM_API game_engine_play_multiplayer_sound(datum_index player_index, _enum multiplayer_sound_index, bool should_replicate)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(GAME_ENGINE_PLAY_MULTIPLAYER_SOUND);

			API_FUNC_NAKED_START()
				push	esi
				push	edi

				movzx	esi, should_replicate
				push	esi
				movsx	esi, multiplayer_sound_index
				mov		edi, player_index
				call	FUNCTION
				add		esp, 4 * 1

				pop		edi
				pop		esi
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		//////////////////////////////////////////////////////////////////////////
		// players.c
		API_FUNC_NAKED void PLATFORM_API player_examine_nearby_vehicle(datum_index player_index, datum_index vehicle_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(PLAYER_EXAMINE_NEARBY_VEHICLE);

			API_FUNC_NAKED_START()
				push	vehicle_index
				push	player_index
				call	FUNCTION
				add		esp, 4 * 2
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		API_FUNC_NAKED void PLATFORM_API player_set_action_result(datum_index player_index, datum_index action_object_index, int32 action_result, int32 action_seat_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(PLAYER_SET_ACTION_RESULT);

			API_FUNC_NAKED_START()
				push	ebx

				push	action_seat_index
				push	action_result
				mov		ebx, action_object_index
				mov		eax, player_index
				call	FUNCTION
				add		esp, 4 * 2

				pop		ebx
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		API_FUNC_NAKED void PLATFORM_API player_set_action_result_to_network(datum_index player_index, datum_index action_object_index, int32 action_result_type, int32 action_result, int32 action_seat_index, datum_index next_weapon_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(PLAYER_SET_ACTION_RESULT_TO_NETWORK);

			API_FUNC_NAKED_START()
				push	edi

				push	next_weapon_index
				push	action_seat_index
				push	action_result
				push	action_result_type
				mov		edi, action_object_index
				mov		ecx, player_index
				call	FUNCTION
				add		esp, 4 * 4

				pop		edi
			API_FUNC_NAKED_END_NO_STACK_POP()
		}


		datum_index PLATFORM_API player_index_from_unit_index(datum_index unit_index)
		{
			return Engine::Players::IndexFromUnitIndex(unit_index);
		}
		bool PLATFORM_API player_teleport(datum_index player_index, datum_index source_unit_index, const real_point3d& position)
		{
			return Engine::Players::Teleport(player_index, position, source_unit_index);
		}
		void PLATFORM_API player_over_shield_screen_effect(datum_index player_index)
		{
			Engine::Players::ScreenEffectOvershield(player_index);
		}
		void PLATFORM_API player_active_camo_screen_effect(datum_index player_index)
		{
			Engine::Players::ScreenEffectCamo(player_index);
		}
		void PLATFORM_API player_health_pack_screen_effect(datum_index player_index)
		{
			Engine::Players::ScreenEffectHealth(player_index);
		}

		//////////////////////////////////////////////////////////////////////////
		// player_control.c
		API_FUNC_NAKED void PLATFORM_API player_control_get_unit_camera_info(const int16 player_index, Players::s_unit_camera_info& camera_info)
		{
#if !PLATFORM_IS_DEDI
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(PLAYER_CONTROL_GET_UNIT_CAMERA_INFO);

			API_FUNC_NAKED_START()
				push	eax
				push	ecx
				push	edx
				push	esi

				mov		ax, player_index
				mov		esi, camera_info
				call	FUNCTION
				
				pop		esi
				pop		edx
				pop		ecx
				pop		eax
				pop		ebp
			API_FUNC_NAKED_END_()
#else
			API_FUNC_NAKED_START()
				pop		ebp
			API_FUNC_NAKED_END_()
#endif
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// hs
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// hs_library_external.c
		void PLATFORM_API hs_object_orient(datum_index object_index, int32 cutscene_flag_index, bool teleport, bool set_facing)
		{
			Engine::Objects::ObjectOrient(object_index, cutscene_flag_index, teleport, set_facing);
		}


		void PLATFORM_API hs_effect_new_from_object_marker(datum_index effect_definition_index, datum_index object_index, cstring marker_name)
		{
			Engine::Effects::NewOnObjectMarker(effect_definition_index, object_index, marker_name);
		}
		//////////////////////////////////////////////////////////////////////////
		// object_lists.c
		void PLATFORM_API object_list_add(datum_index object_list_index, datum_index object_index)
		{
			Engine::HS::ObjectListAdd(object_list_index, object_index);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// input
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// input_windows.c
		API_FUNC_NAKED bool PLATFORM_API input_key_is_down(_enum key_code)
		{
#if !PLATFORM_IS_DEDI
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(INPUT_KEY_IS_DOWN);

			API_FUNC_NAKED_START()
				push	ecx

				mov		cx, key_code
				call	FUNCTION

				pop		ecx
			API_FUNC_NAKED_END_NO_STACK_POP()
#else
			API_FUNC_NAKED_START_()
				xor		eax, eax
			API_FUNC_NAKED_END_()
#endif
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// interface
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// hud.c
		API_FUNC_NAKED wcstring PLATFORM_API hud_get_item_string(int16 reference_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(HUD_GET_ITEM_MESSAGE);

			API_FUNC_NAKED_START()
				movsx	edx, reference_index
				call	FUNCTION
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		//////////////////////////////////////////////////////////////////////////
		// hud_chat.c
		void PLATFORM_API hud_chat_to_network(int32 player_number, long_enum chat_type, wcstring text)
		{
		}

		API_FUNC_NAKED void PLATFORM_API hud_chat_display_message(wcstring message)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(KEYSTONE_CHAT_LOG_ADD_STRING);

			API_FUNC_NAKED_START()
				push	message
				call	FUNCTION
				add		esp, 4 * 1
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		//////////////////////////////////////////////////////////////////////////
		// hud_draw.c
		API_FUNC_NAKED void PLATFORM_API hud_draw_meter(int16 /*local_player_index*/, TagGroups::s_hud_absolute_placement* placement, TagGroups::s_hud_element_meter* element,
			byte arg_C, byte arg_10, long_flags flags, real arg_18, real arg_1C)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(HUD_DRAW_METER);

			API_FUNC_NAKED_START()
				push	esi

				push	arg_1C
				push	arg_18
				push	flags
				movzx	esi, arg_10
				push	esi
				movzx	esi, arg_C
				push	esi
				mov		esi, element
				push	placement
				call	FUNCTION
				add		esp, 4 * 6

				pop		esi
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		API_FUNC_NAKED void PLATFORM_API hud_draw_numbers(int16 local_player_index, TagGroups::s_hud_absolute_placement* placement, TagGroups::s_hud_element_number* element,
			int32 number, int16 arg_10, long_flags flags, game_ticks_t first_render_time, real arg_1C)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(HUD_DRAW_NUMBERS);

			API_FUNC_NAKED_START()
				push	esi

				push	arg_1C
				push	first_render_time
				push	flags
				movsx	esi, arg_10
				push	esi
				push	number
				push	element
				push	placement
				movsx	esi, local_player_index
				push	esi
				call	FUNCTION
				add		esp, 4 * 8

				pop		esi
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		API_FUNC_NAKED void PLATFORM_API hud_draw_static_element(int16 local_player_index, TagGroups::s_hud_absolute_placement* placement, TagGroups::s_hud_element_static* element,
			long_flags flags, game_ticks_t first_render_time)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(HUD_DRAW_STATIC_ELEMENT);

			API_FUNC_NAKED_START()
				push	esi

				push	first_render_time
				push	flags
				push	element
				push	placement
				movsx	esi, local_player_index
				push	esi
				call	FUNCTION
				add		esp, 4 * 5

				pop		esi
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		//////////////////////////////////////////////////////////////////////////
		// hud_messaging.c
		API_FUNC_NAKED void PLATFORM_API hud_print_message(int16 local_player_index, wcstring message)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(HUD_PRINT_MESSAGE);

			API_FUNC_NAKED_START()
				push	message
				movsx	eax, local_player_index
				call	FUNCTION
				add		esp, 4 * 1
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		//////////////////////////////////////////////////////////////////////////
		// ui_video_screen.c
		void PLATFORM_API ui_video_screen_add_resolution(uint32 width, uint32 height, uint32 refresh_rate)
		{
			Engine::RasterizerAddResolution(width, height, refresh_rate);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// items
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// weapons.c
		API_FUNC_NAKED bool PLATFORM_API weapon_prevents_melee_attack(const datum_index weapon_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(WEAPON_PREVENTS_MELEE_ATTACK);

			API_FUNC_NAKED_START()
				push	ecx

				mov		ecx, weapon_index
				call	FUNCTION

				pop		ecx
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED bool PLATFORM_API weapon_prevents_grenade_throwing(const datum_index weapon_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(WEAPON_PREVENTS_GRENADE_THROWING);

			API_FUNC_NAKED_START()
				push	ecx

				mov		ecx, weapon_index
				call	FUNCTION

				pop		ecx
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED void PLATFORM_API weapon_stop_reload(const datum_index weapon_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(WEAPON_STOP_RELOAD);

			_asm jmp	FUNCTION;
		}

		API_FUNC_NAKED void PLATFORM_API first_person_weapon_message_from_unit(const datum_index unit_index, const int32 weapon_message_type)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(FIRST_PERSON_WEAPON_MESSAGE_FROM_UNIT);

			API_FUNC_NAKED_START()
				push	eax
				push	ecx
				push	edx

				mov		eax, unit_index
				push	weapon_message_type
				call	FUNCTION
				add		esp, 4
				
				pop		edx
				pop		ecx
				pop		eax
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED int16 PLATFORM_API weapon_get_first_person_animation_time(const datum_index weapon_index
			, const int16 frame_type
			, Enums::first_person_weapon_animation animation
			, const int32 arg3)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(WEAPON_GET_FIRST_PERSON_ANIMATION_TIME);

			API_FUNC_NAKED_START()
				push	ecx

				xor		ecx, ecx
				movsx	cx, frame_type
				push	arg3
				push	ecx
				mov		eax, weapon_index
				movsx	cx, animation
				call	FUNCTION
				add		esp, 8
				
				pop		ecx
				pop		ebp
			API_FUNC_NAKED_END_()
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// main
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// console.c
		API_FUNC_NAKED bool PLATFORM_API console_process_command(long_flags access_flags, cstring command)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(CONSOLE_PROCESS_COMMAND);

			API_FUNC_NAKED_START()
				push	edi

				mov		edi, command
				push	access_flags
				call	FUNCTION
				add		esp, 4 * 1

				pop		edi
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		static API_FUNC_NAKED void PLATFORM_API console_printf_impl(bool clear_screen, cstring format)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(CONSOLE_PRINTF);

			API_FUNC_NAKED_START()
				push	format
				mov		al, clear_screen
				call	FUNCTION
				add		esp, 4 * 1
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		void PLATFORM_API console_printf(bool clear_screen, cstring format, ...)
		{
			char local[k_engine_function_string_buffer_size];
			memset(local, 0, k_engine_function_string_buffer_size);

			va_list args;
			va_start(args, format);
			vsprintf_s(local, format, args);
			va_end(args);

			console_printf_impl(clear_screen, local);
		}
		static API_FUNC_NAKED void PLATFORM_API console_response_printf_impl(BOOL clear_screen, cstring format)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(CONSOLE_RESPONSE_PRINTF);

			API_FUNC_NAKED_START()
				push	format
				push	clear_screen
				call	FUNCTION
				add		esp, 4 * 2
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		void PLATFORM_API console_response_printf(bool clear_screen, cstring format, ...)
		{
			char local[k_engine_function_string_buffer_size];
			memset(local, 0, k_engine_function_string_buffer_size);

			va_list args;
			va_start(args, format);
			vsprintf_s(local, format, args);
			va_end(args);

			console_response_printf_impl(clear_screen, local);
		}
		static API_FUNC_NAKED void PLATFORM_API console_warning_impl(cstring format)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(CONSOLE_WARNING);

			API_FUNC_NAKED_START()
				push	format
				call	FUNCTION
				add		esp, 4 * 1
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		void PLATFORM_API console_warning(cstring format, ...)
		{
			char local[k_engine_function_string_buffer_size];
			memset(local, 0, k_engine_function_string_buffer_size);

			va_list args;
			va_start(args, format);
			vsprintf_s(local, format, args);
			va_end(args);

			console_warning_impl(local);
		}
		bool PLATFORM_API console_process_remote_command(cstring command, int32 machine_index)
		{
			Console::TerminalGlobals()->rcon_machine_index = machine_index;
			bool result = console_process_command(0, command);
			Console::TerminalGlobals()->rcon_machine_index = NONE;

			return result;
		}
		//////////////////////////////////////////////////////////////////////////
		// main.c
		void PLATFORM_API main_menu_load()
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(MAINMENU_LOAD);

			GameState::MainGlobals()->map.main_menu_scenario_load = true;

			__asm call	FUNCTION
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// math
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// periodic_functions.c
		API_FUNC_NAKED real PLATFORM_API periodic_function_evaluate(Enums::periodic_function function_type, real input)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(PERIODIC_FUNCTION_EVALUATE);
			
			API_FUNC_NAKED_START()
				fld		input
				sub		esp, 4 * 2			// allocate space for the 'input' parameter
				fstp	qword ptr [esp]		// store the input on the stack
				movzx	eax, function_type
				call	FUNCTION
				add		esp, 4 * 2			// deallocate. double type consumes two DWORDs of stack
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		API_FUNC_NAKED real PLATFORM_API transition_function_evaluate(Enums::transition_function function_type, real input)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(TRANSITION_FUNCTION_EVALUATE);

			API_FUNC_NAKED_START()
				push	input
				movzx	ecx, function_type
				call	FUNCTION
				add		esp, 4 * 1
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// memory
	namespace blam
	{
		using namespace Yelo::Memory;

		//////////////////////////////////////////////////////////////////////////
		// data.c
		API_FUNC_NAKED s_data_array* PLATFORM_API data_new(cstring name, int32 maximum_count, size_t datum_size)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DATA_NEW);

			API_FUNC_NAKED_START()
				push	ebx

				mov		ebx, datum_size
				push	maximum_count
				push	name
				call	FUNCTION
				add		esp, 4 * 2

				pop		ebx
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		void PLATFORM_API data_dispose(s_data_array* data)
		{
			if(data != nullptr)
			{
				data_verify(data);
				CAST_PTR(Yelo::Memory::s_data_array*, GlobalFree(data));
			}
		}
		API_FUNC_NAKED void PLATFORM_API data_delete_all(s_data_array* data)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DATA_DELETE_ALL);

			API_FUNC_NAKED_START()
				push	esi

				mov		esi, data
				call	FUNCTION

				pop		esi
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		datum_index PLATFORM_API data_next_index(s_data_array* data, datum_index cursor)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DATA_NEXT_INDEX);

			if (data == nullptr || cursor.IsNull()) return datum_index::null;

			__asm {
				push	esi
				push	edi

				mov		esi, cursor
				mov		edi, data
				call	FUNCTION

				pop		edi
				pop		esi
			}
		}
/*		void* PLATFORM_API data_iterator_next(s_data_iterator& iterator)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DATA_ITERATOR_NEXT);

			API_FUNC_NAKED_START()
				push	edi

				mov		edi, iterator
				call	FUNCTION

				pop		edi
			API_FUNC_NAKED_END_NO_STACK_POP()
		}*/
		datum_index PLATFORM_API datum_new_at_index(s_data_array* data, datum_index index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DATUM_NEW_AT_INDEX);

			if (data == nullptr || index.IsNull()) return datum_index::null;

			__asm {
				mov		eax, index
				mov		edx, data
				call	FUNCTION
			}
		}
		datum_index PLATFORM_API datum_new(s_data_array* data)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DATUM_NEW);

			if (data == nullptr) return datum_index::null;

			__asm {
				mov		edx, data
				call	FUNCTION
			}
		}
		void PLATFORM_API datum_delete(s_data_array* data, datum_index index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DATUM_DELETE);

			if (data == nullptr || index.IsNull()) return;

			__asm {
				mov		edx, index
				mov		eax, data
				call	FUNCTION
			}
		}
		void* PLATFORM_API datum_try_and_get(s_data_array* data, datum_index index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(DATUM_TRY_AND_GET);

			if (data == nullptr) return nullptr;

			__asm {
				push	edx
				push	esi

				mov		edx, index
				mov		esi, data
				call	FUNCTION

				pop		esi
				pop		edx
			}
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// models
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// models.c
		API_FUNC_NAKED int16 PLATFORM_API model_find_marker(const datum_index render_model_definition_index, cstring marker_name)
		{
			static const uintptr_t CALL_ADDRESS = Engine::GET_FUNC_PTR(MODEL_FIND_MARKER);

			API_FUNC_NAKED_START()
				mov		eax, render_model_definition_index
				push	render_model_definition_index
				push	marker_name
				call	CALL_ADDRESS
				add		esp, 8

				pop		ebp
			API_FUNC_NAKED_END_()
		}

		//////////////////////////////////////////////////////////////////////////
		// model_animations.c
		int16 PLATFORM_API animation_choose_random_permutation_internal(long_enum render_or_affects_game_state, datum_index animation_graph_index, int32 animation_index)
		{
			return Engine::AnimationPickRandomPermutation(render_or_affects_game_state, animation_graph_index, animation_index);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// objects
	namespace blam
	{
		using namespace Yelo::Objects;

		//////////////////////////////////////////////////////////////////////////
		// damage.c
		bool PLATFORM_API object_restore_body(datum_index object_index)
		{
			return Engine::Objects::RestoreBody(object_index);
		}
		void PLATFORM_API object_deplete_body(datum_index object_index)
		{
			Engine::Objects::DepleteBody(object_index);
		}
		void PLATFORM_API object_deplete_shield(datum_index object_index)
		{
			Engine::Objects::DepleteShield(object_index);
		}
		void PLATFORM_API object_double_charge_shield(datum_index object_index)
		{
			Engine::Objects::DoubleChargeShield(object_index);
		}
		void PLATFORM_API area_of_effect_cause_damage(s_damage_data& data, datum_index )
		{
			// TODO
		}
		void PLATFORM_API object_cause_damage(s_damage_data& data, datum_index damaged_object_index, int16 node_index, int16 region_index, int16 damage_materials_element_index, real_vector3d* normal)
		{
			Engine::Objects::ObjectCauseDamage(data, damaged_object_index, 
				node_index, region_index, damage_materials_element_index, 
				normal);
		}
		//////////////////////////////////////////////////////////////////////////
		// index_resolution.c
		int32 PLATFORM_API index_resolution_table_translate(MessageDeltas::s_index_resolution_table& table, datum_index local_index)
		{
			// TODO: get rid of these guards
#ifndef YELO_NO_NETWORK
			return Engine::Networking::TranslateIndex(table, local_index);
#else
			return 0;
#endif
		}

		//////////////////////////////////////////////////////////////////////////
		// objects.c
		void PLATFORM_API object_reset(datum_index object_index)
		{
			Engine::Objects::Reset(object_index);
		}

		void PLATFORM_API object_set_position(datum_index object_index, 
			__in_opt real_point3d* new_position, __in_opt real_vector3d* new_forward, __in_opt real_vector3d* new_up)
		{
			Engine::Objects::SetPosition(object_index, new_position, new_forward, new_up);
		}

		void PLATFORM_API object_set_position_network(datum_index object_index, 
			real_point3d* new_position)
		{
			Engine::Objects::SetPositionNetwork(object_index, new_position);
		}

		void PLATFORM_API object_translate(datum_index object_index, 
			const real_point3d& new_position, __in_opt const s_scenario_location* new_location)
		{
			// TODO
		}

		void PLATFORM_API object_placement_data_new(s_object_placement_data& data, datum_index object_definition_index, datum_index owner_object_index)
		{
			Engine::Objects::PlacementDataNew(data, object_definition_index, owner_object_index);
		}

		datum_index PLATFORM_API object_new(s_object_placement_data& data)
		{
			return Engine::Objects::New(data);
		}

		datum_index PLATFORM_API object_new_with_role(s_object_placement_data& data, Enums::networked_datum role)
		{
			return Engine::Objects::NewWithRole(data, role);
		}

		void PLATFORM_API object_delete_to_network(datum_index object_index) { }

		void PLATFORM_API object_delete(datum_index object_index) {
			Engine::Objects::Delete(object_index);
		}

		void PLATFORM_API object_reconnect_to_map(datum_index object_index, __in_opt s_scenario_location* location_reference)
		{
			Engine::Objects::ReconnectToMap(object_index, location_reference);
		}

		void PLATFORM_API object_disconnect_from_map(datum_index object_index)
		{
			Engine::Objects::DisconnectFromMap(object_index);
		}

		API_FUNC_NAKED int16 PLATFORM_API object_get_marker_by_name(const datum_index object_index
			, cstring marker_name
			, s_object_marker* markers
			, const int16 maximum_marker_count)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(OBJECT_GET_MARKER_BY_NAME);

			_asm jmp	FUNCTION;
		}

		void PLATFORM_API object_attach_to_marker(datum_index target_object_index, cstring target_marker_name, datum_index object_index, cstring marker_name)
		{
			Engine::Objects::Attach(target_object_index, target_marker_name, object_index, marker_name);
		}

		void PLATFORM_API object_detach(datum_index object_index)
		{
			Engine::Objects::Detach(object_index);
		}

		real_point3d& PLATFORM_API object_get_origin(datum_index object_index, __out real_point3d& return_origin)
		{
			return Engine::Objects::GetOrigin(object_index, return_origin);
		}

		void PLATFORM_API object_get_orientation(datum_index object_index, 
			__out_opt real_vector3d* return_forward, __out_opt real_vector3d* return_up)
		{
			Engine::Objects::GetOrientation(object_index, return_forward, return_up);
		}

		s_scenario_location& PLATFORM_API object_get_location(datum_index object_index, __out s_scenario_location& return_location)
		{
			return Engine::Objects::GetLocation(object_index, return_location);
		}

		void PLATFORM_API object_start_interpolation(datum_index object_index, int32 interpolation_frame_count)
		{
			Engine::Objects::StartInterpolation(object_index, interpolation_frame_count);
		}

		s_object_data* PLATFORM_API object_iterator_next(s_object_iterator& iter)
		{
			return Engine::Objects::IteratorNext(iter);
		}

		int16 PLATFORM_API objects_in_sphere(Flags::objects_find_flags find_flags, long_flags object_type_flags, 
			const s_scenario_location& location, const real_point3d& center, real radius, 
			datum_index object_indices[], int16 maximum_object_indices)
		{
			return Engine::Objects::FindInSphere(find_flags, object_type_flags,
				location, center, radius, object_indices, maximum_object_indices);
		}

		void PLATFORM_API object_definition_predict(datum_index object_index)
		{
			Engine::Objects::DefinitionPredict(object_index);
		}

		bool PLATFORM_API object_header_block_allocate(datum_index object_index, size_t block_reference_offset, size_t size)
		{
			return Engine::Objects::HeaderBlockAllocate(object_index, block_reference_offset, size);
		}

		void PLATFORM_API objects_scripting_set_scale(datum_index object_index, real scale, int32 ticks)
		{
			Engine::Objects::SetScale(object_index, scale, ticks);
		}

		real PLATFORM_API object_get_level_of_detail_pixels(datum_index object_index)
		{
			return Engine::Objects::GetLevelOfDetailPixels(object_index);
		}

		void PLATFORM_API object_render_state_refresh(datum_index object_render_state_index, datum_index object_index, real level_of_detail_pixels, byte arg4)
		{
			Engine::Objects::ObjectRenderStateRefresh(object_render_state_index, object_index, level_of_detail_pixels, arg4);
		}

		API_FUNC_NAKED void PLATFORM_API objects_update()
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(OBJECTS_UPDATE);

			_asm jmp	FUNCTION;
		}

		API_FUNC_NAKED void PLATFORM_API object_update(datum_index object_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(OBJECT_UPDATE);

			_asm jmp	FUNCTION;
		}
		
		API_FUNC_NAKED void PLATFORM_API object_destroy(const datum_index object_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(OBJECT_DESTROY);

			API_FUNC_NAKED_START()
				push	eax
				push	ecx
				push	edx
				push	edi

				mov		eax, object_index
				call	FUNCTION

				pop		edi
				pop		edx
				pop		ecx
				pop		eax
				pop		ebp
			API_FUNC_NAKED_END_()
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// physics
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// collisions.c
		API_FUNC_NAKED bool PLATFORM_API collision_test_vector(long_flags flags, real_point3d& location, real_vector3d& vector, 
			datum_index object_index, Physics::s_collision_result& collision)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(COLLISION_TEST_VECTOR);

			__asm jmp	FUNCTION;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// rasterizer
	namespace blam {
		//////////////////////////////////////////////////////////////////////////
		// dx9/rasterizer_dx9.c
		bool PLATFORM_API rasterizer_set_texture_bitmap_data(_enum stage, TagGroups::s_bitmap_data* bitmap) {
			return Engine::SetTextureSamplerStage(bitmap, stage);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// render
	namespace blam {
		static const void * jmpAddrs=CAST_PTR(void*, 0x50F998);
		static const void * unk_call=CAST_PTR(void*, 0x556C00);
		static const void * unk_call_b=CAST_PTR(void*, 0x457970);
		static const void * unk_call_c=CAST_PTR(void*, 0x51AE70);
		static const void * render_related = CAST_PTR(void*, 0x75E024); 
		//static const DWORD * something_in_render_clusters = (DWORD *) 0x75E464;
#ifndef local_const
#define arg_var_location esp+2A8h
#define _plyr_idx arg_var_location + 4
#define _ren_win_obj_global 75E464h
#define LOAD_MEM_TO_REGISTER(_register) mov _register, dword ptr  
#define SAVE_TO_MEM mov dword ptr ds:
#define SAVE_TO_MEM_W mov word ptr ds:
#define local_const
#endif
		API_FUNC_NAKED void __cdecl render_window(const uint16 local_player_index
			, void* render_camera
			, void* render_frustum
			, void* rasterizer_camera
			, void* rasterizer_frustum
			, void* rasterizer_target
			, const bool is_mirror)
		{
			//uint32 player_index = ((int)&local_player_index - 4);
			//*something_in_render_clusters;
				
		//save local_player_index in local register
			__asm {
					mov         ebp, [_plyr_idx] //local_player_index
					push esi
					LOAD_MEM_TO_REGISTER(esi) ds:[_ren_win_obj_global]
					push edi
					inc         esi
					SAVE_TO_MEM[_ren_win_obj_global], esi
					LOAD_MEM_TO_REGISTER(esi) [esp + 2B8h]
					xor         eax, eax
					SAVE_TO_MEM_W[_ren_win_obj_global + 4h], bp
					mov         ecx, 96h
					lea         edi, [esp + 58h]
					rep stos	dword ptr es : [edi]
					mov         ax, word ptr[esp + 2C8h]
					mov         ecx, 15h
					mov         edi, 75E474h
					rep movs	dword ptr es : [edi], dword ptr[esi]
					mov         esi, dword ptr[esp + 2BCh]
					mov         ecx, 63h
					mov         edi, 75E4C8h
					rep movs   dword ptr es : [edi], dword ptr[esi]
					mov         esi, dword ptr[esp + 2C0h]
					mov         dx, word ptr [75E46Ah]
					mov         ecx, 15h
					lea         edi, [esp + 60h]
					rep movs   dword ptr es : [edi], dword ptr[esi]
					mov         esi, dword ptr[esp + 2C4h]
					mov         ecx, 63h
					lea         edi, [esp + 0B4h]
					rep movs   dword ptr es : [edi], dword ptr[esi]
					mov         cl, byte ptr[esp + 2CCh]
					mov         byte ptr[esp + 5Ch], cl
					mov         word ptr[esp + 58h], ax
					mov         word ptr[esp + 5Ah], dx
					mov         ecx, 14h
					mov         esi, 75E654h
					lea         edi, [esp + 240h]
					rep movs   dword ptr es : [edi], dword ptr[esi]
					call        unk_call
					lea         ebx, [esp + 290h]
					mov         ecx, ebp
					call        unk_call_b
					lea         eax, [esp + 58h]
					push        eax
					call        unk_call_c
					add         esp, 4
					mov         al, 1	
					jmp			jmpAddrs
				}			
		}



			/*
#pragma region
		NAKED_FUNC_WRITER_ASM_BEGIN(render_window_chunk_B)
					//call        ds : [0x4978A0]
			//call        ds : [0x514C80]
			//call        Yelo::Memory::t_function_process_block<0x5326016, &Yelo::Memory::function_preprocess_render_sky_definition, 1, &Yelo::Memory::function_postprocess_render_sky_definition, 1, 0>::FunctionHook(51603520h)
			//call        Yelo::Objects::Weapon::ApplyWeaponPreset(51617400h)
			//call        ds:[0x4F4600]
			//call        Yelo::Memory::t_function_process_block<5317184h, &Yelo::Memory::function_preprocess_render_objects_definition, 1, &Yelo::Memory::function_postprocess_render_objects_definition, 1, 0>::FunctionHook(51603500h)
			//call        ds:[0x555C30]
			//call        Yelo::Memory::t_function_process_block<5594416h, &Yelo::Memory::function_preprocess_render_structure_definition, 1, &Yelo::Memory::function_postprocess_render_structure_definition, 1, 0>::FunctionHook(51603540h)
			//call        ds:[0x517430]
			//call		  ds:[0x51C900]
					mov         al, byte ptr ds : [00637CCCh]
					xor         ebx, ebx
					cmp         al, bl
					je          a_loc_50F9F8
					mov         eax, 0FFFFFFFFh
					mov         dword ptr[esp + 10h], eax
					mov         dword ptr[esp + 54h], eax
					lea         eax, [esp + 10h]
					mov         dword ptr[esp + 14h], ebx
					mov         byte ptr[esp + 18h], 1
					mov         byte ptr[esp + 19h], 1
					mov         dword ptr[esp + 50h], ebx
					call        ds:[0x512480]
a_loc_50F9F8:
					call        ds:[0x4F5090]
					cmp         word ptr ds : [6249C4h], bx
					jne         a_loc_50FA4D
					cmp         byte ptr ds : [6249D5h], bl
					je          a_loc_50FA4D
					mov         edi, 2
					call        ds:[0x51DF30]
					xor         esi, esi
					cmp         word ptr ds : [76B6F0h], bx
					jle         a_loc_50FA48
a_loop_unk:
					movsx       ecx, si
					imul        ecx, ecx, 1A0h
					xor         edx, edx
					mov         dx, word ptr[ecx + 75E6F0h]
					push        edx
					call        ds:[0x51E170]
					add         esp, 4
					inc         esi
					cmp         si, word ptr ds : [76B6F0h]
					jl          a_loop_unk
a_loc_50FA48:
					call        ds:[0x51E7E0]
a_loc_50FA4D
					mov         edi, 3
					call        ds:[0x51DF30]
					xor         esi, esi
					cmp         word ptr ds : [76B6F0h], bx
					jle         a_loc_50FA87
					movsx       eax, si
					imul        eax, eax, 1A0h
					xor         ecx, ecx
					mov         cx, word ptr[eax + 75E6F0h]
					push        ecx
					call        ds:[0x51E170]
					add         esp, 4
					inc         esi
					cmp         si, word ptr ds : [76B6F0h]
					jl          0x50FA62
a_loc_50FA87:
					call        ds:[0x51E7E0]
					cmp         byte ptr ds : [67EA98h], bl
					je          0x50FABF
					call        ds:[5215A0]
					mov         edx, dword ptr ds : [67EA9Ch]
					mov         eax, dword ptr ds : [007EB6F4h]
					push        ebx
					push        ebx
					push        515800h
					push        ebx
					push        edx
					mov         ecx, 518793A0h
					call        ds : [K_STRUCTURE_RENDER_PASS_FUNC]
					add         esp, 14h
					call        ds:[0x522040]
					cmp         word ptr ds : [6249C4h], bx
					jne         rast_shader_jmp
					cmp         byte ptr ds : [6249D5h], bl
					je          rast_shader_jmp;
					xor         edi, edi
					call        ds : [0x51DF30]
					xor         esi, esi
					cmp         word ptr ds : [76B6F0h], bx
					jle         0x50FB15
					lea         ebx, [ebx]
					movsx       eax, si
					imul        eax, eax, 1A0h
					xor         ecx, ecx
					mov         cx, word ptr[eax + 75E6F0h]
					push        ecx
					call        0x51E170
					add         esp, 4
					inc         esi
					cmp         si, word ptr ds : [76B6F0h]
					jl          0x50FAF0
					call        0x51E7E0
					mov         edi, 1
					call        0x51DF30
					xor         esi, esi
					cmp         word ptr ds : [76B6F0h], bx
					jle         0x50FB55
					nop
a_unk_50FB30:
					movsx       edx, si
					imul        edx, edx, 1A0h
					xor         eax, eax
					mov         ax, word ptr[edx + 75E6F0h]
					push        eax
					call        ds:[0x51E170]
					add         esp, 4
					inc         esi
					cmp         si, word ptr ds : [76B6F0h]
					jl          a_unk_50FB30
					mov         eax, dword ptr ds : [0x6B840Ch]
					mov         ecx, dword ptr[eax]
					push        ebx
					push        1Ch
					push        eax
					call        dword ptr[ecx + 0E4h]
					test        dword ptr ds : [75C444h], 4000000h
					je          a_loc_50FB86
					mov         eax, dword ptr ds : [006B840Ch]
					mov         edx, dword ptr[eax]
					push        ebx
					push        0C3h
					push        eax
					call        dword ptr[edx + 0E4h]
a_loc_50FB86:
					test        dword ptr ds : [75C444h], 2000000h
					je          a_loc_50FBA6
					mov         eax, dword ptr ds : [006B840Ch]
					mov         ecx, dword ptr[eax]
					push        ebx
					push        0AFh
					push        eax
					call        dword ptr[ecx + 0E4h]
a_loc_50FBA6:
					cmp         word ptr ds : [67480Ch], 3
					jne         rast_shader_jmp //jum straight to yelo::rast:shaderexten
					mov         eax, 2
					call        ds:[0x51C900]
rast_shader_jmp:
					call        Yelo::Rasterizer::ShaderExtension::Environment::StructureRenderSpecularDirectionalLightmaps(5164F1F0h)
					cmp         byte ptr ds : [67EA98h], bl
					je         a_keyword
					mov         edx, dword ptr ds : [6E225Ch]
					cmp         dword ptr[edx + 0Ch], 0FFFFFFFFh
					mov         esi, dword ptr ds : [637D34h]
					jne         a_unk_label
					cmp         si, bx
					jne         a_unk_label
					mov         word ptr ds : [637D34h], 1
a_unk_label:
					call        ds:[0x525020]
					mov         eax, dword ptr ds : [0067EA9Ch]
					push        ebx
					push        0x44B900h
					push        0x515890h
					push        0x515840h
					push        eax
					mov         eax, dword ptr ds : [007EB6F4h]
					mov         ecx, 518793A0h
					call        ds : [K_STRUCTURE_RENDER_PASS_FUNC]
					mov         al, byte ptr ds : [0067EA98h]
					add         esp, 14h
					cmp         al, bl
					mov         word ptr ds : [637D34h], si
					je          a_keyword
					call        ds:[0x523BD0]
					mov         ecx, dword ptr ds : [67EA9Ch]
					mov         eax, dword ptr ds : [007EB6F4h]
					push        ebx
					push        44B900h
					push        5158D0h
					push        5158C0h
					push        ecx
					mov         ecx, 518793A0h
					call        ds : [K_STRUCTURE_RENDER_PASS_FUNC]
					mov         al, byte ptr ds : [0067EA98h]
					add         esp, 14h
					cmp         al, bl
					mov         dword ptr ds : [6B8468h], ebx
					je         a_keyword
					call       ds:[0x523FB0]
					mov         edx, dword ptr ds : [67EA9Ch]
					mov         eax, dword ptr ds : [007EB6F4h]
					push        ebx
					push        ebx
					push        5158F0h
					push        ebx
					push        edx
					mov         ecx, 518793A0h
					call        ds:[K_STRUCTURE_RENDER_PASS_FUNC]
					mov         al, byte ptr ds : [0067EA98h]
					add         esp, 14h
					cmp         al, bl
					je         a_keyword
					call        ds:[0x5234C0]
					mov         eax, dword ptr ds : [0067EA9Ch]
					push        ebx
					push        ebx
					push        515920h
					push        ebx
					push        eax
					mov         eax, dword ptr ds : [007EB6F4h]
					mov         ecx, 518793A0h
					call        ds : [K_STRUCTURE_RENDER_PASS_FUNC]
					mov         al, byte ptr ds : [0067EA98h]
					add         esp, 14h
					cmp         al, bl
					je          a_keyword
					mov         ecx, dword ptr ds : [67EA9Ch]
					mov         eax, dword ptr ds : [007EB6F4h]
					push        515930h
					push        ebx
					push        ebx
					push        ebx
					push        ecx
					mov         ecx, 518793A0h
					mov         dword ptr ds : [67CD18h], ebx
					mov         byte ptr ds : [6B8504h], bl
					call        ds : [K_STRUCTURE_RENDER_PASS_FUNC]
					mov         al, byte ptr ds : [0067EA98h]
					add         esp, 14h
					cmp         al, bl
					je          a_keyword
					call        ds:[0x5220F0]
					mov         edx, dword ptr ds : [67EA9Ch]
					mov         eax, dword ptr ds : [007EB6F4h]
					push        ebx
					push        ebx
					push        515970h
					push        ebx
					push        edx
					mov         ecx, 518793A0h
					call        ds:[K_STRUCTURE_RENDER_PASS_FUNC]
					add         esp, 14h
a_keyword:					
					mov         eax, dword ptr ds : [0068CD24h]
					cmp         eax, ebx
					je          a_render_weather_particle_systems
					mov         eax, dword ptr[eax + 34h]
					cmp         eax, ebx
					je          a_render_weather_particle_systems
					call        eax
a_render_weather_particle_systems:
					call        Yelo::Memory::t_function_process_block<4559872, &Yelo::Memory::function_preprocess_weather_particle_systems_render_definition, 1, 0, 0, 0>::FunctionHook(51603460h)
					call        0x513600
					call        0x4554B0
					push        0FFFFFFF3h
					call        0x511830
					add         esp, 4
					push        1
					call        0x518D40
					add         esp, 4
					mov         edi, 4
					call        0x51DF30
					xor         esi, esi
					cmp         word ptr ds : [76B6F0h], bx
					jle         a_loc_50FD99
					movsx       eax, si
					imul        eax, eax, 1A0h
					xor         ecx, ecx
					mov         cx, word ptr[eax + 75E6F0h]
					push        ecx
					call        0x51E170
					add         esp, 4
					inc         esi
					cmp         si, word ptr ds : [76B6F0h]
					jl          0x50FD74
a_loc_50FD99:
					mov         eax, dword ptr ds : [006B840Ch]
					mov         edx, dword ptr[eax]
					push        ebx
					push        1Ch
					push        eax
					call        dword ptr[edx + 0E4h]
					test        dword ptr ds : [75C444h], 4000000h
					je          0x50FDCA
					mov         eax, dword ptr ds : [006B840Ch]
					mov         ecx, dword ptr[eax]
					push        ebx
					push        0C3h
					push        eax
					call        dword ptr[ecx + 0E4h]
a_loc_50FDCA:
					test        dword ptr ds : [75C444h], 2000000h
					je          a_loc_50FDEA
					mov         eax, dword ptr ds : [006B840Ch]
					mov         edx, dword ptr[eax]
					push        ebx
					push        0AFh
					push        eax
					call        dword ptr[edx + 0E4h]
a_loc_50FDEA:
					cmp         word ptr ds : [67480Ch], 3
					jne         0x50FDFE
					mov         eax, 2
					call        ds:[0x51C900]
a_loc_50FDFE:
					call        ds:[0x555710]
					push        ebx
					jmp         &Yelo::DX9::c_gbuffer_system::Hook_RenderObjectsTransparent //5175A0
					add         esp, 4
					xor         eax, eax
					call        0x51C900
					cmp         byte ptr ds : [67EA98h], bl
					je          0x50FE5C
					mov         eax, dword ptr ds : [0067EA9Ch]
					push        ebx
					push        ebx
					push        44B900h
					push        ebx
					push        eax
					mov         eax, dword ptr ds : [007EB6F4h]
					mov         ecx, 518793A0h
					call        ds : [K_STRUCTURE_RENDER_PASS_FUNC]
					mov         ecx, dword ptr ds : [67EA9Ch]
					mov         eax, dword ptr ds : [007EB6F4h]
					add         esp, 14h
					push        ebx
					push        ebx
					push        44B900h
					push        ebx
					push        ecx
					mov         ecx, 518793A0h
					call        ds : [K_STRUCTURE_RENDER_PASS_FUNC]
					add         esp, 14h
_a_loc_50FE5C:
					call        ds:[0x5175A0]
					j
					call        Yelo::Memory::t_function_process_block<4814064, &Yelo::Memory::function_preprocess_render_hud_definition, 3, &Yelo::Memory::function_postprocess_render_hud_definition, 2, 0>::FunctionHook(51603480h)
					call        0x531E20
					mov         eax, ebp
					call        Yelo::Memory::t_function_process_block<4830288, &Yelo::Memory::function_preprocess_render_ui_definition, 1, &Yelo::Memory::function_postprocess_render_ui_definition, 2, 0>::FunctionHook(516034E0h)
					cmp         word ptr ds : [75C582h], 0FFFFh
					jne         a_loc_50FE8F_exit
					push        ebx
					mov         ebx, 75E400h
					call        ds:[0x515DC0]
					add         esp, 4
					call        ds:[0x516710]
a_loc_50FE8F_exit:
					pop         edi
					pop         esi
					pop         ebp
					pop         ebx
					add         esp, 2A0h
					ret
		NAKED_FUNC_WRITER_ASM_END()
#pragma endregion
*/
	};
	//////////////////////////////////////////////////////////////////////////
	// scenario
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// scenario.c
		// After calling this function you should call Yelo::GameState::InitializeForNewBSP
		API_FUNC_NAKED bool PLATFORM_API scenario_switch_structure_bsp(int16 bsp_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(SCENARIO_SWITCH_STRUCTURE_BSP);

			API_FUNC_NAKED_START()
				push	esi

				movsx	esi, bsp_index
				call	FUNCTION

				pop		esi
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
		bool PLATFORM_API scenario_trigger_volume_test_point(int32 trigger_volume_index, const real_point3d& point)
		{
			return Engine::Scenario::TriggerVolumeTestPoint(trigger_volume_index, point);
		}
		bool PLATFORM_API scenario_trigger_volume_test_object(int32 trigger_volume_index, datum_index object_index)
		{
			return Engine::Scenario::TriggerVolumeTestObject(trigger_volume_index, object_index);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// shell
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// shell_windows.c
		API_FUNC_NAKED bool PLATFORM_API shell_get_command_line_argument(cstring param, _Out_opt_ cstring* value)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(SHELL_GET_COMMAND_LINE_ARGUMENT);

			API_FUNC_NAKED_START()
				push	edi

				mov		edi, value
				push	param
				call	FUNCTION
				add		esp, 4 * 1

				pop		edi
			API_FUNC_NAKED_END_NO_STACK_POP()
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// tag_files
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// tag_groups.c
		datum_index PLATFORM_API tag_loaded(tag group_tag, cstring name)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(TAG_LOADED);

#if defined(ENGINE_FUNCTIONS_USE_LOCAL)
			char local[256];
			memset(local, 0, sizeof(local));
			strcpy_s(local, name);
#else
			cstring local = name;
#endif

			__asm {
				push	edi

#if defined(ENGINE_FUNCTIONS_USE_LOCAL)
				lea		eax, local
#else
				mov		eax, local
#endif
				push	eax
				mov		edi, group_tag
				call	FUNCTION
				add		esp, 4 * 1

				pop		edi
			}
		}
		void PLATFORM_API tag_iterator_new(TagGroups::s_tag_iterator& iter, const tag group_tag_filter)
		{
			memset(&iter, 0, sizeof(iter));
			iter.group_tag_filter = group_tag_filter;
		}
		datum_index PLATFORM_API tag_iterator_next(TagGroups::s_tag_iterator& iter)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(TAG_ITERATOR_NEXT);

			__asm {
				push	esi

				mov		esi, iter
				call	FUNCTION

				pop		esi
			}
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// text
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// text_group.c
		wcstring unicode_string_list_get_string(datum_index unicode_string_list_definition_index, int32 reference_index)
		{
			return Engine::TagGroups::UnicodeStringListGetString(unicode_string_list_definition_index, reference_index);
		}
	};
	//////////////////////////////////////////////////////////////////////////
	// units
	namespace blam
	{
		//////////////////////////////////////////////////////////////////////////
		// units.c
		API_FUNC_NAKED void PLATFORM_API unit_update(const datum_index unit_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_UPDATE);

			_asm jmp	FUNCTION;
		}

		void PLATFORM_API unit_set_animation(datum_index unit_index
			, datum_index animation_graph_index
			, int32 animation_index)
		{
			Engine::Objects::UnitSetAnimation(unit_index, animation_graph_index, animation_index);
		}
		
		API_FUNC_NAKED void PLATFORM_API unit_animation_start_action(const datum_index unit_index, const Enums::unit_replacement_animation_state action_type)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_ANIMATION_START_ACTION);

			_asm jmp	FUNCTION;
		}

		int16 PLATFORM_API unit_find_nearby_seat(datum_index unit_index
			, datum_index target_unit_index
			, __out int16& parent_seat_index)
		{
			return Engine::Objects::UnitFindNearbySeat(unit_index, target_unit_index, parent_seat_index);
		}

		bool PLATFORM_API unit_can_enter_seat(datum_index unit_index
			, datum_index target_unit_index
			, int16 seat_index
			, __out datum_index* unit_in_seat_index)
		{
			return Engine::Objects::UnitCanEnterSeat(unit_index, target_unit_index, seat_index, unit_in_seat_index);
		}

		bool PLATFORM_API unit_enter_seat(datum_index unit_index
			, datum_index target_unit_index
			, int32 seat_index)
		{
			return Engine::Objects::UnitEnterSeat(unit_index, target_unit_index, seat_index);
		}

		void PLATFORM_API unit_get_camera_position(datum_index unit_index, __out real_point3d* return_position)
		{
			Engine::Objects::UnitGetCameraPosition(unit_index, return_position);
		}

		void PLATFORM_API unit_kill(const datum_index unit_index)
		{
			auto* unit_datum = blam::object_try_and_get_and_verify_type<s_unit_datum>(unit_index);
			if(unit_datum)
			{
				SET_FLAG(unit_datum->object.damage.flags, Flags::_object_killed_bit, true);
			}
		}

		void PLATFORM_API unit_open(datum_index unit_index)
		{
			Engine::Objects::UnitOpen(unit_index);
		}

		void PLATFORM_API unit_close(datum_index unit_index)
		{
			Engine::Objects::UnitClose(unit_index);
		}

		void PLATFORM_API unit_scripting_exit_vehicle(datum_index unit_index)
		{
			Engine::Objects::UnitExitVehicle(unit_index);
		}

		API_FUNC_NAKED bool PLATFORM_API unit_try_and_exit_seat(const datum_index unit_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_TRY_AND_EXIT_SEAT);

			API_FUNC_NAKED_START()
				push	ecx
				push	edx
				push	edi

				mov		edi, unit_index
				call	FUNCTION
				
				pop		edi
				pop		edx
				pop		ecx
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED void PLATFORM_API unit_detach_from_parent(const datum_index unit_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_DETACH_FROM_PARENT);

			API_FUNC_NAKED_START()
				push	edi

				mov		edi, unit_index
				call	FUNCTION

				pop		edi
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		void PLATFORM_API unit_exit_seat_end(datum_index unit_index
			, bool no_network_message
			, bool can_run_on_client_side
			, bool unknown)
		{
			Engine::Objects::UnitExitSeatEnd(unit_index, no_network_message, can_run_on_client_side, unknown);
		}
		
		API_FUNC_NAKED bool PLATFORM_API unit_can_see_point(const datum_index unit_index
			, const real_point3d* point
			, const real view_radians)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_CAN_SEE_POINT);
			
			API_FUNC_NAKED_START()
				push	ecx
				push	edx
				push	edi

				mov		ecx, unit_index
				mov		edi, point
				push	view_radians
				call	FUNCTION
				add		esp, 4

				pop		edi
				pop		edx
				pop		ecx
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		int16 unit_get_custom_animation_time(datum_index unit_index)
		{
			return Engine::Objects::UnitGetCustomAnimationTime(unit_index);
		}
		
		API_FUNC_NAKED bool PLATFORM_API unit_start_user_animation(const datum_index unit_index
			, const datum_index animation_definition_index
			, cstring animation_name
			, const bool interpolate)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_START_USER_ANIMATION);

			API_FUNC_NAKED_START()
				push	ebx
				push	ecx
				push	edx
				push	edi

				mov		eax, animation_name
				mov		edi, animation_definition_index
				xor		ebx, ebx
				mov		bl, interpolate
				push	ebx
				push	unit_index
				call	FUNCTION
				add		esp, 8

				pop		edi
				pop		edx
				pop		ecx
				pop		ebx
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED void PLATFORM_API unit_animation_set_state(const datum_index unit_index, const Enums::unit_animation_state state)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_ANIMATION_SET_STATE);

			_asm jmp	FUNCTION;
		}

		API_FUNC_NAKED void PLATFORM_API unit_drop_current_weapon(const datum_index unit_index, const bool force)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_DROP_CURRENT_WEAPON);

			_asm jmp	FUNCTION;
		}

		API_FUNC_NAKED datum_index PLATFORM_API unit_inventory_get_weapon(const datum_index unit_index, const int16 index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_INVENTORY_GET_WEAPON);
			
			API_FUNC_NAKED_START()
				push	ecx
				push	edx

				mov		eax, unit_index
				xor		ecx, ecx
				mov		cx, index
				call	FUNCTION
				
				pop		edx
				pop		ecx
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED void PLATFORM_API unit_ready_desired_weapon(const datum_index unit_index, const bool force)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_READY_DESIRED_WEAPON);

			_asm jmp	FUNCTION;
		}

		API_FUNC_NAKED void PLATFORM_API unit_throw_grenade_release(const datum_index unit_index, const sbyte keyframe)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_THROW_GRENADE_RELEASE);

			_asm jmp	FUNCTION;
		}
		
		API_FUNC_NAKED void PLATFORM_API unit_cause_player_melee_damage(const datum_index unit_index)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_CAUSE_PLAYER_MELEE_DAMAGE);

			_asm jmp	FUNCTION;
		}

		API_FUNC_NAKED void PLATFORM_API unit_damage_aftermath(const datum_index unit_index
			, const Objects::s_damage_data* damage_data
			, const _enum damage_flags
			, const real shield_amount
			, const real body_amount
			, void* arg6
			, const int32 damage_part)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_DAMAGE_AFTERMATH);

			_asm jmp	FUNCTION;
		}

		API_FUNC_NAKED void PLATFORM_API unit_scripting_set_current_vitality(const datum_index unit_index
			, const real health
			, const real shield)
		{
			static const uintptr_t CALL_ADDRESS = Engine::GET_FUNC_PTR(UNIT_SCRIPTING_SET_CURRENT_VITALITY);

			API_FUNC_NAKED_START()
				push	eax

				mov		eax, unit_index
				push	shield
				push	health
				call	CALL_ADDRESS
				add		esp, 8

				pop		eax
				pop		ebp
			API_FUNC_NAKED_END_()
		}

		API_FUNC_NAKED void PLATFORM_API unit_set_actively_controlled(const datum_index unit_index, const bool controlled)
		{
			static const uintptr_t FUNCTION = Engine::GET_FUNC_PTR(UNIT_SET_ACTIVELY_CONTROLLED);

			API_FUNC_NAKED_START()
				push	ecx

				xor		ecx, ecx
				mov		cl, controlled
				push	unit_index
				call	FUNCTION
				add		esp, 4

				pop		ecx
				pop		ebp
			API_FUNC_NAKED_END_()
		}
	};
};