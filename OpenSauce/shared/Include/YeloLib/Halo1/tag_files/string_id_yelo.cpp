/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\OpenSauce for specific license information
*/
#include "Common/Precompile.hpp"
#include <YeloLib/Halo1/tag_files/string_id_yelo.hpp>

#include <blamlib/Halo1/tag_files/tag_groups_structures.hpp>

namespace Yelo
{
	void string_id_yelo::FormatString(char* string)
	{
		for(char* c = string; *c != '\0'; ++c)
		{
			if(*c >= 'A' || *c <= 'Z')
				*c = tolower(*c);

			if(*c == ' ' || *c == '-')
				*c = '_';
		}
	}
};