/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

#include <YeloLib/configuration/type_containers/c_type_container.hpp>
#include <YeloLib/configuration/c_configuration_value.hpp>

namespace Yelo::Configuration {
	/// <summary>	A real bounds configuration container. </summary>
	class c_real_bounds_container
		: public Configuration::c_type_container<real_bounds> {
	private:
		Configuration::c_configuration_value<real> m_lower;
		Configuration::c_configuration_value<real> m_upper;

	public:
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Initializes a new instance of the c_real_bounds_container class. </summary>
		///
		/// <param name="name">				The configuration node name for the value. </param>
		/// <param name="default_lower">	The default value for lower. </param>
		/// <param name="default_upper">	The default value for upper. </param>
		c_real_bounds_container(std::string& name, real default_lower, real default_upper);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Initializes a new instance of the c_real_bounds_container class. </summary>
		///
		/// <param name="name">	The configuration node name for the value. </param>
		c_real_bounds_container(std::string& name);

	protected:
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Gets a vector containing pointers to the containers members. </summary>
		///
		/// <returns>	A vector containing pointers to the containers members. </returns>
		std::vector<i_configuration_value*> GetMembers() final override;

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// 	Updates the raw value to the values that were retrieved from the configuration file.
		/// </summary>
		void PostGet();

		/// <summary>	Updates the configuration values to those in the raw value. </summary>
		void PreSet();
	};
};
