///////////////////////////////////////////////////////////////////////////////
// File:	sampleAuth.c
// SDK:		GameSpy Authentication Service SDK
//
// Copyright (c) IGN Entertainment, Inc.  All rights reserved.  
// This software is made available only pursuant to certain license terms offered
// by IGN or its subsidiary GameSpy Industries, Inc.  Unlicensed use or use in a 
// manner not expressly authorized by IGN or GameSpy is prohibited.

#include "sampleCommon.h"

// The availability check needs to be called prior to starting up any of the SDKs - this
// allows the game to gracefully handle planned outages
gsi_bool AvailabilityCheck()
{
    GSIACResult aResult = GSIACWaiting; 
	LOGI("  Checking availability...\n");
    GSIStartAvailableCheck(SAMPLE_GAMENAME);
    while(aResult == GSIACWaiting)
    {
        aResult = GSIAvailableCheckThink();
        msleep(5);
    }

	// This result will only get set if GameSpy services are no longer supported by your title
    if (aResult == GSIACUnavailable)
    {
        LOGI("    Online Services for <insert your game name here> are no longer available\n");
        return gsi_false;
    }

	// GameSpy will set this result during planned outages so that users can be notified immediately
	// (prior to actual connection attempts to different services where you would have to wait for a timeout)
    if (aResult == GSIACTemporarilyUnavailable)
    {
        LOGI("    Online Services for <insert your game name here> are temporarily down for maintenance\n");
        return gsi_false;
    }

	LOGI("    Success - online services are available\n");

    return gsi_true;
}

// triggered when we receive a response from AuthService after our wsLogin call 
void SampleAuthenticationCallback(GHTTPResult httpResult, WSLoginResponse * theResponse, void * theUserData)
{
	// we use theUserData to track login completion
    int * loginComplete = (gsi_bool *)theUserData;

	// an HTTP specific error was encountered (prior to actual request processing on the backend)
	if (httpResult != GHTTPSuccess)
	{
        LOGI("    Authentication failed, HTTP error: %s (%d)\n", ghttpResultString(httpResult), httpResult);
		*loginComplete = -1;
	}
    // authentication failed for this user, check the mLoginResult in the WSLoginResponse for details
	else if (theResponse->mLoginResult != WSLogin_Success)
	{
		LOGI("    Authentication failed, Login result: %s (%d)\n", wsLoginValueString(theResponse->mLoginResult), theResponse->mLoginResult);
		*loginComplete = -1;
	}
	else // login success
	{
        LOGI("    Authentication success - we now have a certificate for player %s\n", theResponse->mCertificate.mUniqueNick);
		*loginComplete = 1;
		// save off the profileId, mCertificate, and mPrivateData, to be used for Atlas/Sake/D2G/Brigades 
		// authentication
		memcpy(&certificate, &theResponse->mCertificate, sizeof(GSLoginCertificate));
		memcpy(&privateData, &theResponse->mPrivateData, sizeof(GSLoginPrivateData));
	}
}

// Authenticates the player (based on UniqueNick and password) and 'thinks' until the callback is 
// triggered to verify successful authentication
gsi_bool SampleAuthenticatePlayer(const char * uniqueNick, const char * password)
{
	gsi_u32 result;   // for the (local) return from the wsLogin functions

	int loginComplete = 0; // this will be set to 1 once our login callback is triggered

	// Authenticate the player profile via his uniquenick and password, using the default GameSpy 
	// namespace and partnercode (meaning he can use the same GameSpy account as that used for IGN 
	// and most other GameSpy-Powered games)
	// *Note: the cdkeyhash parameter is currently unused, so you can just pass NULL
    LOGI("  Attempting to authenticate our GameSpy profile...\n");
	result = wsLoginUnique(SAMPLE_GAME_ID, WSLogin_PARTNERCODE_GAMESPY, WSLogin_NAMESPACE_SHARED_UNIQUE, 
    uniqueNick, password, NULL, SampleAuthenticationCallback, &loginComplete);

	if (result != WSLogin_Success) // local issue prior to request actually going out
    {
        LOGI("  Failed setting up login call. Result: %d\n", result);
        return gsi_false;
    }

	// keep 'thinking' while we wait for the wsLogin callback
    while (loginComplete == 0)
    {
		// normally you would be doing other game stuff here; we just sleep because we have nothing 
		// better to do
        msleep(10); 
        gsCoreThink(0);
    }

	if (loginComplete < 0) // we set loginComplete to -1 if the callback returns an error
		return gsi_false;

	return loginComplete;
}

