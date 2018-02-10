/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\OpenSauce for specific license information
*/
#include "Common/Precompile.hpp"
#include <YeloLib/Halo1/cache/data_file_yelo.hpp>

#include <direct.h> // _mkdir

#include <blamlib/Halo1/cache/cache_files.hpp>
#include <YeloLib/files/files.hpp>
#include <YeloLib/Halo1/open_sauce/settings/c_settings_cheape.hpp>
#include <YeloLib/Halo1/open_sauce/settings/yelo_shared_settings.hpp>

namespace Yelo
{
	namespace DataFiles
	{
		static cstring K_DATA_FILE_SUBDIRECTORY = "data_files\\";
	};

	namespace Cache
	{
		void c_data_files_name_utils::SanitizeModSetName(_Inout_ tag_string& mod_set_name)
		{
			size_t mod_set_name_length = strlen(mod_set_name);

			if (mod_set_name_length > k_max_mod_set_name_length)
				mod_set_name[k_max_mod_set_name_length+1] = '\0';
		}

		void c_data_files_name_utils::BuildName(cstring mod_name, Enums::data_file_type type,
			char name[k_name_length+1])
		{
			if (!is_null_or_empty(mod_name))
			{
				strcpy_s(name, k_name_length+1, DataFiles::K_DATA_FILE_SUBDIRECTORY);
				strcat_s(name, k_name_length+1,	"-");
			}

			strcat_s(name, k_name_length+1, Cache::DataFileTypeToString(type));
		}
		void c_data_files_name_utils::BuildNames(cstring mod_name)
		{
			memset(m_names, 0, sizeof(m_names));
			BuildName(mod_name, Enums::_data_file_type_bitmaps,m_names[Enums::_data_file_type_bitmaps]);
			BuildName(mod_name, Enums::_data_file_type_sounds, m_names[Enums::_data_file_type_sounds]);
			BuildName(mod_name, Enums::_data_file_type_locale, m_names[Enums::_data_file_type_locale]);
		}

		bool c_data_files_finder::SearchPath(cstring maps_path)
		{
			char data_file_path[MAX_PATH] = "";
			strcpy_s(data_file_path, maps_path);
			// pointer into data_file_path that starts after the maps_path string
			char* data_file_name = data_file_path + strlen(maps_path);

			*data_file_name = '\0';
			strcat(data_file_name, m_names[Enums::_data_file_type_bitmaps]);
			strcat(data_file_name, K_DATA_FILE_EXTENSION);
			if (!FileIO::PathExists(data_file_path))
				return false;

			*data_file_name = '\0';
			strcat(data_file_name, m_names[Enums::_data_file_type_sounds]);
			strcat(data_file_name, K_DATA_FILE_EXTENSION);
			if (!FileIO::PathExists(data_file_path))
				return false;

			*data_file_name = '\0';
			strcat(data_file_name, m_names[Enums::_data_file_type_locale]);
			strcat(data_file_name, K_DATA_FILE_EXTENSION);
			if (!FileIO::PathExists(data_file_path))
				return false;

			return true;
		}
		bool c_data_files_finder::SearchEnvironment()
		{
			if (SearchPath(m_maps_path.environment))
				m_maps_path.final = m_maps_path.environment;

			return m_maps_path.final == m_maps_path.environment;
		}
		bool c_data_files_finder::SearchUserProfile()
		{
			if (SearchPath(m_maps_path.user_profile))
				m_maps_path.final = m_maps_path.user_profile;

			return m_maps_path.final == m_maps_path.user_profile;
		}
		c_data_files_finder::c_data_files_finder(cstring mod_name)
		{
			BuildNames(mod_name);

			m_maps_path.environment = Cache::K_MAP_FILES_DIRECTORY;
			m_maps_path.user_profile = Settings::PlatformUserMapsPath();
			m_maps_path.final = nullptr;
		}
		bool c_data_files_finder::TryAndFind(std::string& bitmaps_path, std::string& sounds_path, std::string& locale_path)
		{
			if (SearchEnvironment() || SearchUserProfile())
			{
				bitmaps_path.reserve(MAX_PATH);
				sounds_path.reserve(MAX_PATH);
				locale_path.reserve(MAX_PATH);

				bitmaps_path.append(m_maps_path.final);
				sounds_path.append(m_maps_path.final);
				locale_path.append(m_maps_path.final);

				bitmaps_path.append(m_names[Enums::_data_file_type_bitmaps]);
				sounds_path.append(m_names[Enums::_data_file_type_sounds]);
				locale_path.append(m_names[Enums::_data_file_type_locale]);

				bitmaps_path.append(K_DATA_FILE_EXTENSION);
				sounds_path.append(K_DATA_FILE_EXTENSION);
				locale_path.append(K_DATA_FILE_EXTENSION);

				bitmaps_path.shrink_to_fit();
				sounds_path.shrink_to_fit();
				locale_path.shrink_to_fit();

				return true;
			}

			bitmaps_path.clear();
			sounds_path.clear();
			locale_path.clear();

			return false;
		}
	};
};