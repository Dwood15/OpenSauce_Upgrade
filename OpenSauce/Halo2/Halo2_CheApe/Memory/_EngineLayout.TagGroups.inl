/*
    Yelo: Open Sauce SDK
		Halo 2 (Editing Kit) Edition

	See license\OpenSauce\Halo2_CheApe for specific license information
*/

//////////////////////////////////////////////////////////////////////////
// TagGroups.cpp
#if __EL_INCLUDE_FILE_ID == __EL_TAGGROUPS_TAG_GROUPS

	FUNC_PTR(TAG_GET_GROUP_TAG,					0x482910, 0x52CF10, 0x4AE900);
	FUNC_PTR(TAG_GET_ROOT_BLOCK,				0x482930, 0x52CF30, 0x4AE920);
	FUNC_PTR(TAG_GET_NAME,						0x482950, 0x52CF50, 0x4AE940);
	FUNC_PTR(TAG_FILE_EXISTS,					0x4829D0, 0x52D000, 0x4AE9F0);
	FUNC_PTR(TAG_BLOCK_CLEAR,					0x4829E0, 0x52D010, 0x4AEA00);
	FUNC_PTR(TAG_REFERENCE_CLEAR,				0x482A50, 0x52D080, 0x4AEA70);
	FUNC_PTR(TAG_REFERENCE_ZERO,				FUNC_PTR_NULL, 0x52D0B0, FUNC_PTR_NULL); // NOTE: Only in Tool
	FUNC_PTR(TAG_REFERENCE_MOVE,				0x482A80, 0x52D0D0, 0x4AEAA0);
	FUNC_PTR(TAG_REFERENCES_COMPARE,			0x482AD0, 0x52D100, 0x4AEAD0);
	FUNC_PTR(TAG_IS_READ_ONLY,					FUNC_PTR_NULL, 0x52D1A0, 0x4AEB70); // NOTE: Guerilla
	FUNC_PTR(TAG_BLOCK_MOVE,					0x482BF0, 0x52D240, 0x4AEC10);
	FUNC_PTR(TAG_REFERENCE_SET,					0x482C40, 0x52D290, 0x4AEC60);
	FUNC_PTR(TAG_REFERENCE_COPY,				0x482C80, 0x52D2D0, 0x4AECA0);
	FUNC_PTR(TAG_GROUP_GET_NEXT,				0x482DB0, 0x52D3C0, 0x4AEDB0);
	FUNC_PTR(TAG_DATA_GET_POINTER,				0x482E60, 0x52D4A0, 0x4AEE90);
	FUNC_PTR(STRING_ID_GET_STRING,				0x482EF0, 0x52D550, 0x4AEF20);
	FUNC_PTR(STRING_ID_GET_VALUE,				0x482F50, 0x52D5B0, 0x4AEF80);
	FUNC_PTR(TAG_BLOCK_DELETE_ALL,				0x483380, 0x52D9E0, 0x4AF3B0);
	FUNC_PTR(FIND_TAG_INSTANCE,					0x483740, 0x52DDA0, 0x4AF770);
	FUNC_PTR(TAG_GROUP_GET,						0x484000, 0x52E660, 0x4B0030);
	FUNC_PTR(TAG_GET,							0x484A00, 0x52F150, 0x4B0B20);
	FUNC_PTR(TAG_UNLOAD,						0x484E40, 0x52F7C0, 0x4B1190);
	FUNC_PTR(TAG_ORPHAN,						FUNC_PTR_NULL, 0x52F810, FUNC_PTR_NULL); // NOTE: Only in Tool
	FUNC_PTR(TAG_RENAME,						0x484E90, 0x52F840, 0x4B11E0);
	FUNC_PTR(TAG_LOADED,						0x484FF0, 0x52F9A0, 0x4B1340);
	FUNC_PTR(TAG_DATA_RESIZE,					0x485050, 0x52FA00, 0x4B13A0);
	FUNC_PTR(TAG_NEW,							0x486A00, 0x5313F0, 0x4B2E10);
	FUNC_PTR(TAG_BLOCK_GET_ELEMENT_WITH_SIZE,	0x487F80, 0x532970, 0x4B4390);
	FUNC_PTR(TAG_SAVE,							0x4883A0, 0x532F40, 0x4B47C0);
	FUNC_PTR(TAG_BLOCK_GET_ELEMENT,				0x488890, 0x533430, 0x4B4CB0);
	FUNC_PTR(TAG_LOAD,							0x488D90, 0x533930, 0x4B51B0);
	FUNC_PTR(TAG_BLOCK_ADD_ELEMENT,				0x489100, 0x533CA0, 0x4B5520);
	FUNC_PTR(TAG_REFERENCE_IS_VALID,			0x489150, 0x533CF0, 0x4B5570);
	FUNC_PTR(DUPLICATE_RECURSIVE_TAG_BLOCK,		0x4893D0, 0x533F70, 0x4B57F0);
	FUNC_PTR(TAG_RELOAD,						0x489670, 0x534210, 0x4B5A90);
	FUNC_PTR(TAG_BLOCK_RESIZE,					0x489840, 0x5343E0, 0x4B5C60);
	FUNC_PTR(TAG_LOAD_CHILDREN,					0x489C80, 0x5347E0, 0x4B60A0);
	FUNC_PTR(TAG_BLOCK_COPY,					0x489CB0, 0x534810, 0x4B60D0);
	FUNC_PTR(TAG_COPY_AND_ORPHAN,				0x489D10, 0x534870, 0x4B6130);
	FUNC_PTR(TAG_BLOCK_DELETE_ELEMENT,			0x489DB0, 0x534910, 0x4B61D0);

	namespace TagGroups
	{
		FUNC_PTR(SHADER_FILL_POSTPROCESS_DEFINITION,				0x643330, 0x70BF30, 0x6DB940);
		FUNC_PTR(SHADER_FILL_POSTPROCESS_DEFINITION_CALL,			0x635D92, 0x6FCE82, 0x6BD3D2);

		FUNC_PTR(SHADER_POSTPROCESS,								0x63FCE0, 0x705DE0, 0x6C8950);
		FUNC_PTR(SHADER_POSTPROCESS_CALL_PARTICLE_GROUP_POSTPROCESS,0x50C631, 0x5737E1, 0x546711);

		FUNC_PTR(SHADER_TEMPLATE_PLATFORM_POSTPROCESS,				0x63FE50, 0x705F50, 0x6C8AC0);
		FUNC_PTR(SHADER_TEMPLATE_PLATFORM_POSTPROCESS_CALL,			0x635E08, 0x6FCEF8, 0x6BD448);

		FUNC_PTR(SHADER_PASS_PLATFORM_POSTPROCESS,					0x640110, 0x706210, 0x6C8D80);
		FUNC_PTR(SHADER_PASS_PLATFORM_POSTPROCESS_CALL,				0x635E5E, 0x6FCF4E, 0x6BD49E);
	};

#else
	#error Undefined engine layout include for: __EL_INCLUDE_TAGGROUPS
#endif