///////////////////////////////////////////////////////////////////////////////
// File:	sampleAuth.c
// SDK:		GameSpy Authentication Service SDK
//
// Copyright (c) 2012 GameSpy Technology & IGN Entertainment, Inc. All rights
// reserved. This software is made available only pursuant to certain license
// terms offered by IGN or its subsidiary GameSpy Industries, Inc. Unlicensed
// use or use in a manner not expressly authorized by IGN or GameSpy Technology
// is prohibited.

#include "../../common/gsCommon.h"
#include "../../common/gsCore.h"
#include "../../common/gsAvailable.h"
#include "../../webservices/AuthService.h"
#include "../sampleCommon.h"

static gsi_bool Setup()
{
	// First, run the GameSpy Availability Check before attempting to use any
	// GameSpy services. We provide this as a convenience function in our 
	// sample apps to wrap the calls you'll need to make.
	if (!AvailabilityCheck())
		return gsi_false;

	// Initialize the GameSpy SDK Core object for the AuthService and task
	// management.
	gsCoreInitialize();

	// Before AuthService login, you must first set your game identifiers.
	// This identifies your game so that your game metrics can be visualized on
	// your GameSpy Developer Dashboard.
	wsSetGameCredentials(SAMPLE_ACCESS_KEY, SAMPLE_GAME_ID, SAMPLE_SECRET_KEY);

	return gsi_true;
}

static gsi_bool Cleanup()
{
	// Here we shut down the GameSpy SDK Core object, which ensures that
	// internal objects get freed.
    CoreCleanup();

	// Keep the command-line prompt open so that you can view the output.
    WaitForUserInput();

	return gsi_true;
}

int test_main()
{	
	// First do the prerequisite initialization so that we'll be ready to
	// authenticate a player.
    printf("Doing the prerequisite setup and initialization\n");
	if (!Setup())
		return -1;

	// This is the meat of the sample app. You can also reference this sample
	// code on the GameSpy Developer Dashboard under 'Docs.' We provide this as
	// a convenience function in our sample apps to wrap the calls you'll need 
	// to make.
	printf("Authenticating the player\n");
	if (!SampleAuthenticatePlayer(SAMPLE_UNIQUENICK[0], SAMPLE_PASSWORD))
		return -1;

	// Shutdown the GameSpy SDK Core object, and then wait for user input 
	// before exiting the app.
	printf("Cleaning up after ourselves\n");
	if (!Cleanup())
		return -1;
    
	return 0;
}
