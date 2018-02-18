/*
	Yelo: Open Sauce SDK
		Halo 1 (Editing Kit) Edition

	See license\OpenSauce\Halo1_CheApe for specific license information
*/
#pragma once

namespace Yelo
{
	namespace TagGroups
	{
		struct s_tag_reference
		{
			TAG_FIELD(tag_reference, tag);
		}; static_assert(sizeof(s_tag_reference) == 0x10, STATIC_ASSERT_FAIL); // max count: 200

		struct s_tag_collection_definition
		{
			enum { k_group_tag = 'tagc' };

			TAG_TBLOCK(tag_references, s_tag_reference);
		}; static_assert(sizeof(s_tag_collection_definition) == 0xC, STATIC_ASSERT_FAIL); // max count: 1
	};
};
