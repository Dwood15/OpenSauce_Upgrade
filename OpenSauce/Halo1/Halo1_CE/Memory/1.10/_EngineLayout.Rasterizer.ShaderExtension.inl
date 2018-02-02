/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/

#if !PLATFORM_IS_DEDI
#include "Memory/1.10/Pointers/HaloCE_110_Runtime.Rasterizer.ShaderExtension.inl"
#include "Memory/1.10/Pointers/HaloCE_110_Runtime_Manual.Rasterizer.ShaderExtension.inl"
#endif

//////////////////////////////////////////////////////////////////////////
// ShaderExtension.cpp
#if __EL_INCLUDE_FILE_ID == __EL_RASTERIZER_SHADEREXTENSION
	FUNC_PTR(RASTERIZER_MODEL_DRAW_INVERT_BACKFACE_NORMALS_CHECK_HOOK,			K_RASTERIZER_MODEL_DRAW_INVERT_BACKFACE_NORMALS_CHECK_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_DRAW_INVERT_BACKFACE_NORMALS_CHECK_RETN,			K_RASTERIZER_MODEL_DRAW_INVERT_BACKFACE_NORMALS_CHECK_RETN, FUNC_PTR_NULL);

	FUNC_PTR(RASTERIZER_DX9_SHADERS_EFFECT_SHADERS_INITIALIZE__SPRINTF_CALL,	K_RASTERIZER_DX9_SHADERS_EFFECT_SHADERS_INITIALIZE__SPRINTF_CALL, PTR_NULL);

	static cstring* K_VSH_COLLECTION_PATH_REFERENCES[] = {
		CAST_PTR(cstring*,K_VSH_COLLECTION_PATH_REFERENCE_0),
		CAST_PTR(cstring*,K_VSH_COLLECTION_PATH_REFERENCE_1),
	};

	static void** K_PS_VERSION_ITERATOR_START[] = {
		CAST_PTR(void**,K_PS_VERSION_ITERATOR_START_0),
		CAST_PTR(void**,K_PS_VERSION_ITERATOR_START_1),
	};

