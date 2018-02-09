/*
Yelo: Open Sauce SDK

See license\OpenSauce\OpenSauce for specific license information
*/
#include "Common/Precompile.hpp"
#include <blamlib/Halo1/cache/data_file.hpp>

#include <blamlib/Halo1/cache/cache_file_builder.hpp>
#include <blamlib/Halo1/cache/cache_files.hpp>
#include <blamlib/Halo1/cache/cache_files_globals.hpp>
#include <blamlib/Halo1/cache/data_file_structures.hpp>
#include <YeloLib/Halo1/open_sauce/settings/yelo_shared_settings.hpp>

namespace Yelo {
	namespace Cache {
		cstring K_DATA_FILE_EXTENSION=".map";

		cstring DataFileTypeToString(Enums::data_file_type type) {
			switch (type) {
				case Enums::_data_file_type_bitmaps:return "bitmaps";
				case Enums::_data_file_type_sounds:	return "sounds";
				case Enums::_data_file_type_locale:	return "loc";

					YELO_ASSERT_CASE_UNREACHABLE();
			}

			return nullptr;
		}
		cstring DataFileTypeToString(Enums::data_file_reference_type type) {
			return DataFileTypeToString(CAST(Enums::data_file_type, type - Enums::_data_file_reference_bitmaps));
		}

		s_data_file& s_data_file_globals::Get(Enums::data_file_reference_type data_file) {
			switch (data_file) {
				case Enums::_data_file_reference_bitmaps:return this->bitmaps;
				case Enums::_data_file_reference_sounds: return this->sounds;
				case Enums::_data_file_reference_locale: return this->locale;

					YELO_ASSERT_CASE_UNREACHABLE();
			}
		}
		s_data_file& DataFileGet(Enums::data_file_reference_type data_file) {
			auto& globals=CacheFileGlobals()->data_files;

			return globals.Get(data_file);

		}

		bool DataFileGetItemDataInfo(Enums::data_file_reference_type data_file, int32 item_index, int32& out_data_offset, int32& out_data_size) {
			auto& df=DataFileGet(data_file);

			return df.GetItemDataInfo(item_index, out_data_offset, out_data_size);
		}

		bool DataFileReadItemData(Enums::data_file_reference_type data_file, uint32 position, void* buffer, size_t buffer_size) {
			auto& df=DataFileGet(data_file);

			return df.ReadItemData(position, buffer, buffer_size);
		}

		bool DataFilesOpen(cstring bitmaps_path, cstring sounds_path, cstring locale_path, bool store_resources) {

			using namespace Enums;

			if (DataFileGet(_data_file_reference_bitmaps).Open(_data_file_reference_bitmaps, store_resources, bitmaps_path) &&
				DataFileGet(_data_file_reference_sounds).Open(_data_file_reference_sounds, store_resources, sounds_path) &&
				DataFileGet(_data_file_reference_locale).Open(_data_file_reference_locale, store_resources, locale_path)) {
				return true;
			}
			return false;
		}


		void s_data_file::FreeResources() {
			if (file_names.address != nullptr) {
				YELO_DELETE_ARRAY(file_names.address);
			}

			if (file_index_table.address != nullptr) {
				YELO_DELETE_ARRAY(file_index_table.address);
			}
		}

		bool s_data_file::Read(uint32 position, void* buffer, size_t buffer_size) {
			DWORD bytes_read;

			OVERLAPPED overlapped={ };
			overlapped.Offset=position;
			if (ReadFile(file_handle, buffer, buffer_size, &bytes_read, &overlapped) != FALSE) {
				return bytes_read == buffer_size;
			} else if (GetLastError() == ERROR_IO_PENDING &&
					   GetOverlappedResult(file_handle, &overlapped, &bytes_read, TRUE) != FALSE) {
				return bytes_read == buffer_size;
			}

			return false;
		}

		bool s_data_file::ReadHeader(Enums::data_file_reference_type expected_type) {
			if (!Read(0, &header, sizeof(header))) {
				DWORD last_error=GetLastError();

				return false;
			}

			if (header.type != expected_type) {
				memset(&header, 0, sizeof(header));

				return false;
			}

			return true;
		}

		bool s_data_file::ReadFileNames() {
			DWORD buffer_size=header.file_index_table_offset - header.file_names_offset;
			file_names.address=YELO_NEW_ARRAY(char, buffer_size);

			if (!Read(header.file_names_offset, file_names.address, buffer_size)) {
				DWORD last_error=GetLastError();
				return false;
			}

			file_names.total_size=file_names.used_size=buffer_size;

			return true;
		}

		bool s_data_file::ReadFileIndexTable() {
			DWORD buffer_size=sizeof(s_data_file_item)* header.tag_count;
			file_index_table.address=YELO_NEW_ARRAY(s_data_file_item, header.tag_count);

			if (!Read(header.file_index_table_offset, file_index_table.address, buffer_size)) {
				DWORD last_error=GetLastError();

				return false;
			}

			file_index_table.count=header.tag_count;

			return true;
		}

		bool s_data_file::Open(Enums::data_file_reference_type data_file, bool store_resources,
							   cstring full_path) {
			memset(this, 0, sizeof(*this));
			this->name=full_path;
			this->writable=store_resources;

			DWORD file_access=GENERIC_READ;
			if (store_resources)
				file_access|=GENERIC_WRITE;

			DWORD file_share_mode=PLATFORM_ENGINE_VALUE(FILE_SHARE_READ, 0);

			DWORD file_flags=FILE_ATTRIBUTE_NORMAL;

			this->file_handle=CreateFileA(full_path, file_access, file_share_mode, nullptr, OPEN_ALWAYS, file_flags, nullptr);

			if (file_handle != INVALID_HANDLE_VALUE &&
				ReadHeader(data_file) && ReadFileNames() && ReadFileIndexTable()) {
				SetFilePointer(file_handle, header.file_names_offset, nullptr, FILE_BEGIN);
				return true;
			}

			FreeResources();
			if (store_resources) {
				header.type=data_file;
				header.file_names_offset=sizeof(header);
				DWORD bytes_written;
				WriteFile(file_handle, &header, sizeof(header), &bytes_written, nullptr);
			}

			DWORD last_error=GetLastError();
			printf_s("### FAILED TO OPEN DATA-CACHE FILE: %s.\n\n", this->name);
			return false;
		}

		bool s_data_file::Close() {
			CloseHandle(file_handle);
			FreeResources();

			memset(&header, 0, sizeof(header));
			file_handle=INVALID_HANDLE_VALUE; // engine doesn't do this
			return true;
		}

		bool s_data_file::GetItemDataInfo(int32 item_index,
										  int32& out_data_offset, int32& out_data_size) const {
			if (item_index < 0 || item_index >= file_index_table.count)
				return false;

			s_data_file_item* item=&file_index_table.address[item_index];
			out_data_offset=item->data_offset;
			out_data_size=item->size;

			return true;
		}

		bool s_data_file::ReadItemData(uint32 position, void* buffer, size_t buffer_size) {
			YELO_ASSERT(file_handle != INVALID_HANDLE_VALUE);

			return Read(position, buffer, buffer_size);
		}
	};

	namespace blam {
		bool data_files_close() {
			return
				data_file_close(Enums::_data_file_reference_bitmaps) &&
				data_file_close(Enums::_data_file_reference_sounds) &&
				data_file_close(Enums::_data_file_reference_locale);
		}

		bool data_file_close(Enums::data_file_reference_type data_file) {
			auto& file=Cache::DataFileGet(data_file);

			return file.Close();
		}

	};
};