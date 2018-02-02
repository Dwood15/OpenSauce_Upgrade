///////////////////////////////////////////////////////////////////////////////
// File:	sampleCommon.c
// SDK:		GameSpy Authentication Service SDK
//
// Copyright (c) 2012 GameSpy Technology & IGN Entertainment, Inc. All rights
// reserved. This software is made available only pursuant to certain license
// terms offered by IGN or its subsidiary GameSpy Industries, Inc. Unlicensed
// use or use in a manner not expressly authorized by IGN or GameSpy Technology
// is prohibited.

#include "../common/gsCommon.h"
#include "../common/gsAvailable.h"
#include "../webservices/AuthService.h"
#include "sampleCommon.h"

#ifdef SAKE_SAMPLE
#include "../sake/sake.h"
#endif

#ifdef ATLAS_SAMPLE
#include "../sc/sc.h"
#endif

// You'll need to call the GameSpy Availability Check prior to starting up any 
// GameSpy SDKs to verify that GameSpy services are available and to handle any
// planned service outages. We provide this as a convenience function in our 
// sample apps to wrap the calls you'll need to make.
gsi_bool AvailabilityCheck()
{
	// Initialize a variable so we know when the check is complete.
    GSIACResult aResult = GSIACWaiting; 
	printf("  Checking availability...\n");
    GSIStartAvailableCheck(SAMPLE_GAMENAME);
	// Loop until the response arrives.
    while(aResult == GSIACWaiting)
    {
        aResult = GSIAvailableCheckThink();
        msleep(5);
    }

	// The Availability Check will only return GSIACUnavalable if GameSpy 
	// services no longer support your title.
    if (aResult == GSIACUnavailable)
    {
        printf("    Online Services for <insert your game name here> are no longer available\n");
        return gsi_false;
    }

	// GameSpy will set GSIACTemporarilyUnavailable during planned service 
	// outages so that users can be notified prior to actual connection
	// attempts to different services.
    if (aResult == GSIACTemporarilyUnavailable)
    {
        printf("    Online Services for <insert your game name here> are temporarily down for maintenance\n");
        return gsi_false;
    }

	printf("    Success - online services are available\n");

    return gsi_true;
}

// This callback is received from the response from wsLogin(). We provide this 
// as a convenience function in our sample apps to wrap the calls you'll need 
// to make.
void SampleAuthenticationCallback(GHTTPResult		httpResult, 
								  WSLoginResponse	* theResponse, 
								  void				* theUserData)
{
	// Here, we use theUserData to track login completion.
    int * loginComplete = (gsi_bool *)theUserData;

	// An HTTP-specific error was encountered, so print a message.
	if (httpResult != GHTTPSuccess)
	{
        printf("    Authentication failed, HTTP error: %s (%d)\n", ghttpResultString(httpResult), httpResult);
		*loginComplete = -1;
	}
    // Otherwise, if authentication failed for this user, check the 
	// mLoginResult in the WSLoginResponse for details.
	else if (theResponse->mLoginResult != WSLogin_Success)
	{
		printf("    Authentication failed, Login result: %s (%d)\n", wsLoginValueString(theResponse->mLoginResult), theResponse->mLoginResult);
		*loginComplete = -1;
	}
	// Otherwise, login was successful.
	else
	{
        printf("    Authentication success - we now have a certificate for player %s\n", theResponse->mCertificate.mUniqueNick);	
		*loginComplete = 1;

		// Save off the profileId, mCertificate, and mPrivateData, to use for 
		// ATLAS/Sake/D2G/Brigades authentication.
		memcpy(&certificate, &theResponse->mCertificate, sizeof(GSLoginCertificate));
		memcpy(&privateData, &theResponse->mPrivateData, sizeof(GSLoginPrivateData));
	}
}

