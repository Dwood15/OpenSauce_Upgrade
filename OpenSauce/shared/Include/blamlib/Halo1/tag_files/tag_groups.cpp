/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\OpenSauce for specific license information
	*/
#include "Common/Precompile.hpp"
#include <blamlib/Halo1/tag_files/tag_groups.hpp>

#include <blamlib/Halo1/models/model_definitions.hpp>
#include <blamlib/Halo1/tag_files/tag_field_scanner.hpp>
#include <YeloLib/Halo1/tag_files/string_id_yelo.hpp>
#include <YeloLib/Halo1/tag_files/tag_group_memory.hpp>

namespace Yelo {
	namespace TagGroups {
		//////////////////////////////////////////////////////////////////////////
		// c_tag_iterator
		c_tag_iterator::c_tag_iterator(const void* endHackDummy) //:
			//m_tag_index(datum_index::null)
		{
			//std::memset(&m_state, 0, sizeof(m_state));
			m_state.instances_iterator.SetEndHack();
		}

		c_tag_iterator::c_tag_iterator(const tag group_tag_filter) :
			m_tag_index(datum_index::null) {
			blam::tag_iterator_new(m_state, group_tag_filter);
		}

		datum_index c_tag_iterator::Next() {
			return m_tag_index=blam::tag_iterator_next(m_state);
		}

		bool c_tag_iterator::operator!=(const c_tag_iterator& other) const {
			// NOTE: we intentionally don't compare the group_tag filter
			return m_state.instances_iterator != other.m_state.instances_iterator;
		}
	};
};
