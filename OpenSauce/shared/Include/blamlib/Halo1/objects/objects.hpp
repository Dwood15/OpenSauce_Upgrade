/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

#include <blamlib/Halo1/memory/data.hpp>
#include <blamlib/Halo1/memory/memory_pool.hpp>
#include <blamlib/Halo1/objects/object_types.hpp>
#include <blamlib/Halo1/scenario/scenario.hpp> // for object names

namespace Yelo
{
	namespace TagGroups
	{
		struct model_animation_graph;

		struct s_object_definition;
	};

	namespace Enums
	{
		enum networked_datum : long_enum;

		enum {
			// Stock game state allocation size for the object memory pool
			// When running in editor tools, this and the max number of objects is increased by 5x
			k_object_memory_pool_allocation_size = 0x200000,
		};
	};

	namespace Flags
	{
		enum object_header_flags : byte_flags
		{
			_object_header_active_bit,
			_object_header_visible_bit,
			_object_header_newly_created_bit, // true until after the first call to the 
			_object_header_being_deleted_bit,
			_object_header_child_bit, // attached to another object (at a marker/node)
			_object_header_connected_to_map_bit,
			_object_header_automatic_deactivation_bit,
			_object_header_unk7_bit,
		};

		enum objects_find_flags : long_flags
		{
			_objects_find_collideable_bit,
			_objects_find_noncollideable_bit,
		};
	};

	namespace Objects {
		struct s_object_placement_data;
		struct s_object_data;

		struct s_object_header_datum : Memory::s_datum_base
		{
			Flags::object_header_flags flags;
			byte_enum object_type;
			uint16 cluster_index;
			uint16 data_size;
			union {
				void* address;

				s_object_data* _object; // Note: since 's_object_data' is the base of all object datums, we can do this

				// Allows us to implicitly treat 'address' as an specific object type ptr
				// in situations where we're programming for something more specific (eg, unit types only)
				struct s_scenery_datum* _scenery;
				struct s_sound_scenery_datum* _sound_scenery;
				struct s_garbage_datum* _garbage;
				struct s_projectile_datum* _projectile;
				struct s_placeholder_datum* _placeholder;

				struct s_device_datum* _device;
				struct s_device_machine_datum* _machine;
				struct s_device_control_datum* _control;
				struct s_device_lightfixture_datum* _lightfixture;

				struct s_item_datum* _item;
				struct s_weapon_datum* _weapon;
				struct s_equipment_datum* _equipment;

				struct s_unit_datum* _unit;
				struct s_biped_datum* _biped;
				struct s_vehicle_datum* _vehicle;
			};
		}; static_assert(sizeof(s_object_header_datum) == 0xC, STATIC_ASSERT_FAIL);
		typedef Memory::DataArray<	s_object_header_datum, 2048> object_header_data_t;

		struct s_objects_pool_data
		{
			Memory::s_memory_pool header;

			byte data[Enums::k_object_memory_pool_allocation_size];
		};

		struct s_object_globals_data
		{
			bool object_is_being_placed;
			bool object_marker_initialized;
			bool garbage_collect_now;
			UNKNOWN_TYPE(bool);
			int16 object_type_being_placed; // hs type
			UNKNOWN_TYPE(int16);
			UNKNOWN_TYPE(datum_index);
			long_flags pvs[BIT_VECTOR_SIZE_IN_DWORDS(512)];
			long_flags pvs2[BIT_VECTOR_SIZE_IN_DWORDS(512)];
			int32 last_garbage_collection_tick;					// 0x8C
			// none = 0
			// object = 1
			// cluster = 2
			_enum pvs_activator_type;							// 0x90
			PAD16;
			union {												// 0x94
				datum_index pvs_activating_object_index;
				int16 pvs_activating_cluster_index;
			};
		}; static_assert(sizeof(s_object_globals_data) == 0x98, STATIC_ASSERT_FAIL);

