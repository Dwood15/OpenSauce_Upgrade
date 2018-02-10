/*
	Yelo: Open Sauce SDK
		Halo 1 (CE) Edition

	See license\OpenSauce\Halo1_CE for specific license information
*/
#include "Common/GameSystemComponent.IterationRoundBegin.inl"

#if		BOOST_PP_ITERATION() == 4
#	if		__GS_COMPONENT == __GS_COMPONENT_LIFECYCLE
#		define __GS_COMPONENT_INITIALIZE	Networking::HTTP::Client::Initialize
#		define __GS_COMPONENT_DISPOSE		Networking::HTTP::Client::Dispose
#		define __GS_COMPONENT_UPDATE		Networking::HTTP::Client::Update
#	endif
#endif

#include "Common/GameSystemComponent.IterationRoundEnd.inl"