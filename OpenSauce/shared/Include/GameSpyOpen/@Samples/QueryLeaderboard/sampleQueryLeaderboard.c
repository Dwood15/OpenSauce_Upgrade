///////////////////////////////////////////////////////////////////////////////
// File:	sampleQueryLeaderboard.c
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
#include "../../sc/sci.h"
#include "../../sc/sc.h"
#include "../sampleCommon.h"
#include "atlas_atlasSamples_v2.h"

// This will make things easier, since every ATLAS request returns an immediate
// result that indicates whether or not the request was successfully
// dispatched.
#define CHECK_SCRESULT(result) if(result != SCResult_NO_ERROR) { return gsi_false; }

SCInterfacePtr statsInterface;

// Initialization for the Sake SDK.
static gsi_bool Setup()
{
	// Follow these helper functions into sampleCommon.c for details on
	// required setup steps.
	if (!AvailabilityCheck())
		return gsi_false;

	if (!AuthSetup())
		return gsi_false;

	if (!AtlasQuerySetup(&statsInterface))
		return gsi_false;

	// Now we're ready to build and submit our ATLAS reports.
	return gsi_true; 
}

// This callback function is registered later and invoked when the results of a
// Player Stats query request are returned.
static void OnPlayerStatsQueryComplete(const SCInterfacePtr			theInterface,
										GHTTPResult					httpResult,
										SCResult					result,
										gsi_char					* msg,
										int							msgLen,
										SCPlayerStatsQueryResponse	* response,
										void						* userData)
{
	// This pointer tracks the status of the report submission. 
	int * queryComplete = (int *)userData;

	// If an HTTP-level error was encountered with this request, display an
	// appropriate message.
	if (httpResult != GHTTPSuccess)
	{
        printf("    Stats query failed, HTTP error: %s (%d)\n", ghttpResultString(httpResult), httpResult);
		*queryComplete = -1;
	}

	// The stats query failed, so print a message.
	else if (result != SCResult_NO_ERROR)
	{
		printf("    Stats query failed, Result: %d\n", result);
		if (msgLen > 0)
			printf("      %s\n", msg);
		*queryComplete = -1;
	}
	else // The report was successfully submitted, so print a message.
	{
		printf("    Success - here are the results:\n\n");
		*queryComplete = 1;

		// Print out the records/stats in the response.
		DisplayAtlasPlayerRecords(response);

		// Cleanup: Make sure you you've copied the response object first if
		// you'll need it for further paging, because the memory will be 
		// deallocated when this call returns.
		scDestroyPlayerStatsQueryResponse(&response);
	}
	GSI_UNUSED(theInterface);
} 

// For this sample we search the top 5 players sorted by Win/Loss ratio, which
// we accomplish by invoking the 'PLAYER_LB' Ranked query (which we created on
// the ATLAS admin site), passing 5 for pagesize and 1 for pageindex. If we 
// wanted the 2nd page (again assuming a 5 records-per-page display) the code
// would be identical except pageindex would be 2.

// Note: We normally recommend that you query around 50 records at once so that
// you don't need to make a new query when the player pages up and down until
// he navigates past the 50th record.
static gsi_bool sampleQueryLeaderboard()
{
	// The version of the Ruleset in use, as chosen on the ATLAS Web Admin 
	// Panel.
	int rulesetVer = ATLAS_RULE_SET_VERSION;
	// This will be used to track the status of the report submission.
	int queryComplete = 0;
	// We need a NULL parameter list that will then be 'created' by the SDK.
	SCQueryParameterListPtr queryParamsList = NULL;
	// This will hold the query GUID that identifies the query we want to
	// invoke.
	char queryID[GS_GUID_SIZE]; 
	// This Player Query has two parameters: 'pageindex' and 'pagesize'.
	int queryParamCount = 2;
	
	// We want the first page of the leaderboard (e.g., the top <x> records
	// where x is the pagesize below).
	gsi_char * queryParam1Name = "pageindex"; 
	gsi_char * queryParam1Value = "1";  
	
	// The number of records per page, so that we will query the top 5 
	// records. If we wanted to query the 2nd page of this leaderboard 
	// (records 6-10) we would simply change the queryParam1Value above to "2".
	gsi_char * queryParam2Name = "pagesize";
	gsi_char * queryParam2Value = "5";  
	
	// Set the query Id to the appropriate query label, as defined in the
	// header file exported from the ATLAS Web Admin Panel.
	sprintf(queryID, ATLAS_Query_Player_Stats_PLAYER_LB);

	// Setup the query parameter list.
	CHECK_SCRESULT(scCreateQueryParameterList(&queryParamsList, queryParamCount));

	// Add each parameter to the list as a name/value pair.
	CHECK_SCRESULT(scAddQueryParameterToList(queryParamsList, queryParam1Name, queryParam1Value));
	CHECK_SCRESULT(scAddQueryParameterToList(queryParamsList, queryParam2Name, queryParam2Value));

	// Run the query, then wait for the callback; the ATLAS interface and login 
	// data are from previous initialization/authentication.
	printf("  Invoking the player stats query...\n");
	CHECK_SCRESULT(scRunPlayerStatsQuery(statsInterface, &certificate, &privateData, rulesetVer, queryID, queryParamsList, OnPlayerStatsQueryComplete, &queryComplete));

	// Cleanup: We no longer need the queryParamsList.
	CHECK_SCRESULT(scDestroyQueryParameterList(&queryParamsList));

	// Keep 'thinking' while we wait for the query complete callback.
	while (queryComplete == 0)
	{
		// Normally you would be doing other game stuff here; we just sleep 
		// because we have nothing better to do.
		msleep(10); 
		scThink(statsInterface);
	}

	// We set queryComplete to -1 if the callback returns an error.
	if (queryComplete < 0)
		return gsi_false;

	return gsi_true;
}

static gsi_bool Cleanup()
{
	// Shutdown ATLAS and the GameSpy SDK Core, which ensures internal objects 
	// get freed.
	AtlasCleanup(statsInterface);

	// Keep the command line prompt open so you can view the output.
	WaitForUserInput();

	return gsi_true;
}

int test_main()
{	
	// First, do the prerequisite initialization so that we'll be ready to 
	// authenticate a player.
    printf("Doing the prerequisite setup and initialization\n");
	if (!Setup())
		return -1;

	// This is the beef of the sample app. You can also reference this sample 
	// code on the developer portal 'Docs.'
	printf("Retrieve the first page of the player stats leaderboard\n");
	if (!sampleQueryLeaderboard())
		return -1;

	// Shutdown the GameSpy SDK Core object, and then we'll wait for user input
	// before exiting the app.
	printf("Cleaning up after ourselves\n");
	if (!Cleanup())
		return -1;
    
	return 0;
}
