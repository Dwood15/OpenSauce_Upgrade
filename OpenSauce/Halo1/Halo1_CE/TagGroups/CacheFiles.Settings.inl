/*
	Yelo: Open Sauce SDK
	Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
	*/

c_settings_container::c_settings_container() : Configuration::c_configuration_container("CacheFiles"), m_check_yelo_files_first("CheckYeloFilesFirst", true), m_mainmenu_scenario("MainMenuScenario", "") { }

const std::vector<Configuration::i_configuration_value* const> c_settings_container::GetMembers() {
	return std::vector<i_configuration_value* const> { &m_check_yelo_files_first, &m_mainmenu_scenario };
}

void c_settings_cache::PostLoad() {
	c_map_file_finder::g_search_for_yelo_first=Get().m_check_yelo_files_first;
}