#elif __EL_INCLUDE_FILE_ID == __EL_RASTERIZER_SHADEREXTENSION_MODEL
	static void** K_SHADER_USAGE_ID_ARRAY_REFERENCES[] = {
		CAST_PTR(void**,K_SHADER_USAGE_ID_ARRAY_REFERENCE_0),
		CAST_PTR(void**,K_SHADER_USAGE_ID_ARRAY_REFERENCE_1),
		CAST_PTR(void**,K_SHADER_USAGE_ID_ARRAY_REFERENCE_2),
		CAST_PTR(void**,K_SHADER_USAGE_ID_ARRAY_REFERENCE_3),
		CAST_PTR(void**,K_SHADER_USAGE_ID_ARRAY_REFERENCE_4),
		CAST_PTR(void**,K_SHADER_USAGE_ID_ARRAY_REFERENCE_5),
	};

	ENGINE_PTR(int, RASTERIZER_MODEL_SHADER_LOAD_COUNT,				K_RASTERIZER_MODEL_SHADER_LOAD_COUNT, PTR_NULL);
	ENGINE_PTR(int, RASTERIZER_MODEL_SHADER_LOAD_NO_INVERSE_COUNT,	K_RASTERIZER_MODEL_SHADER_LOAD_NO_INVERSE_COUNT, PTR_NULL);

	FUNC_PTR(RASTERIZER_MODEL_ENVIRONMENT_NO_USAGE_ID_OFFSET_HOOK,	K_RASTERIZER_MODEL_ENVIRONMENT_NO_USAGE_ID_OFFSET_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_ENVIRONMENT_NO_USAGE_ID_OFFSET_RETN,	K_RASTERIZER_MODEL_ENVIRONMENT_NO_USAGE_ID_OFFSET_RETN, FUNC_PTR_NULL);

	FUNC_PTR(RASTERIZER_MODEL_NO_USAGE_ID_OFFSET_HOOK,				K_RASTERIZER_MODEL_NO_USAGE_ID_OFFSET_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_NO_USAGE_ID_OFFSET_RETN,				K_RASTERIZER_MODEL_NO_USAGE_ID_OFFSET_RETN, FUNC_PTR_NULL);


	static void** K_PIXEL_SHADER_REFERENCES_ENVIRONMENT_NO[] = {
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_ENVIRONMENT_NO_0),
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_ENVIRONMENT_NO_1),
	};
	static void** K_PIXEL_SHADER_REFERENCES_CHANGE_COLOR[] = {
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_CHANGE_COLOR_0),
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_CHANGE_COLOR_1),
	};
	static void** K_PIXEL_SHADER_REFERENCES_MULTIPURPOSE[] = {
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_MULTIPURPOSE_0),
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_MULTIPURPOSE_1),
	};
	static void** K_PIXEL_SHADER_REFERENCES_NO[] = {
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_NO_0),
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_NO_1),
	};
	static void** K_PIXEL_SHADER_REFERENCES_REFLECTION[] = {
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_REFLECTION_0),
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_REFLECTION_1),
	};
	static void** K_PIXEL_SHADER_REFERENCES_SELF_ILLUMINATION[] = {
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_SELF_ILLUMINATION_0),
		CAST_PTR(void**,K_PIXEL_SHADER_REFERENCES_SELF_ILLUMINATION_1),
	};


	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_ENVIRONMENT_NO_HOOK,			K_RASTERIZER_MODEL_PS_INDEX_ENVIRONMENT_NO_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_ENVIRONMENT_NO_RETN,			K_RASTERIZER_MODEL_PS_INDEX_ENVIRONMENT_NO_RETN, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_ENVIRONMENT_NO_INV_HOOK,		K_RASTERIZER_MODEL_PS_INDEX_ENVIRONMENT_NO_INV_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_ENVIRONMENT_NO_INV_RETN,		K_RASTERIZER_MODEL_PS_INDEX_ENVIRONMENT_NO_INV_RETN, FUNC_PTR_NULL);

	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_CHANGE_COLOR_HOOK,			K_RASTERIZER_MODEL_PS_INDEX_CHANGE_COLOR_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_CHANGE_COLOR_RETN,			K_RASTERIZER_MODEL_PS_INDEX_CHANGE_COLOR_RETN, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_CHANGE_COLOR_INV_HOOK,		K_RASTERIZER_MODEL_PS_INDEX_CHANGE_COLOR_INV_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_CHANGE_COLOR_INV_RETN,		K_RASTERIZER_MODEL_PS_INDEX_CHANGE_COLOR_INV_RETN, FUNC_PTR_NULL);

	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_MULTIPURPOSE_HOOK,			K_RASTERIZER_MODEL_PS_INDEX_MULTIPURPOSE_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_MULTIPURPOSE_RETN,			K_RASTERIZER_MODEL_PS_INDEX_MULTIPURPOSE_RETN, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_MULTIPURPOSE_INV_HOOK,		K_RASTERIZER_MODEL_PS_INDEX_MULTIPURPOSE_INV_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_MULTIPURPOSE_INV_RETN,		K_RASTERIZER_MODEL_PS_INDEX_MULTIPURPOSE_INV_RETN, FUNC_PTR_NULL);

	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_NO_HOOK,						K_RASTERIZER_MODEL_PS_INDEX_NO_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_NO_RETN,						K_RASTERIZER_MODEL_PS_INDEX_NO_RETN, FUNC_PTR_NULL);

	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_REFLECTION_HOOK,				K_RASTERIZER_MODEL_PS_INDEX_REFLECTION_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_REFLECTION_RETN,				K_RASTERIZER_MODEL_PS_INDEX_REFLECTION_RETN, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_REFLECTION_INV_HOOK,			K_RASTERIZER_MODEL_PS_INDEX_REFLECTION_INV_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_REFLECTION_INV_RETN,			K_RASTERIZER_MODEL_PS_INDEX_REFLECTION_INV_RETN, FUNC_PTR_NULL);

	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_SELF_ILLUMINATION_HOOK,		K_RASTERIZER_MODEL_PS_INDEX_SELF_ILLUMINATION_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_SELF_ILLUMINATION_RETN,		K_RASTERIZER_MODEL_PS_INDEX_SELF_ILLUMINATION_RETN, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_SELF_ILLUMINATION_INV_HOOK,	K_RASTERIZER_MODEL_PS_INDEX_SELF_ILLUMINATION_INV_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_MODEL_PS_INDEX_SELF_ILLUMINATION_INV_RETN,	K_RASTERIZER_MODEL_PS_INDEX_SELF_ILLUMINATION_INV_RETN, FUNC_PTR_NULL);