		struct s_object_name_list_data
		{
			std::array<datum_index, Enums::k_maximum_object_names_per_scenario> object_name_to_datum_table;
		};

		struct s_object_iterator
		{
			enum { k_signature = 0x86868686 };

			long_flags type_mask;						// object types to iterate
			Flags::object_header_flags ignore_flags;	// When any of these bits are set, the object is skipped
			PAD8;
			int16 next_index;
			datum_index object_index;
			tag signature;

			void SetEndHack()		{ signature = Memory::s_data_iterator::k_end_hack_signature; }
			bool IsEndHack() const	{ return signature == Memory::s_data_iterator::k_end_hack_signature; }
		};

		struct s_object_marker
		{
			int16 node_index;
			PAD16;
			real_matrix4x3 matrix;
			real_matrix4x3 transformed_matrix;
		}; static_assert(sizeof(s_object_marker) == 0x6C, STATIC_ASSERT_FAIL);


		object_header_data_t&							ObjectHeader();
		s_objects_pool_data*							ObjectsPool();
		s_object_globals_data*							ObjectGlobals();
		s_object_name_list_data*						ObjectNameList();

		TagGroups::s_object_definition const* GetObjectDefinition(datum_index object_index);
		template<typename TObjectDefinition> inline
		TObjectDefinition const* GetObjectDefinition(datum_index object_index)
		{
			return CAST_PTR(TObjectDefinition const*, GetObjectDefinition(object_index));
		}

		TagGroups::model_animation_graph const* GetObjectAnimations(datum_index object_index);

		// Checks to see if [object_index_to_test] is an enemy of [object_index]
		bool ObjectIsEnemy(datum_index object_index, datum_index object_index_to_test);
	};

	namespace blam
	{
		using namespace Yelo::Objects;

		datum_index object_index_from_name_index(int16 name_index);

		void object_set_object_index_for_name_index(int16 name_index, datum_index object_index);

		void PLATFORM_API object_reset(datum_index object_index);

		// precondition: [object_index] is a valid object index, everything else can be null
		// Recomputes the node matrices and reconnects the object to the map after updating the position data
		void PLATFORM_API object_set_position(datum_index object_index, 
			__in_opt real_point3d* new_position = nullptr, __in_opt real_vector3d* new_forward = nullptr, __in_opt real_vector3d* new_up = nullptr);

		void PLATFORM_API object_set_position_network(datum_index object_index, 
			real_point3d* new_position);

		void PLATFORM_API object_translate(datum_index object_index, 
			const real_point3d& new_position, __in_opt const s_scenario_location* new_location = nullptr);

		void PLATFORM_API object_placement_data_new(s_object_placement_data& data, datum_index object_definition_index, datum_index owner_object_index = datum_index::null);

		datum_index PLATFORM_API object_new(s_object_placement_data& data);

		datum_index PLATFORM_API object_new_with_role(s_object_placement_data& data, Enums::networked_datum role);

		// Sends a message_object_deletion packet for [object_index], deleting it in the process
		void PLATFORM_API object_delete_to_network(datum_index object_index);

		void PLATFORM_API object_delete(datum_index object_index);

		void PLATFORM_API object_reconnect_to_map(datum_index object_index, __in_opt s_scenario_location* location_reference = nullptr);

		void PLATFORM_API object_disconnect_from_map(datum_index object_index);

		int16 PLATFORM_API object_get_marker_by_name(const datum_index object_index, cstring marker_name, s_object_marker* markers, const int16 maximum_marker_count);

		// Attaches the object to the target_object (marker names can be empty strings)
		void PLATFORM_API object_attach_to_marker(datum_index target_object_index, cstring target_marker_name, datum_index object_index, cstring marker_name);

		// Detaches the object from its parent
		void PLATFORM_API object_detach(datum_index object_index);