// This call authenticates the player (based on their uniquenick and password) and
// 'thinks' (waits) until the callback function is triggered to verify
// successful authentication. We provide this as a convenience function in our 
// sample apps to wrap the calls you'll need to make.
gsi_bool SampleAuthenticatePlayer(const char * uniqueNick,
								  const char * password)
{
	// Create a variable to capture whether or not the request was
	// successfully dispatched.
	gsi_u32 result;

	// This will be set to '1' once our login callback function is triggered.
	int loginComplete = 0;

	// Authenticate the player profile via his uniquenick and password, using 
	// the default GameSpy namespace and partnercode (meaning he can use the
	// same GameSpy account as the one he used for IGN and most other GameSpy-
	// powered games).
	// NOTE: the cdkeyhash parameter is currently unused, so you can just pass
	// NULL.
    printf("  Attempting to authenticate our GameSpy profile...\n");
	result = wsLoginUnique(SAMPLE_GAME_ID, WSLogin_PARTNERCODE_GAMESPY, WSLogin_NAMESPACE_SHARED_UNIQUE, 
    uniqueNick, password, NULL, SampleAuthenticationCallback, &loginComplete);

	// The request could not be dispatched and return, so print an error.
	if (result != WSLogin_Success)
    {
        printf("  Failed setting up login call. Result: %d\n", result);
        return gsi_false;
    }

	// Keep 'thinking' (waiting) while we wait for the wsLogin() callback
	// function (i.e., the response).
    while (loginComplete == 0)
    {
		// Normally you would be doing other game stuff here; we just sleep
		// because we have nothing better to do.
        msleep(10); 
        gsCoreThink(0);
    }
	// We set loginComplete to -1 if the callback returns an error.
	if (loginComplete < 0) 
		return gsi_false;

	return loginComplete;
}

// When the authentication is complete we'll have a 'certificate' to pass to
// other SDK calls to identify the player and prove that he has successfully
// authenticated. We provide this as a convenience function in our sample apps
// to wrap the calls you'll need to make.
gsi_bool AuthSetup()
{
	// Initialize the GameSpy SDK Core object for the AuthService and task
	// management.
	gsCoreInitialize();

	// Before AuthService login you must first set your game identifiers.
	// This identifies you game so that your game metrics can be visualized on
	// your GameSpy Developer Dashboard.
	wsSetGameCredentials(SAMPLE_ACCESS_KEY, SAMPLE_GAME_ID, SAMPLE_SECRET_KEY);

	// Here we authenticate the player.
	if (!SampleAuthenticatePlayer(SAMPLE_UNIQUENICK[0], SAMPLE_PASSWORD))
		return gsi_false;

	return gsi_true;
}

// Shutdown the task management system used by AuthService and various other 
// SDKs. We provide this as a convenience function in our sample apps to wrap
// the calls you'll need to make.
void CoreCleanup()
{
	// Shutdown the GameSpy SDK Core (the task management system), which 
	// cancels any tasks in progress.
    gsCoreShutdown();

	// Wait for SDK Core shutdown (this should be instantaneous unless you have
	// multiple Cores running).
    while(gsCoreIsShutdown() == GSCore_SHUTDOWN_PENDING)
    {
		// Normally you would be doing other game stuff here; we just sleep 
		// because we have nothing better to do.
        gsCoreThink(0);
        msleep(5);
    }
}

#ifdef SAKE_SAMPLE
// Here we initialize the Sake object and set the game and profile credentials. 
// Note that for this sample program we've wrapped Sake functions like 
// sakeStartup() and sakeSetGame() inside of a convenience function named 
// SakeSetup(). Your game will need to make these calls separately for itself
// as we do below.
gsi_bool SakeSetup()
{
	SAKEStartupResult startupResult;

	// Here we initialize the Sake object, which will then be passed to
	// subsequent SDK calls.
	startupResult = sakeStartup(&sake);
	// If the Sake SDK could not be successfully started, print an error and 
	// return.
	if(startupResult != SAKEStartupResult_SUCCESS)
	{
		printf("  Failed starting up Sake: %d\n", startupResult);
		return gsi_false;
	}

	// Here we identify our game along with our game's secret key to verify 
	// authenticity.
	sakeSetGame(sake, SAMPLE_GAMENAME, SAMPLE_GAME_ID, SAMPLE_SECRET_KEY);
	
	// Here we identify the user with the profileid, certificate, and 
	// privatedata from the AuthService login callback function.
	sakeSetProfile(sake, certificate.mProfileId, &certificate, &privateData);

	return gsi_true;
}

