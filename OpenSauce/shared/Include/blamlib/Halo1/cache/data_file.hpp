/*
	Yelo: Open Sauce SDK

	See license\OpenSauce\OpenSauce for specific license information
	*/
#pragma once

namespace Yelo {
	namespace Enums {
		enum data_file_type {
			_data_file_type_bitmaps,
			_data_file_type_sounds,
			_data_file_type_locale,

			k_number_of_data_file_types,
		};

		enum data_file_reference_type : _enum {
			_data_file_reference_none,
			_data_file_reference_bitmaps,
			_data_file_reference_sounds,
			_data_file_reference_locale,
		};
	};


};