// when this is complete we'll have a 'certificate' to pass to other SDK calls to identify the player
// and prove he has successfully authenticated
gsi_bool AuthSetup()
{
	// Initialize SDK core/common objects for the auth service and task management  
	gsCoreInitialize();

	// Before auth service login you must first set your game identifiers
	// (this is used to isolate the title for metric tracking, and the metrics are
	// visualized on the developer portal Dashboard)
	wsSetGameCredentials(SAMPLE_ACCESS_KEY, SAMPLE_GAME_ID, SAMPLE_SECRET_KEY);

	// authenticate the player
	if (!SampleAuthenticatePlayer(SAMPLE_UNIQUENICK[0], SAMPLE_PASSWORD))
		return gsi_false;

	return gsi_true;
}

// shutdown the task management system used by autheservice and various other SDKs
void CoreCleanup()
{
	// shutdown core (the task management system), which cancels any tasks in progress
    gsCoreShutdown();

    // Wait for core shutdown (should be instantaneous unless you have multiple cores)
    while(gsCoreIsShutdown() == GSCore_SHUTDOWN_PENDING)
    {
		// normally you would be doing other game stuff here; we just sleep because we have nothing 
		// better to do
        gsCoreThink(0);
        msleep(5);
    }
}


// callback to verify whether the session was created without issue; once this is triggered (assuming
// success) you can obtain the session ID with scGetSessionId, in order to then share it with the other
// players in the match
static void createSessionCallback(SCInterfacePtr theInterface,
						   GHTTPResult httpResult,
                           SCResult result,
						   void * userData)
{
	// pointer to track the status of the session create
	int * sessionCreated = (int *)userData;

	// an HTTP specific error was encountered (prior to actual request processing on the backend)
	if (httpResult != GHTTPSuccess)
	{
        LOGI("    Create Session failed, HTTP error: %s (%d)\n", ghttpResultString(httpResult), httpResult);
		*sessionCreated = -1;
	}
    // session creation failed
	else if (result != SCResult_NO_ERROR)
	{
		LOGI("    Create Session failed, Result: %d\n", result);
		*sessionCreated = -1;
	}
	else // success - session created
	{
		//const char * sessionId = scGetSessionId(theInterface)
		LOGI("    Success - the session has been created\n");
		*sessionCreated = 1;
	}

	GSI_UNUSED(theInterface);
}

// callback to verify whether the report intention was set without issue; after this has been triggered 
// (assuming success) you can obtain the player's connection ID with scGetConnectionId, in order to then 
// share this with the other players in the match (since they'll need to pass it to scReportSetPlayerData 
// when building their match report)
static void setReportIntentionCallback(SCInterfacePtr theInterface,
						   GHTTPResult httpResult,
                           SCResult result,
						   void * userData)
{
	// pointer to track the status of the report intention setting
	int * reportIntentionSet = (int *)userData;

	// an HTTP specific error was encountered (prior to actual request processing on the backend)
	if (httpResult != GHTTPSuccess)
	{
        LOGI("    Failed setting report intention, HTTP error: %s (%d)\n", ghttpResultString(httpResult), httpResult);
		*reportIntentionSet = -1;
	}
    // set report intention failed
	else if (result != SCResult_NO_ERROR)
	{
		LOGI("    Failed setting report intention, Result: %d\n", result);
		*reportIntentionSet = -1;
	}
	else // success - we now have a connection ID to identify us in this match
	{
		//const char * connectionId = scGetConnectionId(theInterface);
		LOGI("    Success - report intention has been set\n");
		*reportIntentionSet = 1;
	}

	GSI_UNUSED(theInterface);
}