const char * SakeFieldValueToString(SAKEField * field)
{
	static char buffer[64];
	SAKEFieldType type = field->mType;
	SAKEValue * value = &field->mValue;

	if(type == SAKEFieldType_BYTE)
		sprintf(buffer, "%d", (int)value->mByte);
	else if(type == SAKEFieldType_SHORT)
		sprintf(buffer, "%d", (int)value->mShort);
	else if(type == SAKEFieldType_INT)
		sprintf(buffer, "%d", value->mInt);
	else if(type == SAKEFieldType_FLOAT)
		sprintf(buffer, "%0.3f", value->mFloat);
	else if(type == SAKEFieldType_ASCII_STRING)
		return value->mAsciiString;
	else if(type == SAKEFieldType_UNICODE_STRING)
		UCS2ToAsciiString(value->mUnicodeString, buffer);
	else if(type == SAKEFieldType_BOOLEAN)
		return (value->mBoolean)?"true":"false";
	else if(type == SAKEFieldType_DATE_AND_TIME)
	{
		char * str = gsiSecondsToString(&value->mDateAndTime);
		str[strlen(str) - 1] = '\0';
		return str;
	}
	else if(type == SAKEFieldType_BINARY_DATA)
	{
		int i;
		int len = GS_MIN(value->mBinaryData.mLength, 8);
		for(i = 0 ; i < len ; i++)
			sprintf(buffer + (len*2), "%0X", value->mBinaryData.mValue[i]);
		buffer[len*2] = '\0';
	}
	else if(type == SAKEFieldType_INT64)
	{
		gsiInt64ToString(buffer, value->mInt64);
	}
	else
		return "      ERROR!!  Invalid value type set";

	return buffer;
}

// Here we iterate through the Fields of each Sake Record and print out the 
// values.
void DisplaySakeRecords(SAKEField	** records,
						int			numRecords, 
						int			numFields)
{
	SAKEField * field;
	int recordIndex;
	int fieldIndex;

	// Now we loop through the Records.
	for(recordIndex = 0 ; recordIndex < numRecords ; recordIndex++)
	{
		// We loop through the Fields.
		for(fieldIndex = 0 ; fieldIndex < numFields ; fieldIndex++)
		{
			field = &records[recordIndex][fieldIndex];

			// This shouldn't happen unless the Field is an ATLAS stat with no
			// default.
			if (field->mType == SAKEFieldType_NULL)
				printf("      %s is null\n", field->mName);
			else // Print the name and value of the Field.
				printf("      %s: %s\n", field->mName, SakeFieldValueToString(field));
		}
	}
}

// CopySakeRecordsFromResponseData() is a helper function to copy the records
// received from Sake for later use.
SAKEGetMyRecordsOutput * CopySakeRecordsFromResponseData(SAKEGetMyRecordsInput	* input,
														 SAKEGetMyRecordsOutput * output)
{
	SAKEGetMyRecordsOutput *myRecord = (SAKEGetMyRecordsOutput *) gsimalloc (sizeof(SAKEGetMyRecordsOutput));
	int i, j;

	if (output->mNumRecords >0)
	{
		// Now we can traverse the records.
		myRecord->mNumRecords = output->mNumRecords ;

		myRecord->mRecords = (SAKEField **) gsimalloc( sizeof(SAKEField *) * myRecord->mNumRecords);

		for (j=0; j<output->mNumRecords; j++)
		{
			myRecord->mRecords[j] = (SAKEField *) gsimalloc( sizeof(SAKEField) * input->mNumFields );
			for( i=0 ; i<input->mNumFields; i++)
			{
				myRecord->mRecords[j][i].mName = (char *) gsimalloc(strlen(output->mRecords[j][i].mName)+1);
				strcpy(myRecord->mRecords[j][i].mName , output->mRecords[j][i].mName);
				switch (output->mRecords[j][i].mType)
				{
					case SAKEFieldType_ASCII_STRING:
						
						myRecord->mRecords[j][i].mValue.mAsciiString = (char *) gsimalloc(strlen(output->mRecords[j][i].mValue.mAsciiString)+1);
						strcpy(myRecord->mRecords[j][i].mValue.mAsciiString , output->mRecords[j][i].mValue.mAsciiString);	
						break;
					case SAKEFieldType_INT:
						myRecord->mRecords[j][i].mValue.mInt = output->mRecords[j][i].mValue.mInt;
						break;
					default:
						printf("Unexpected type\n");
				}
			}
		}	
	}
	return  myRecord;
}

