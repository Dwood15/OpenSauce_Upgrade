/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#include "Common/Precompile.hpp"
#include "Interface/OpenSauceUI/Screen/c_screen_controller_mapdownload.hpp"

#if !PLATFORM_IS_DEDI

#include "Interface/OpenSauceUI/Control/control_property_ids.hpp"

namespace Yelo
{
	namespace Interface { namespace OpenSauceUI { namespace Screen
	{
#pragma region Resource ID Defines
#define K_LBL_MAP_DOWNLOAD_TITLE_ID									RESOURCE_ID("#LBL_map_download_title",			0xF29195EB)
		
#define K_LBL_MAP_NAME_ID											RESOURCE_ID("#LBL_map_name",					0x3BC95495)
#define K_PRG_MAP_PROGRESS_ID										RESOURCE_ID("#PRG_map_progress",				0x6B38B87F)
#define K_LBL_ESC_TO_CANCEL_TEXT_ID									RESOURCE_ID("#LBL_esc_to_cancel_text",			0x93F82DD2)

#define K_DOWNLOAD_SOURCE_TITLE_ID									RESOURCE_ID("#LBL_download_source_title",		0xCDB0CD99)
#define K_DOWNLOAD_SOURCE_DESCRIPTION_ID							RESOURCE_ID("#LBL_download_source_description",	0x88C43AA5)
#pragma endregion

		c_screen_controller_mapdownload::c_screen_controller_mapdownload(Definitions::c_screen_definition& definition): c_screen_controller_base(definition){ }

		void c_screen_controller_mapdownload::ResetScreen() {
			SetControlProperty(K_LBL_MAP_DOWNLOAD_TITLE_ID,				K_PROPERTY_TEXT_ID, "Map Download");
			SetControlProperty(K_LBL_MAP_NAME_ID,						K_PROPERTY_TEXT_ID, "");
			SetControlProperty(K_PRG_MAP_PROGRESS_ID,					K_PROPERTY_TEXT_ID, "0%");
			SetControlProperty(K_PRG_MAP_PROGRESS_ID,					K_PROPERTY_VALUE_ID, 0.0f);
			SetControlProperty(K_LBL_ESC_TO_CANCEL_TEXT_ID,				K_PROPERTY_TEXT_ID, "Press [esc] to cancel");
			SetControlProperty(K_DOWNLOAD_SOURCE_TITLE_ID,				K_PROPERTY_TEXT_ID, "");
			SetControlProperty(K_DOWNLOAD_SOURCE_DESCRIPTION_ID,		K_PROPERTY_TEXT_ID, "");
		}

		void c_screen_controller_mapdownload::SetStaticProperties() {
			// Set the screen to be central
			AnchorScreen(
				(screen_anchor)(_screen_anchor_center_v | _screen_anchor_center_h),
				(screen_anchor)(_screen_anchor_center_v | _screen_anchor_center_h));

			ResetScreen();
		}

		void c_screen_controller_mapdownload::BindDynamicProperties() {

		}

		void c_screen_controller_mapdownload::Show()
		{
			// Reset UI text before showing the control
			ResetScreen();

			c_screen_controller_base::Show();
		}
	};};};
};
#endif