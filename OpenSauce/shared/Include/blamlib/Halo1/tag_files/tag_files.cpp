/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\OpenSauce for specific license information
	*/
#include "Common/Precompile.hpp"
#include <blamlib/Halo1/tag_files/tag_files.hpp>


namespace Yelo {
	namespace blam {
		cstring tag_name_strip_path(cstring name) {
			YELO_ASSERT(name);

			cstring sans_path=strrchr(name, '\\');

			return sans_path != nullptr ? sans_path + 1 : name;
		}
	};
};