// FreeSakeRecords() is a helper function used to free Record memory allocated
// in a Sake search callback function (e.g., the sakeGetMyRecords() callback).
void FreeSakeRecords(SAKEGetMyRecordsOutput * records, 
					 int					numberOfFields)
{
	int i,j;

	for (i=0; i< records->mNumRecords; i++)
	{
		for (j= 0 ; j< numberOfFields; j++)
		{
			gsifree(records->mRecords[i][j].mName);
			// We only use ASCII for this sample.
			if (records->mRecords[i][j].mType == SAKEFieldType_ASCII_STRING)
				gsifree(records->mRecords[i][j].mValue.mAsciiString);
		}
		gsifree(records->mRecords[i]);
	}
	gsifree(records->mRecords);
}

// This callback function verifies whether or not the Record was successfully
// deleted.
static void DeleteRecordCallback(SAKE				sake, 
								 SAKERequest		request, 
								 SAKERequestResult	result, 
								 void				* inputData, 
								 void				* outputData, 
								 void				* userData)
{
	// This pointer tracks the status of the deletion.
	int * recordDeleted = (int *)userData;

	// Something went wrong and the Record wasn't deleted, so print an error.	
	if (result != SAKERequestResult_SUCCESS) 
	{
		printf("    There was an error deleting our Sake record. Result : %d", result);
		*recordDeleted = -1;
		return;
	}
	
	// The Record was deleted successfully, so print a message.
	*recordDeleted = 1;
	printf("    Success - our Sake record was deleted\n");

	// Avoid strict complier warnings.
	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(inputData);
	GSI_UNUSED(outputData);
}

// This function will delete a specific Record from a specific Table using 
// sakeDeleteRecord().
gsi_bool DeleteSakeRecord(char * tableName, int recordId)
{
	// This input object will just need the Table name and Record ID.
	SAKEDeleteRecordInput input; 
	SAKERequest request;  // This will get set to an object that tracks the 
						  // Sake request (this will also get passed back to us
						  // in the callback function).
	SAKEStartRequestResult startRequestResult;

	// This will be set to '1' once our callback function is triggered after
	// sakeDeleteRecord() (if successful).
	int recordDeleted = 0;

	// Here we set the input variables.
	input.mTableId = tableName;
	input.mRecordId = recordId;

	// Send out the DeleteRecord request (then we'll need to 'think' (wait) 
	// while we wait for the response).
	request = sakeDeleteRecord(sake, &input, DeleteRecordCallback, &recordDeleted);
	
	// Something went wrong, so print an error.
	if(!request)
	{
		// This gets the result of the most recent Sake call attempted.
		startRequestResult = sakeGetStartRequestResult(sake);
	    printf("  Failed to start Sake request: %d", startRequestResult);
		return gsi_false;
	}
	
	// Keep 'thinking' while we wait for the deleteRecord callback function.
    while (recordDeleted == 0)
    {
		// Normally you would be doing other game stuff here; we just sleep
		// because we have nothing better to do.
        msleep(10); 
		sakeThink();
    }

	// We set recordDeleted to -1 if the callback function returns an error.
	if (recordDeleted < 0)
		return gsi_false;
	
	return gsi_true;
}

// Shutdown Sake and the GameSpy SDK Core.
void SakeCleanup()
{
	// Shutdown Sake (freeing the 'Sake' object).
	sakeShutdown(sake);

	// Shutdown the SDK Core.
	CoreCleanup();
}
#endif // The above code is only needed for Sake samples.

#ifdef ATLAS_SAMPLE

// This callback function verifies whether or not the ATLAS Session was created
// without any issues; once this callback function is triggered (assuming
// success), you can obtain the ATLAS Session ID with scGetSessionId() in order 
// to then share it with the other players in the ATLAS Session.
static void createSessionCallback(SCInterfacePtr	theInterface,
								  GHTTPResult		httpResult,
								  SCResult			result,
								  void				* userData)
{
	// This pointer tracks the status of the ATLAS Session creation.
	int * sessionCreated = (int *)userData;

	// An HTTP-specific error was encountered, so print a message.
	if (httpResult != GHTTPSuccess)
	{
        printf("    Create Session failed, HTTP error: %s (%d)\n", ghttpResultString(httpResult), httpResult);
		*sessionCreated = -1;
	}
    // ATLAS Session creation failed, so print a message.
	else if (result != SCResult_NO_ERROR)
	{
		printf("    Create Session failed, Result: %d\n", result);
		*sessionCreated = -1;
	}
	else // The ATLAS Session was created successufully.
	{
		//const char * sessionId = scGetSessionId(theInterface)
		printf("    Success - the session has been created\n");
		*sessionCreated = 1;
	}

	GSI_UNUSED(theInterface);
}

