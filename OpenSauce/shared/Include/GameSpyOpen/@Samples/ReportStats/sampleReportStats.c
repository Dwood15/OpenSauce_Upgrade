///////////////////////////////////////////////////////////////////////////////
// File:	sampleReportStats.c
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
#include "../../sc/sc.h"
#include "../sampleCommon.h"
#include "atlas_atlasSamples_v2.h"

// This will make things easier, since every ATLAS request returns an immediate
// result that indicates whether or not the request was successfully
// dispatched.
#define CHECK_SCRESULT(result) if(result != SCResult_NO_ERROR) { return gsi_false; }

#define NUM_PLAYERS 3

// Here we list the cars for the match.
const int CORVETTE = 0;
const int VIPER = 1;

// Here we list the tracks for the match.
const int DAYTONA_BEACH = 0;
const int MONACO = 1; 

typedef struct SampleMatchData
{
	char sessionId[SC_SESSION_GUID_SIZE];
	int track;
	gsi_u8 corvetteUses;
	gsi_u8 viperUses;

	SCGameStatus status;
} SampleMatchData;
static SampleMatchData matchData;

typedef struct SamplePlayerData
{
	GSLoginCertificate * certificate;
	GSLoginPrivateData * privateData;

	SCInterfacePtr statsInterface;
	
	SCReportPtr report;

	gsi_u8 connectionId[SC_CONNECTION_GUID_SIZE];
	
	SCGameResult result;
	int vehicleUsed;
	gsi_u64 raceTime;

// 20 is the max length for a GameSpy uniquenick.
	char uniquenick[20];

} SamplePlayerData;
static SamplePlayerData playerData[NUM_PLAYERS];

// Here we have a utility to get a random integer.
static int myGetRandomInt(int theMax)
{
	return (int)((float)rand()/RAND_MAX*theMax);
}

// The PlayFakeMatch() call generates match data such as race time and win/loss
// results for each player in the match so that we can then populate and submit
// our match report for ATLAS.
static void PlayFakeMatch()
{
	// This is for looping through players.
	int i, j;
	gsi_u64 bestRaceTime = 0;

	// This is a random seed for the random number generator.
	srand(current_time());

	printf("  Playing our fake match so that we'll have some match data to report to ATLAS\n");
	// Match (global) data: Set the track and initialize the vehicle uses,
	// which will increment for each player that uses that vehicle.
	matchData.track = myGetRandomInt(2);
	matchData.corvetteUses = 0;
	matchData.viperUses = 0;

	// Player data: Set the nick, racetime, and vehicle used by each player.
	for (i = 0; i < NUM_PLAYERS; i++)
	{
		strcpy(playerData[i].uniquenick, SAMPLE_UNIQUENICK[i]);
		playerData[i].vehicleUsed = myGetRandomInt(2);
		if (playerData[i].vehicleUsed == CORVETTE)
			matchData.corvetteUses++;
		else if (playerData[i].vehicleUsed == VIPER)
			matchData.viperUses++;
		playerData[i].raceTime = myGetRandomInt(300000);; // Between 0 and 5 minutes
														  // (we'll assume unit is
														  // milliseconds).
		if (bestRaceTime == 0 || playerData[i].raceTime < bestRaceTime)
			// Keep track of the fastest race time so that we can determine the winner.
			bestRaceTime = playerData[i].raceTime; 
	}

	// Loop through players once more to set results: the best race time 
	// wins and all others lose.
	for (j = 0; j < NUM_PLAYERS; j++)
	{
		if (playerData[j].raceTime == bestRaceTime)
			playerData[j].result = SCGameResult_WIN;
		else
			playerData[j].result = SCGameResult_LOSS;
	}

	// This status will be needed for the match report to show that the match
	// is finished so that the backend knows to close the session and commence
	// normalization and processing.
	matchData.status = SCGameStatus_COMPLETE;
}

static gsi_bool Setup()
{
	int i;

	// Follow these helper functions into sampleCommon.c for details on
	// required setup steps.
	if (!AvailabilityCheck())
		return gsi_false;

	if (!AuthSetup())
		return gsi_false;

	// ATLAS setup for host (player index 0).
	if (!AtlasReportingSetup(&playerData[0].statsInterface, 0, NULL))
		return gsi_false;

	// The host needs to hand off the session Id to the clients. Since we're 
	// 'cheating' by having all players report in the same app, we use the 
	// matchData and playerData[] structs to store the data that would normally
	// need to be shared across players.
	strcpy(matchData.sessionId, scGetSessionId(playerData[0].statsInterface));

	// Set our player-specific authentication credentials and identifier data 
	// for the host.
	strcpy((char *)playerData[0].connectionId, scGetConnectionId(playerData[0].statsInterface));
	playerData[0].certificate = &certificates[0];
	playerData[0].privateData = &privateDatas[0];

	// Iterate through the rest of the players to set them up for ATLAS
	// reporting (again, this would normally take place seperately in each
	// respective client application).
	for (i = 1; i < NUM_PLAYERS; i++)
	{
		if (!AtlasReportingSetup(&playerData[i].statsInterface, i, matchData.sessionId))
			return gsi_false;
		strcpy((char *)playerData[i].connectionId, scGetConnectionId(playerData[i].statsInterface));
		playerData[i].certificate = &certificates[i];
		playerData[i].privateData = &privateDatas[i];
	}

	// Now we proceed with the match, tracking the stats that we'll include in
	// our match report.
	PlayFakeMatch();

	// Now we're ready to build and submit our reports.
	return gsi_true;
}

