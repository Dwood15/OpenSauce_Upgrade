/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\Halo1_CE for specific license information
*/
#pragma once

#include <YeloLib/configuration/type_containers/c_type_container.hpp>
#include <YeloLib/configuration/c_configuration_value.hpp>

namespace Yelo::Configuration
{
	/// <summary>	A real vector 3D configuration container. </summary>
	class c_real_vector3d_container : public Configuration::c_type_container<real_vector3d>
	{
	private:
		Configuration::c_configuration_value<real> m_i;
		Configuration::c_configuration_value<real> m_j;
		Configuration::c_configuration_value<real> m_k;

	public:
		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Initializes a new instance of the c_real_vector3d_container class. </summary>
		///
		/// <param name="name">			The configuration node name for the value. </param>
		/// <param name="default_i">	The default value for i. </param>
		/// <param name="default_j">	The default value for j. </param>
		/// <param name="default_k">	The default value for k. </param>
		c_real_vector3d_container(std::string& name, real default_i, real default_j, real default_k);

		////////////////////////////////////////////////////////////////////////////////////////////////////
		/// <summary>	Initializes a new instance of the c_real_vector3d_container class. </summary>
		///
		/// <param name="name">	The configuration node name for the value. </param>
		c_real_vector3d_container(std::string& name);

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