// We use this callback function to verify whether or not the Report Intention 
// was set without issue; after this callback function has been triggered 
// (assuming success) you can obtain the player's connection ID with 
// scGetConnectionId() in order to then share this Intention with the other
// players in the ATLAS session (since they'll need to pass it to
// scReportSetPlayerData () when building their Session Report).
static void setReportIntentionCallback(SCInterfacePtr	theInterface,
									   GHTTPResult		httpResult,
									   SCResult			result,
									   void				* userData)
{
	// This pointer tracks the status of the Report Intention setting.
	int * reportIntentionSet = (int *)userData;

	// An HTTP-specific error was encountered, so print a message.
	if (httpResult != GHTTPSuccess)
	{
        printf("    Failed setting report intention, HTTP error: %s (%d)\n", ghttpResultString(httpResult), httpResult);
		*reportIntentionSet = -1;
	}
    // reportItentionSet() failed, so print a message.
	else if (result != SCResult_NO_ERROR)
	{
		printf("    Failed setting report intention, Result: %d\n", result);
		*reportIntentionSet = -1;
	}
	else // reportIntentionSet() was successful: we now have a connection ID to
		 // identify us in this ATLAS Session.
	{
		//const char * connectionId = scGetConnectionId(theInterface);
		printf("    Success - report intention has been set\n");
		*reportIntentionSet = 1;
	}

	GSI_UNUSED(theInterface);
}

// Initialize ATLAS and do all the prerequisite tasks necessary before creating
// and submitting a Stats Report. The host (playerIndex 0) will need to create
// the ATLAS Session, while the other players will need to set their ATLAS 
// Session ID to match the one created by the host. All players will set their
// Report Intention, specifying 'Authoritative' so that all Reports have equal
// weight in the arbitration process. 
//
// Note that players would normally go through this setup process separately in 
// their respective applications, but for the sake of having a full roster 
// we're having all players report from the same sample application.
gsi_bool AtlasReportingSetup(SCInterfacePtr	* statsInterface, 
							 int			playerIndex, 
							 char			* sessionId)
{
	SCResult result = SCResult_NO_ERROR;
	
	// We'll use these to track status on our asynchronous calls.
	int sessionCreated = 0;
	int reportIntentionSet = 0;

	// This will initialize our interface object, which we'll need to pass to
	// the rest of the SC calls.
	result = scInitialize(SAMPLE_GAME_ID, statsInterface);
	if (result != SCResult_NO_ERROR)
	{
		printf("  Failed initializing Atlas : %d\n", result);
		return gsi_false;
	}

	// Only the host needs to create an ATLAS Session; the clients will just
	// need to set their Session IDs.
	if (playerIndex == 0)
	{
		// We'll use the certificates array since we're authenticating multiple
		// players.
		// NOTE: You should NEVER use this array in your own code; We only use 
		// here because we're reporting multiple players in the same sample.
		memcpy(&certificates[playerIndex], &certificate, sizeof(GSLoginCertificate));
		memcpy(&privateDatas[playerIndex], &privateData, sizeof(GSLoginPrivateData));

		// Create the session, and then we can grab the SessionID 
		// (scGetSessionId()) to pass to the other players in the match. We 
		// pass 0 for the timeout parameter to wait indefinitely for the
		// callback function.
		printf("  The host is creating a session...\n");
		result = scCreateSession(*statsInterface, &certificates[playerIndex], &privateDatas[playerIndex], createSessionCallback, 0, &sessionCreated);  
		if (result != SCResult_NO_ERROR)
		{
			printf("  Failed creating session: %d\n", result);
			return gsi_false;
		}

		// Keep 'thinking' while we wait for the sessionCreated callback 
		// function.
		while (sessionCreated == 0)
		{
			// Normally you would be doing other game stuff here; we just sleep
			// because we have nothing better to do.
			msleep(10); 
			scThink(*statsInterface);
		}
		// We set sessionCreated to -1 if the callback function returns an error.
		if (sessionCreated < 0)
			return gsi_false;
	}
	else // Non-host players need to set their SessionId (after authenticating).
	{
		// Authenticate the player (we must shutdown and re-initialize the 
		// GameSpy SDK Core first since it doesn't handle authenticating 
		// multiple players).
		CoreCleanup();
		gsCoreInitialize();
		wsSetGameCredentials(SAMPLE_ACCESS_KEY, SAMPLE_GAME_ID, SAMPLE_SECRET_KEY);
		if (!SampleAuthenticatePlayer(SAMPLE_UNIQUENICK[playerIndex], SAMPLE_PASSWORD))
			return gsi_false;

		// Here we set the respective certificate data, which we'll need to 
		// pass to subsequent ATLAS calls.
		memcpy(&certificates[playerIndex], &certificate, sizeof(GSLoginCertificate));
		memcpy(&privateDatas[playerIndex], &privateData, sizeof(GSLoginPrivateData));

		// Here we set the Session ID, which the client will have received from
		// the host; this way the ATLAS system will know they are in the same
		// Session. Note that you must call scSetSessionId() prior to setting 
		// your Report Intention (scSetReportIntention()).
		scSetSessionId(*statsInterface, (gsi_u8 *)sessionId);
	}

	// All players need to set a report intention. We will have all reports be
	// Authoritative so that each player's reported data has equal weight in
	// arbitration. Note that the ConnectionID parameter should be NULL unless
	// the game supports late entry and this player was in this same match
	// previously, and we pass 0 for the timeout parameter to wait indefinitely
	// for the callback function.
	printf("  Setting report intention for player %d...\n", playerIndex+1);
	result = scSetReportIntention(*statsInterface, NULL, gsi_true, &certificates[playerIndex], &privateDatas[playerIndex], setReportIntentionCallback, 0, &reportIntentionSet);
	if (result != SCResult_NO_ERROR)
	{
		printf("  Failed setting report intention: %d\n", result);
		return gsi_false;
	}

	// Keep 'thinking' while we wait for the reportIntentionSet callback.
    while (reportIntentionSet == 0)
    {
		// Normally you would be doing other game stuff here; we just sleep
		// because we have nothing better to do.
        msleep(10); 
		scThink(*statsInterface);
    }
	// We set reportIntentionSet to -1 if the callback function returns an error.
	if (reportIntentionSet < 0)
		return gsi_false;

	return gsi_true;
}

