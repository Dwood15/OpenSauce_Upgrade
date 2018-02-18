/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\OpenSauce for specific license information
*/
#include "Common/Precompile.hpp"
#include <YeloLib/Halo1/open_sauce/project_yellow_scenario_definitions.hpp>

#include <blamlib/Halo1/cseries/errors.hpp>
#include <blamlib/Halo1/tag_files/tag_groups.hpp>
#include <blamlib/Halo1/scenario/scenario_definitions.hpp>
#include <YeloLib/Halo1/hs/hs_yelo.hpp>
#include <YeloLib/Halo1/open_sauce/project_yellow_global_definitions.hpp>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace Yelo
{
	static_assert(sizeof(boost::uuids::uuid) == Enums::k_uuid_buffer_size, STATIC_ASSERT_FAIL);

	namespace TagGroups {
		bool s_project_yellow_scenario_build_info::HasUuid() const {
			return !CAST_PTR(const boost::uuids::uuid&, uuid_buffer).is_nil();
		}

		void s_project_yellow_scenario_build_info::GenerateUuid() {
			auto uuid_generator = boost::uuids::random_generator();

			CAST_PTR(boost::uuids::uuid&, uuid_buffer) = uuid_generator();
		}

		//////////////////////////////////////////////////////////////////////////
		// project_yellow
		cstring project_yellow::k_default_name = "i've got a lovely bunch of corncobs";

		bool project_yellow::_physics::IsGravityScaleValid() const {
			return gravity_scale >= 0.0f || gravity_scale <= 2.0f;
		}

		void project_yellow::_physics::ResetGravityScale() {
			gravity_scale = 1.0f;
		}

		bool project_yellow::_physics::IsPlayerSpeedScaleValid() const {
			return gravity_scale >= 0.0f || gravity_scale <= 6.0f;
		}

		void project_yellow::_physics::ResetPlayerSpeedScale() {
			player_speed_scale = 1.0f;
		}

		bool project_yellow::IsNull() const				{ return TEST_FLAG(flags, Flags::_project_yellow_null_definition_bit); }
		bool project_yellow::IsCacheProtected() const	{ return TEST_FLAG(flags, Flags::_project_yellow_cache_is_protected_bit); }
		//////////////////////////////////////////////////////////////////////////
	};
};