		// Get the origin of [object_index]. Takes the parent object (if there is one) into account.
		real_point3d& PLATFORM_API object_get_origin(datum_index object_index, __out real_point3d& return_origin);

		// Get the orientation of [object_index]. Takes the parent object (if there is one) into account.
		void PLATFORM_API object_get_orientation(datum_index object_index, 
			__out_opt real_vector3d* return_forward, __out_opt real_vector3d* return_up);

		// Get the scenario location of [object_index]
		s_scenario_location& PLATFORM_API object_get_location(datum_index object_index, __out s_scenario_location& return_location);

		void PLATFORM_API object_start_interpolation(datum_index object_index, int32 interpolation_frame_count);

		s_object_data* PLATFORM_API object_try_and_get_and_verify_type(datum_index object_index, long_flags expected_types);
		template<typename T>
		T* object_try_and_get_and_verify_type(datum_index object_index)
		{
			return CAST_PTR(T*, object_try_and_get_and_verify_type(object_index, T::k_object_types_mask));
		}

		s_object_iterator& object_iterator_new(s_object_iterator& iter, long_flags type_mask, Flags::object_header_flags ignore_flags);

		s_object_data* PLATFORM_API object_iterator_next(s_object_iterator& iter);

		int16 PLATFORM_API objects_in_sphere(Flags::objects_find_flags find_flags, long_flags object_type_flags, 
			const s_scenario_location& location, const real_point3d& center, real radius, 
			datum_index object_indices[], int16 maximum_object_indices);

		// Loads the predicted resources defined in [object_index]'s tag definition (if they're not already loaded)
		void PLATFORM_API object_definition_predict(datum_index object_index);

		bool PLATFORM_API object_header_block_allocate(datum_index object_index, size_t block_reference_offset, size_t size);

		// Sets the scale of an object over a duration of time (game ticks)
		void PLATFORM_API objects_scripting_set_scale(datum_index object_index, real scale, int32 ticks);
		
		real PLATFORM_API object_get_level_of_detail_pixels(datum_index object_index);

		void PLATFORM_API object_render_state_refresh(datum_index object_render_state_index, datum_index object_index, real level_of_detail_pixels, byte arg4);

		void PLATFORM_API objects_update();

		void PLATFORM_API object_update(datum_index object_index);

		s_object_data* object_get(datum_index object_index);
		s_object_data* object_get_and_verify_type(datum_index object_index, long_flags expected_types);
		template<typename T> inline
		T* object_get_and_verify_type(datum_index object_index)
		{
			return CAST_PTR(T*, object_get_and_verify_type(object_index, T::k_object_types_mask));
		}

		void PLATFORM_API object_destroy(const datum_index object_index);
	};

	namespace Objects
	{
		class c_object_iterator {
			s_object_iterator m_state;
			s_object_data* m_object;

			c_object_iterator(const void* endHackDummy)
				: m_object(nullptr)
			{
				m_state.SetEndHack();
			}
		public:
			c_object_iterator(long_flags type_mask, Flags::object_header_flags ignore_flags = (Flags::object_header_flags)0)
				: m_object(nullptr)
			{
				blam::object_iterator_new(m_state, type_mask, ignore_flags);
			}

			static c_object_iterator all()
			{
				return c_object_iterator(Enums::_object_type_mask_all);
			}

			s_object_data* Next()
			{
				return m_object = blam::object_iterator_next(m_state);
			}

			bool operator!=(const c_object_iterator& other) const;

			c_object_iterator& operator++()
			{
				Next();
				return *this;
			}
			Memory::DataArrayIteratorResult<s_object_data> operator*() const
			{
				return Memory::DataArrayIteratorResult<s_object_data>(m_state.object_index, m_object);
			}

			c_object_iterator& begin() /*const*/
			{
				this->Next();
				return *this;
			}
			static const c_object_iterator end() /*const*/
			{
				return c_object_iterator(nullptr);
			}
		};
	};
};