// All we need to do before running ATLAS Queries is initialize the ATLAS SDK
// (we do not create an ATLAS Session or set a Report Intention or any of the
// other code specific to ATLAS Reporting).
gsi_bool AtlasQuerySetup(SCInterfacePtr * statsInterface)
{
	// This will initialize our interface object, which we'll need to pass to
	// the rest of the SC calls.
	SCResult result;
	result = scInitialize(SAMPLE_GAME_ID, statsInterface);
	if (result != SCResult_NO_ERROR)
	{
		printf("  Failed initializing Atlas : %d\n", result);
		return gsi_false;
	}
	return gsi_true;
}

// Iterate through the each stat for each player in the response (from the 
// player stats query callback function), printing out the names and values.
void DisplayAtlasPlayerRecords(SCPlayerStatsQueryResponse * response)
{
	unsigned int i,j; // This loops through players and stats.

	// Loop through each player.
	for(i= 0; i < response->mCategories->mPlayersCount; i++) 
	{ 
		// Loop through each stat for this player.
		for(j= 0; j < response->mCategories->mPlayers[i].mStatsCount; j++)
		{  
			// "<stat name>:<statvalue>" 
			printf("      %s:%s\n", response->mCategories->mPlayers[i].mStats[j].mName,
			       response->mCategories->mPlayers[i].mStats[j].mValue);  
		} 
		printf("      profileid:%d\n\n",response->mCategories->mPlayers[i].mProfileId);
	}
}

// Shutdown the ATLAS and GameSpy SDK Core objects, taking in the stats 
// interface pointer since that's what we need to free.
void AtlasCleanup(SCInterfacePtr statsInterface)
{
	// Shutdown ATLAS (freeing the interface object).
	scShutdown(statsInterface);

	// Shutdown the SDK Core object.
	CoreCleanup();
}
#endif // The code above is only needed for ATLAS samples.

// This function keeps the command line prompt open until the user responds.
void WaitForUserInput()
{
	fflush(stderr);
    printf("  Cleanup complete - press Enter to exit\r\n"); 
    fflush(stdout);
    getc(stdin);
}
