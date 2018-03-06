/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

#include <YeloLib/tag_files/tag_groups_base_yelo.hpp>

namespace Yelo
{
	namespace Flags
	{
		enum collision_surface_flags : byte_flags
		{
			_collision_surface_two_sided_bit,
			_collision_surface_invisible_bit,
			_collision_surface_climbable_bit,
			_collision_surface_breakable_bit,
		};
	};

	namespace TagGroups
	{
		struct bsp3d_node
		{
			TAG_FIELD(int32, plane);
			TAG_FIELD(int32, back_child);
			TAG_FIELD(int32, front_child);
		}; static_assert(sizeof(bsp3d_node) == 0xC, STATIC_ASSERT_FAIL);

		struct collision_leaf
		{
			TAG_FIELD(word_flags, flags);
			TAG_FIELD(int16, reference_count);
			TAG_FIELD(int32, first_reference, bsp2d_reference);
		}; static_assert(sizeof(collision_leaf) == 0x8, STATIC_ASSERT_FAIL);

		struct bsp2d_reference
		{
			TAG_FIELD(int32, plane);
			TAG_FIELD(int32, bsp2d_node);
		}; static_assert(sizeof(bsp2d_reference) == 0x8, STATIC_ASSERT_FAIL);

		struct bsp2d_node
		{
			TAG_FIELD(real_plane2d, plane);
			TAG_FIELD(int32, left_child);
			TAG_FIELD(int32, right_child);
		}; static_assert(sizeof(bsp2d_node) == 0x14, STATIC_ASSERT_FAIL);

		struct collision_surface
		{
			TAG_FIELD(int32, plane);
			TAG_FIELD(int32, first_edge);
			TAG_FIELD(Flags::collision_surface_flags, flags);
			TAG_FIELD(sbyte, breakable_surface);
			TAG_FIELD(int16, material);
		}; static_assert(sizeof(collision_surface) == 0xC, STATIC_ASSERT_FAIL);

		struct collision_edge
		{
			TAG_FIELD(int32, start_vertex);
			TAG_FIELD(int32, end_vertex);
			TAG_FIELD(int32, forward_edge);
			TAG_FIELD(int32, reverse_edge);
			TAG_FIELD(int32, left_surface);
			TAG_FIELD(int32, right_surface);
		}; static_assert(sizeof(collision_edge) == 0x18, STATIC_ASSERT_FAIL);

		struct collision_vertex
		{
			TAG_FIELD(real_point3d, point);
			TAG_FIELD(int32, first_edge);
		}; static_assert(sizeof(collision_vertex) == 0x10, STATIC_ASSERT_FAIL);

		struct collision_bsp
		{
			TAG_TBLOCK(bsp3d_nodes, bsp3d_node);
			TAG_TBLOCK(planes, real_plane3d);
			TAG_TBLOCK(leaves, collision_leaf);
			struct {
				TAG_TBLOCK(references, bsp2d_reference);
				TAG_TBLOCK(nodes, bsp2d_node);
			}bsp2d;
			TAG_TBLOCK(surfaces, collision_surface);
			TAG_TBLOCK(edges, collision_edge);
			TAG_TBLOCK(vertices, collision_vertex);
		}; static_assert(sizeof(collision_bsp) == 0x60, STATIC_ASSERT_FAIL);
	};
};