// Initialize Atlas and do all the prerequisite tasks necessary before creating/submitting a stats
// report. The host (playerIndex 0) will need to create the session, while the other players will need
// to set their session ID to match that created by the host. Then all players will set their report
// intention, specifying 'Authoritative' so that all reports have equal weight in the arbitration process.
//
// Note that players would normally go through this setup process separately in their respective 
// applications, but for the sake of having a full roster we're having all players report from the same
// sample app.
gsi_bool AtlasReportingSetup(SCInterfacePtr * statsInterface, int playerIndex, char * sessionId)
{
	SCResult result = SCResult_NO_ERROR;
	
	// we'll use these to track status on our asynchronous calls
	int sessionCreated = 0;
	int reportIntentionSet = 0;

	// this will initialize our interface object, which we'll need to pass to the rest of the SC calls
	result = scInitialize(SAMPLE_GAME_ID, statsInterface);
	if (result != SCResult_NO_ERROR)
	{
		LOGI("  Failed initializing Atlas : %d\n", result);
		return gsi_false;
	}

	// only the host needs to create a session; the clients will just need to set their session IDs
	if (playerIndex == 0)
	{
		// we'll use the certificates array since we're authenticating multiple players
		// (but seriously, don't try this at home!)
		memcpy(&certificates[playerIndex], &certificate, sizeof(GSLoginCertificate));
		memcpy(&privateDatas[playerIndex], &privateData, sizeof(GSLoginPrivateData));

		// create the session, then we can grab the sessionID (scGetSessionId) to pass to the other players
		// in the match. we pass 0 for the timeout parameter to wait indefinitely for the callback 
		LOGI("  The host is creating a session...\n");
		result = scCreateSession(*statsInterface, &certificates[playerIndex], &privateDatas[playerIndex], createSessionCallback, 0, &sessionCreated);  
		if (result != SCResult_NO_ERROR)
		{
			LOGI("  Failed creating session: %d\n", result);
			return gsi_false;
		}

		// keep 'thinking' while we wait for the create session callback
		while (sessionCreated == 0)
		{
			// normally you would be doing other game stuff here; we just sleep because we have nothing 
			// better to do
			msleep(10); 
			scThink(*statsInterface);
		}
		if (sessionCreated < 0) // we set sessionCreated to -1 if the callback returns an error
			return gsi_false;
	}
	else // non-host players need to set their sessionId (after authenticating)
	{
		// authenticate the player (we must kill and re-init the core first since it doesn't handle
		// authenticating multiple players)
		CoreCleanup();
		gsCoreInitialize();
		wsSetGameCredentials(SAMPLE_ACCESS_KEY, SAMPLE_GAME_ID, SAMPLE_SECRET_KEY);
		if (!SampleAuthenticatePlayer(SAMPLE_UNIQUENICK[playerIndex], SAMPLE_PASSWORD))
			return gsi_false;

		// set the respective certificate data, which we'll need to pass to subsequent ATLAS calls
		memcpy(&certificates[playerIndex], &certificate, sizeof(GSLoginCertificate));
		memcpy(&privateDatas[playerIndex], &privateData, sizeof(GSLoginPrivateData));

		// set the session ID, which the client will have received from the host; this way the ATLAS 
		// system will know they are in the same match. note that this must be called prior to setting
		// report intention (scSetReportIntention)
		scSetSessionId(*statsInterface, (gsi_u8 *)sessionId);
	}

	// all players need to set a report intention - we will have all reports be 'Authoritative' so that 
	// each player's reported data has equal weight in arbitration. note that the ConnectionID parameter 
	// should be NULL unless the game supports late entry and this player was in this same match previously,
	// and we pass 0 for the timeout parameter to wait indefinitely for the callback 
	LOGI("  Setting report intention for player %d...\n", playerIndex+1);
	result = scSetReportIntention(*statsInterface, NULL, gsi_true, &certificates[playerIndex], &privateDatas[playerIndex], setReportIntentionCallback, 0, &reportIntentionSet);
	if (result != SCResult_NO_ERROR)
	{
		LOGI("  Failed setting report intention: %d\n", result);
		return gsi_false;
	}

	// keep 'thinking' while we wait for the set report intention callback
    while (reportIntentionSet == 0)
    {
		// normally you would be doing other game stuff here; we just sleep because we have nothing 
		// better to do
        msleep(10); 
		scThink(*statsInterface);
    }
	if (reportIntentionSet < 0) // we set reportIntentionSet to -1 if the callback returns an error
		return gsi_false;

	return gsi_true;
}

// all we need to do before running Atlas Queries is initialize the SDK (we do not create a session or
// set report intention or any of the stuff specific to Atlas Reporting
gsi_bool AtlasQuerySetup(SCInterfacePtr * statsInterface)
{
	// this will initialize our interface object, which we'll need to pass to the rest of the SC calls
	SCResult result;
	result = scInitialize(SAMPLE_GAME_ID, statsInterface);
	if (result != SCResult_NO_ERROR)
	{
		LOGI("  Failed initializing Atlas : %d\n", result);
		return gsi_false;
	}
	return gsi_true;
}

// iterate through the each stat for each player in the response (from the player stats query callback),
// printing out the names and values
void DisplayAtlasPlayerRecords(SCPlayerStatsQueryResponse * response)
{
	unsigned int i,j; // for looping through players/stats

	// loop through each player
	for(i= 0; i < response->mCategories->mPlayersCount; i++) 
	{ 
		// loop through each stat for this player
		for(j= 0; j < response->mCategories->mPlayers[i].mStatsCount; j++)
		{  
			// "<stat name>:<statvalue>" 
			LOGI("      %s:%s\n", response->mCategories->mPlayers[i].mStats[j].mName,
			       response->mCategories->mPlayers[i].mStats[j].mValue);  
		} 
		LOGI("      profileid:%d\n\n",response->mCategories->mPlayers[i].mProfileId);
	}
}

// shutdown ATLAS and Core, taking in the stats interface pointer since that's what needs to be freed
void AtlasCleanup(SCInterfacePtr statsInterface)
{
	// shutdown ATLAS (freeing the interface object)
	scShutdown(statsInterface);

	// shutdown core
	CoreCleanup();
}