#elif __EL_INCLUDE_FILE_ID == __EL_RASTERIZER_SHADEREXTENSION_ENVIRONMENT
	FUNC_PTR(RASTERIZER_ENVIRONMENT_PS_INDEX_ENVIRONMENT_LIGHTMAP_HOOK,	K_RASTERIZER_ENVIRONMENT_PS_INDEX_ENVIRONMENT_LIGHTMAP_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_ENVIRONMENT_PS_INDEX_ENVIRONMENT_LIGHTMAP_RETN,	K_RASTERIZER_ENVIRONMENT_PS_INDEX_ENVIRONMENT_LIGHTMAP_RETN, FUNC_PTR_NULL);
	
	FUNC_PTR(STRUCTURE_RENDER_DYNAMIC_TRIANGLES_COUNT_REF,				K_STRUCTURE_RENDER_DYNAMIC_TRIANGLES_COUNT_REF, FUNC_PTR_NULL);
	FUNC_PTR(STRUCTURE_RENDER_DYNAMIC_TRIANGLES_BUFFER_REF,				K_STRUCTURE_RENDER_DYNAMIC_TRIANGLES_BUFFER_REF, FUNC_PTR_NULL);
	FUNC_PTR(STRUCTURE_RENDER_PASS_FUNC,								K_STRUCTURE_RENDER_PASS_FUNC, FUNC_PTR_NULL);
	FUNC_PTR(LIGHTS_RENDER_SPECULAR_CALL,								K_LIGHTS_RENDER_SPECULAR_CALL, FUNC_PTR_NULL);
	FUNC_PTR(LIGHTS_RENDER_SPECULAR_FUNC,								K_LIGHTS_RENDER_SPECULAR_FUNC, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_ENVIRONMENT_LIGHTMAP_BEGIN_FUNC,				K_RASTERIZER_ENVIRONMENT_LIGHTMAP_BEGIN_FUNC, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_ENVIRONMENT_LIGHTMAP_DRAW_FUNC,					K_RASTERIZER_ENVIRONMENT_LIGHTMAP_DRAW_FUNC, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_ENVIRONMENT_LIGHTMAP_END_FUNC,					K_RASTERIZER_ENVIRONMENT_LIGHTMAP_END_FUNC, FUNC_PTR_NULL);

#elif __EL_INCLUDE_FILE_ID == __EL_RASTERIZER_SHADEREXTENSION_EFFECT
	FUNC_PTR(RASTERIZER_EFFECT_PARTICLE_SYSTEM_HOOK,				K_RASTERIZER_EFFECT_PARTICLE_SYSTEM_HOOK, FUNC_PTR);
	FUNC_PTR(RASTERIZER_EFFECT_PARTICLE_SYSTEM_RETN,				K_RASTERIZER_EFFECT_PARTICLE_SYSTEM_RETN, FUNC_PTR);
	FUNC_PTR(RASTERIZER_EFFECT_PARTICLE_HOOK,						K_RASTERIZER_EFFECT_PARTICLE_HOOK, FUNC_PTR);
	FUNC_PTR(RASTERIZER_EFFECT_PARTICLE_RETN,						K_RASTERIZER_EFFECT_PARTICLE_RETN, FUNC_PTR);
	FUNC_PTR(RASTERIZER_EFFECT_CONTRAIL_HOOK,						K_RASTERIZER_EFFECT_CONTRAIL_HOOK, FUNC_PTR);
	FUNC_PTR(RASTERIZER_EFFECT_CONTRAIL_RETN,						K_RASTERIZER_EFFECT_CONTRAIL_RETN, FUNC_PTR);

	FUNC_PTR(RASTERIZER_EFFECT_PS_INDEX_NO_HOOK,					K_RASTERIZER_EFFECT_PS_INDEX_NO_HOOK, FUNC_PTR_NULL);
	FUNC_PTR(RASTERIZER_EFFECT_PS_INDEX_NO_RETN,					K_RASTERIZER_EFFECT_PS_INDEX_NO_RETN, FUNC_PTR_NULL);
#else
	#error Undefined engine layout include for: __EL_RASTERIZER_SHADEREXTENSION
#endif