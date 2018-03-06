/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\OpenSauce for specific license information
*/
#pragma once

#include <blamlib/tag_files/tag_groups_base.hpp>
#include <YeloLib/tag_files/tag_groups_base_yelo.hpp>

#if PLATFORM_IS_EDITOR
	#include <blamlib/Halo1/tag_files/tag_groups_structures.hpp>
#endif

namespace Yelo
{
	namespace Enums
	{
		enum {
			k_protected_group_tag = 'prot', // HEK+: this overrides the scenario's group tag in the tag index.
		};
	};

	namespace TagGroups
	{
		// Note: when used in range based for loops this will create an unnecessary copy operation, but with SSE2 it shouldn't be that bad
		class c_tag_iterator {
			s_tag_iterator m_state;
			datum_index m_tag_index;

			c_tag_iterator(const void* endHackDummy);
		public:
			c_tag_iterator(const tag group_tag_filter);
			template<typename T>
			c_tag_iterator() :
				m_tag_index(datum_index::null)
			{
				blam::tag_iterator_new<T>(m_state);
			}
			// Get an iterator that doesn't have any specific group_tag filter
			static c_tag_iterator all()
			{
				return c_tag_iterator(NONE);
			}

			datum_index Next();

			bool operator!=(const c_tag_iterator& other) const;

			c_tag_iterator& operator++()
			{
				Next();
				return *this;
			}
			datum_index operator*() const
			{
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

	namespace blam
	{
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
		void PLATFORM_API tag_iterator_new(TagGroups::s_tag_iterator& iter, const tag group_tag_filter = NONE);
		template<typename T> inline
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Initialize a tag instance iterator for the given group tag </summary>
		///
		/// <param name="iter">	[out] The iterator to initialize </param>
		void tag_iterator_new(TagGroups::s_tag_iterator& iter)
		{
			tag_iterator_new(iter, T::k_group_tag);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Increment a tags instance iterator to the next instance </summary>
		///
		/// <param name="iter">	[in,out] The iterator to increment </param>
		///
		/// <returns>	Returns the next datum's index or [datum_index::null] when finished iterating </returns>
		datum_index PLATFORM_API tag_iterator_next(TagGroups::s_tag_iterator& iter);

#if PLATFORM_IS_EDITOR
		// Get the group definition that follows [group], or the first group if it is NULL
		tag_group* PLATFORM_API tag_group_get_next(const tag_group* group = nullptr);

		// Get the group definition based on a four-character code
		tag_group* PLATFORM_API tag_group_get(tag group_tag);
		template<typename T> inline
		tag_group* tag_group_get()
		{
			return tag_group_get(T::k_group_tag);
		}

		// Rename the tag definition [tag_index] to [new_name]
		void PLATFORM_API tag_rename(datum_index tag_index, cstring new_name);

		tag_block* PLATFORM_API tag_block_index_resolve(datum_index tag_index, tag_field* block_index_field, int32 index);

		// Get the size in bytes of how much memory the tag definition [tag_index] 
		// consumes with all of its child data too
		uint32 PLATFORM_API tag_size(datum_index tag_index);

		// Get the size in bytes of how much memory [block] consumes with all 
		// of its child data too
		uint32 PLATFORM_API tag_block_size(tag_block* block);

		// Insert a new block element at [index] and return the index 
		// of the inserted element
		int32 PLATFORM_API tag_block_insert_element(tag_block* block, int32 index);
		template<typename T> inline
		T* tag_block_insert_element(TagBlock<T>& block, int32 index)
		{
			return CAST_PTR(T*, tag_block_insert_element(block.to_tag_block(), index));
		}

		// Duplicate the block element at [element_index] and return the index which 
		// represents the duplicated element
		int32 PLATFORM_API tag_block_duplicate_element(tag_block* block, int32 element_index);
		template<typename T> inline
		int32 tag_block_duplicate_element(TagBlock<T>& block, int32 element_index)
		{
			return tag_block_duplicate_element(block.to_tag_block(), element_index);
		}

		void PLATFORM_API tag_block_generate_default_element(const tag_block_definition *definition, void *address);
#endif
	};

	namespace TagGroups
	{
#if PLATFORM_IS_EDITOR
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Get the tag_group for a specific tag definition </summary>
		inline tag_group* TagGetGroup(datum_index tag_index)
		{
			return blam::tag_group_get(blam::tag_get_group_tag(tag_index));
		}

		// Visit all the registered tag_groups and perform an action on them
		// TAction: void operator()([const] tag_group* group)
		template<class TAction>
		void tag_groups_do_action(TAction& action = TAction())
		{
			for(tag_group* group = blam::tag_group_get_next(); group; group = blam::tag_group_get_next(group))
				action(group);
		}
		// Visit all the registered tag_groups and perform an action on their header_block_definition
		// TAction: void operator()(tag_block_definition* header_block_definition)
		template<class TAction>
		void tag_groups_do_header_block_definition_action(TAction& action = TAction())
		{
			for(tag_group* group = blam::tag_group_get_next(); group; group = blam::tag_group_get_next(group))
				action(group->header_block_definition);
		}

		// TAction: bool operator()([const] tag_block* block, int32 element_index, [const] void* element)
		template<class TAction>
		bool tag_block_elements_do_action_sans_safe_get(tag_block* block, TAction& action = TAction())
		{
			auto* definition = block->definition;
			bool result = true;

			for (int x = 0; x < block->count; x++)
			{
				// TODO: need to account for alignment, if we ever fully implement it
				void* element = CAST_PTR(byte*, block->address) +
					(definition->element_size * x);

				result &= action(block, x, element);
			}

			return result;
		}
#endif
	};
};