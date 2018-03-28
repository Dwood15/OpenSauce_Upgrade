/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/

#pragma once

#include <cseries/KillCxxExceptions.hpp>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include "resource.h"
#include <Windows.h>
#include <sal.h>

#include <WinSock2.h>

#include <ShlObj.h>
#include <Shlwapi.h>
#pragma comment (lib, "shlwapi.lib")

#define _USE_MATH_DEFINES // hurrrrrrrrrr, i like math!
#include <math.h>
#include <d3dx9math.h>

#include <d3dx9.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <vld.h>

//////////////////////////////////////////////////////////////////////////
// STD C includes
#include <string>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <io.h>
#include <cerrno>
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// STL includes
#include <array>
#include <iterator>
#include <memory> // std::unique_ptr
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <utility>
#include <vector>
#include <thread>
#include <random>
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Boost includes
#include <boost/preprocessor.hpp>
#include <boost/integer/static_log2.hpp>
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// GameSpy includes & definitions

// Comment out this if you don't have access to the Open SDK
// You'll also need to remove the code file references from the project
// TODO: use msbuild to define this since we can now detect the presence of required libraries?

//////////////////////////////////////////////////////////////////////////

#define YELO_NO_NETWORK
//#define API_DEBUG_MEMORY

#include "Common/Platform.hpp"

#include <blamlib/Halo1/cseries/cseries.hpp>
#include <blamlib/scenario/scenario_location.hpp>
#include <YeloLib/Halo1/open_sauce/blam_memory_upgrades.hpp>

#include <YeloLib/cseries/errors_yelo.hpp>
#include <blamlib/Halo1/cseries/errors.hpp>
#include <YeloLib/Halo1/cseries/memory_yelo.hpp>

#include "Settings/Settings.hpp"