// This callback verifies whether or not our report was submitted successfully 
// to the ATLAS backend. Once the backend receives each report, it will begin
// the normalization process to consolidate the reported data into one report,
// then processing will ensue, where the rules are run with the reported keys
// in order to update the stats.
static void sampleSubmitReportCallback(SCInterfacePtr	theInterface,
									   GHTTPResult		httpResult,
									   SCResult			result,
									   void				* userData)
{
	// This pointer tracks the status of the report submission.
	int * reportSubmitted = (int *)userData;

	// An HTTP-specific error was encountered prior to actual request
	// processing on the backend, so print a message.
	if (httpResult != GHTTPSuccess)
	{
        printf("    Create Session failed, HTTP error: %s (%d)\n", ghttpResultString(httpResult), httpResult);
		*reportSubmitted = -1;
	}
	// The report submission failed, so print an error.
	else if (result != SCResult_NO_ERROR)
	{
		printf("    Create Session failed, Result: %d\n", result);
		*reportSubmitted = -1;
	}
	else // The report was successfully submitted.
	{
		printf("    Success - report has been submitted\n");
		*reportSubmitted = 1;
	}

	GSI_UNUSED(theInterface);
}

// Create the ATLAS report and populate it with the dummy match data we
// generated in PlayFakeMatch.
static gsi_bool SampleReportStats()
{
	// This will loop through players.
	int i, j;
	
	// This loop will *not* be needed in your game code, since only the local
	// player will submit his report. We need it since we're having all players
	// report from the same app for ease of demonstrating a sample session.
	for (i = 0; i < NUM_PLAYERS; i++)
	{
		// This will be used to track the status of the report submission.
		int reportSubmitted = 0;
		
		// Initialize the report object, which will then be passed to the 
		// scReport() functions as we populate it. The ruleset version is the
		// same as the one under Ruleset Management on the ATLAS Web Admin 
		// Panel, and for this sample teams do not apply, so we pass 0 for 
		// numTeams.
		CHECK_SCRESULT(scCreateReport(&playerData[i].statsInterface, ATLAS_RULE_SET_VERSION, NUM_PLAYERS, 0, &playerData[i].report));

		// Now we need to start with the global (match-wide) data, starting
		// with the Begin call to start the section.
		CHECK_SCRESULT(scReportBeginGlobalData(playerData[i].report));

		// Now we use the scReportAdd() functions to write all of our game keys
		// that have non-zero values. The key Ids are those from the ATLAS Web
		// Admin Panel, and are included in the header file 
		// (atlas_atlasSamples_v2.h) that you can download from the ATLAS Web
		// Admin Panel under Ruleset Management.
		CHECK_SCRESULT(scReportAddByteValue(playerData[i].report, ATLAS_KEY_USED_VEHICLE_1_G, matchData.corvetteUses));
		CHECK_SCRESULT(scReportAddByteValue(playerData[i].report, ATLAS_KEY_USED_VEHICLE_2_G, matchData.viperUses));
		
		// Check which track was played and set the corredsponding 
		// USED_TRACK_<x> key.
		if (matchData.track == DAYTONA_BEACH)
		{
			CHECK_SCRESULT(scReportAddByteValue(playerData[i].report, ATLAS_KEY_USED_TRACK_1, 1));
		}
		else if (matchData.track == MONACO)
		{
			CHECK_SCRESULT(scReportAddByteValue(playerData[i].report, ATLAS_KEY_USED_TRACK_2, 1));
		}

		// Now that we've finished the global section, we move on to the 
		// player section. Again, we start with the Begin call to start this
		// section.
		CHECK_SCRESULT(scReportBeginPlayerData(playerData[i].report));

		// Now we'll iterate through each player (including us) to set his data
		// from the match.
		for (j = 0; j < NUM_PLAYERS; j++)
		{
			// We need another Begin call to start each player's section.
			CHECK_SCRESULT(scReportBeginNewPlayer(playerData[i].report));

			// You must set player data for all players in the match; this is 
			// why players will need to share their connection Id and 
			// certificate. We pass 0 for the team Id, since our match doesn't
			// involve teams, and the AuthData is currently unused, so you can
			// just pass NULL. For the result, we pass the win/loss result as
			// determined in PlayFakeMatch.
			CHECK_SCRESULT(scReportSetPlayerData(playerData[i].report, j, playerData[j].connectionId, 0, playerData[j].result, 
								playerData[j].certificate->mProfileId, playerData[j].certificate, NULL));

			CHECK_SCRESULT(scReportAddStringValue(playerData[i].report, ATLAS_KEY_PLAYER_NAME_P, playerData[j].uniquenick));

			// Check which vehicle this player drove and set the corredsponding
			// USED_VEHICLE_<x> key.
			if (playerData[j].vehicleUsed == CORVETTE)
			{
				CHECK_SCRESULT(scReportAddByteValue(playerData[i].report, ATLAS_KEY_USED_VEHICLE_1_P, 1));
			}
			else if (playerData[j].vehicleUsed == VIPER)
			{
				CHECK_SCRESULT(scReportAddByteValue(playerData[i].report, ATLAS_KEY_USED_VEHICLE_2_P, 1));
			}
			
			// We pass the track as a player key as well as game key, since it
			// is used as a qualifier in Player rules (rules that output Player
			// stats) as well as Game rules (found in the Ruleset on the ATLAS 
			// Web Admin Panel).
			if (matchData.track == DAYTONA_BEACH)
			{
				CHECK_SCRESULT(scReportAddByteValue(playerData[i].report, ATLAS_KEY_USED_TRACK_1, 1));
			}
			else if (matchData.track == MONACO)
			{
				CHECK_SCRESULT(scReportAddByteValue(playerData[i].report, ATLAS_KEY_USED_TRACK_2, 1));
			}

			// Set the player's race time; this will end up hitting a 'Minimum'
			// rule that we set up on the ATLAS Web Admin Panel to compare this
			// race time to the player's existing BEST_RACE_TIME stat, 
			// replacing the best race time value if it's been beaten by this 
			// new time.
			CHECK_SCRESULT(scReportAddInt64Value(playerData[i].report, ATLAS_KEY_RACE_TIME_P, playerData[j].raceTime));
		}

		// Now that we've looped through all the players and added all of their
		// data, we Begin the Team section (this Begin needs to be called even 
		// when you have no actual data to report).
		CHECK_SCRESULT(scReportBeginTeamData(playerData[i].report));

		// Now that we've added all of our match data we can end the report.
		CHECK_SCRESULT(scReportEnd(playerData[i].report, gsi_true, matchData.status)); 

		// Having completed the report, it's now time to submit it so that it 
		// can be processed and update the game and player-wide stats
		// accordingly. Again, we pass 0 for the timeout parameter to wait 
		// indefinitely for the callback, and we reiterate here that our report
		// will be Authoritative (i.e., this report must match what we passed 
		// in the scSetReportIntention() call).
		printf("  Submitting the ATLAS report for player %d...\n", i+1);
		CHECK_SCRESULT(scSubmitReport(playerData[i].statsInterface, playerData[i].report, gsi_true, playerData[i].certificate,
								playerData[i].privateData, sampleSubmitReportCallback, 0, &reportSubmitted)); 

		// Keep 'thinking' while we wait for the submit report callback.
		while (reportSubmitted == 0)
		{
			// Normally you would be doing other game stuff here; we just sleep
			// because we have nothing better to do.
			msleep(10); 
			scThink(playerData[i].statsInterface);
		}

		// We set reportSubmitted to -1 if the callback returns an error.
		if (reportSubmitted < 0) 
			return gsi_false;
	}
	return gsi_true; 
	// Our reports for the match session have been submitted successfully, so 
	// we can now go to the Debug Viewer on the ATLAS Web Admin Panel to check
	// on it as it goes through normalization and processing.
}

static gsi_bool Cleanup()
{
	// Because we 'cheated' and reported all players from the same sample app,
	// we need to iterate through each player to shutdown his specific ATLAS
	// interface.
	int i;
	for (i = 0; i < NUM_PLAYERS; i++)
	{
		// Shutdown ATLAS and the GameSpy SDK Core object, which ensures 
		// internal objects get freed.
		AtlasCleanup(playerData[i].statsInterface);
	}

	// Keep the command line prompt open so that you can view the output.
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

	// This is the beef of the sample app. You can also reference this sample
	// code on the developer portal 'Docs.'
	printf("Reporting stats for our sample match\n");
	if (!SampleReportStats())
		return -1;

	// Shutdown the GameSpy SDK Core object, and then we'll wait for user input
	// before exiting the app.
	printf("Cleaning up after ourselves\n");
	if (!Cleanup())
		return -1;
    
	return 0;
}
