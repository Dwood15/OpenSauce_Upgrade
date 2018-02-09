/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\OpenSauce for specific license information
	*/
#pragma once

#include <blamlib/tag_files/tag_groups_base.hpp>
#include <YeloLib/tag_files/tag_groups_base_yelo.hpp>

namespace Yelo {
	namespace Enums {
		enum {
			k_protected_group_tag='prot', // HEK+: this overrides the scenario's group tag in the tag index.
		};
	};

	namespace TagGroups {
		// Note: when used in range based for loops this will create an unnecessary copy operation, but with SSE2 it shouldn't be that bad
		class c_tag_iterator {
			s_tag_iterator m_state;
			datum_index m_tag_index;

			c_tag_iterator(const void* endHackDummy);
		public:
			c_tag_iterator(const tag group_tag_filter);
			template<typename T>
			c_tag_iterator() :
				m_tag_index(datum_index::null) {
				blam::tag_iterator_new<T>(m_state);
			}
			// Get an iterator that doesn't have any specific group_tag filter
			static c_tag_iterator all() {
				return c_tag_iterator(NONE);
			}

			datum_index Next();

			bool operator!=(const c_tag_iterator& other) const;

			c_tag_iterator& operator++() {
				Next();
				return *this;
			}
			datum_index operator*() const {
				return m_tag_index;
			}

			c_tag_iterator& begin() /*const*/
			{
				this->Next();
				return *this;
			}
			static const c_tag_iterator end() /*const*/
			{
				return c_tag_iterator(nullptr);
			}
		};
	};

	namespace blam {
		void PLATFORM_API tag_groups_initialize();
		void PLATFORM_API tag_groups_dispose();
		void PLATFORM_API tag_groups_initialize_for_new_map();
		void PLATFORM_API tag_groups_dispose_from_old_map();
		void PLATFORM_API tag_groups_dump_memory();
		uint32 PLATFORM_API tag_groups_checksum();


		datum_index PLATFORM_API find_tag_instance(tag group_tag, cstring name);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Initialize a tag instance iterator for the given group tag </summary>
		///
		/// <param name="iter">			   	[out] The iterator to initialize </param>
		/// <param name="group_tag_filter">
		/// 	(Optional) the group tag to filter results by. Use [NONE] for [group_tag_filter] to
		/// 	iterate all tag groups.
		/// </param>
		void PLATFORM_API tag_iterator_new(TagGroups::s_tag_iterator& iter, const tag group_tag_filter=NONE);
		template<typename T> inline
			////////////////////////////////////////////////////////////////////////////////////////////////////
			/// <summary>	Initialize a tag instance iterator for the given group tag </summary>
			///
			/// <param name="iter">	[out] The iterator to initialize </param>
			void tag_iterator_new(TagGroups::s_tag_iterator& iter) {
				tag_iterator_new(iter, T::k_group_tag);
			}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Increment a tags instance iterator to the next instance </summary>
		///
		/// <param name="iter">	[in,out] The iterator to increment </param>
		///
		/// <returns>	Returns the next datum's index or [datum_index::null] when finished iterating </returns>
		datum_index PLATFORM_API tag_iterator_next(TagGroups::s_tag_iterator& iter);
	};

	namespace TagGroups {};
};