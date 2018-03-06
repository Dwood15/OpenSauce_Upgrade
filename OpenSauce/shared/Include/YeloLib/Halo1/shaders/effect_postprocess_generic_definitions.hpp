/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

#include <YeloLib/tag_files/tag_groups_base_yelo.hpp>
#include <YeloLib/Halo1/shaders/shader_postprocess_definitions.hpp>

namespace Yelo
{
	namespace TagGroups
	{
		struct s_effect_postprocess_effect_activation_control
		{
			TAG_FIELD(_enum, state);
			struct _flags {
				TAG_FLAG16(invert);
			}flags;
		}; static_assert(sizeof(s_effect_postprocess_effect_activation_control) == 0x4, STATIC_ASSERT_FAIL);

#if 0
		struct s_effect_postprocess_effect_fade_control
		{
			TAG_FIELD(_enum, value);
			struct _flags {
				TAG_FLAG16(invert);
			}flags;

			TAG_FIELD(real_bounds, value_map);
			PAD32;
		}; static_assert( sizeof(s_effect_postprocess_effect_fade_control) == 0x10, STATIC_ASSERT_FAIL);
#endif

		struct s_effect_postprocess_generic_effect_instance
		{
			TAG_FIELD(tag_string, name);
			TAG_FIELD(int16, effect_index);
			TAG_ENUM(render_stage, Enums::postprocess_render_stage);
			TAG_FIELD(s_effect_postprocess_quad_definition, quad_definition);
			TAG_FIELD(_enum, activation_operation);
			PAD16;
			PAD32;
			TAG_TBLOCK_(activation_controls, s_effect_postprocess_effect_activation_control);
#if 0
			TAG_TBLOCK_(fade_controls, s_effect_postprocess_effect_fade_control);
#else
			TAG_PAD(tag_block, 1);
#endif
			TAG_PAD(byte, 16);
		}; static_assert(sizeof(s_effect_postprocess_generic_effect_instance) == 0x74, STATIC_ASSERT_FAIL);

		struct s_effect_postprocess_generic_exposed_parameter {
			TAG_FIELD(tag_string, exposed_name);
			TAG_FIELD(tag_string, parameter_name);
			TAG_FIELD(int32, shader_index);
			TAG_FIELD(shader_variable_type, value_type);

			struct {
				void* parameter_instance; //void* to avoid a problematic include
			}runtime;

			PAD32;
		}; static_assert(sizeof(s_effect_postprocess_generic_exposed_parameter) == 0x50, STATIC_ASSERT_FAIL);

		struct s_effect_postprocess_generic : s_effect_postprocess_definition {
			enum { k_group_tag = 'efpg' };

			TAG_TBLOCK_(shaders, tag_reference);
			TAG_TBLOCK_(shader_indices, int32);
			TAG_TBLOCK_(exposed_parameters, s_effect_postprocess_generic_exposed_parameter);

		}; static_assert(sizeof(s_effect_postprocess_generic) == 0x24 + sizeof(s_effect_postprocess_definition), STATIC_ASSERT_FAIL);
	};
};