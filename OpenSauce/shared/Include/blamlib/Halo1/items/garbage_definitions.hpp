/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\OpenSauce for specific license information
*/
#pragma once

#include <blamlib/Halo1/items/item_definitions.hpp>

namespace Yelo
{
	namespace TagGroups
	{
		struct _garbage_definition
		{
			TAG_PAD(int32, 42);
		}; static_assert(sizeof(_garbage_definition) == 0xA8, STATIC_ASSERT_FAIL);

		struct s_garbage_definition : s_item_definition
		{
			enum { k_group_tag = 'garb' };

			_garbage_definition garbage;
		};
	};
};