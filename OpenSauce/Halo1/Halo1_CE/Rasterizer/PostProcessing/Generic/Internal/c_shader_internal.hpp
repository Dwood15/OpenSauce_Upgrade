/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

#if !PLATFORM_IS_DEDI
#include "Rasterizer/PostProcessing/Generic/c_shader_generic.hpp"

namespace Yelo
{
	namespace Rasterizer { namespace PostProcessing { namespace Generic { namespace Internal
	{
		class c_shader_internal : public c_shader_generic
		{
			/////////////////////////////////////////////////
			// members
		protected:
			struct
			{
				const char* shader_name;
				c_shader_data_postprocess_definition* shader_source_data;
				datum_index shader_tag_index;
			}m_members_internal;

			/////////////////////////////////////////////////
			// member accessors
		public:
			void SetShaderDefinition(TagGroups::s_shader_postprocess_definition* definition);
			void SetShaderName(const char* name);
			void SetDatumIndex(datum_index shader_index);
			datum_index GetDatumIndex();

			/////////////////////////////////////////////////
			// initializers
		public:
			void Ctor()
			{
				c_shader_generic::Ctor();

				m_members_internal.shader_name = NULL;
				m_members_internal.shader_source_data = NULL;
				m_members_internal.shader_tag_index = datum_index::null;
			}

			void Dtor()
			{
				c_shader_generic::Dtor();

				m_members_internal.shader_name = NULL;
				m_members_internal.shader_source_data->Dtor();
				delete m_members_internal.shader_source_data;
				m_members_internal.shader_source_data = NULL;
				m_members_internal.shader_tag_index = datum_index::null;
			}

			/////////////////////////////////////////////////
			// shader setup
		public:
			void	SetupShader();
			void	SetupBitmapParameters();
		};
	};};};};
};
#endif