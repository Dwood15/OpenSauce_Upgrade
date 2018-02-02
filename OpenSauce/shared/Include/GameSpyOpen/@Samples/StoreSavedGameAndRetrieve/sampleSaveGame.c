///////////////////////////////////////////////////////////////////////////////
// File:	sampleSaveGame.c
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
#include "../../sake/sake.h"
#include "../sampleCommon.h"

// This is dummy data for populating bit mask when creating fake game data to
// save.
const gsi_u8 hasShoes = 1;                   // 2^0  (00000001)
const gsi_u8 hasAxe = 2;                     // 2^1  (00000010)
const gsi_u8 hasShield = 4;                  // 2^2  (00000100)
const gsi_u8 hasLeatherArmor = 8;            // 2^3  (00001000)
const gsi_u8 hasDragonScaleArmor = 16;       // 2^4  (00010000)
const gsi_u8 hasMount = 32;                  // 2^5  (00100000)
const gsi_u8 hasMap = 64;                    // 2^6  (01000000)
const gsi_u8 hasSwordOfThousandTruths = 128; // 2^7  (10000000)

int newRecordId = 0; // This will be the Sake RecordId for the new record we're
					 // creating).
char * SavedGameTable = "SavedGameData"; // This will be the name of the Sake
										 // table we're using to store saved
										 // games.

// This populates the fields passed with arbitrary dummy data (pretending we're 
// saving our status in an RPG game).
static void CreateGameData(SAKEField * fields)
{
	int fieldIndex = 0;
	
	// This is a byte-sized bit mask to store which pieces of equipment our
	// character has.
	gsi_u8 equipment = hasShoes + hasAxe + hasLeatherArmor + hasMap;

	// Populate the name as it appears on the Sake Web Admin Panel, the type, 
	// and the value for each of the fields for the record that we're creating.
	fields[fieldIndex].mName = "Level";
	fields[fieldIndex].mType = SAKEFieldType_INT;
	fields[fieldIndex].mValue.mInt = 28;
	fieldIndex++;
	fields[fieldIndex].mName = "Class";
	fields[fieldIndex].mType = SAKEFieldType_ASCII_STRING;
	fields[fieldIndex].mValue.mAsciiString = "Rogue";
	fieldIndex++;
	fields[fieldIndex].mName = "Location";
	fields[fieldIndex].mType = SAKEFieldType_ASCII_STRING;
	fields[fieldIndex].mValue.mAsciiString = "Shadow Rift";
	fieldIndex++;
	fields[fieldIndex].mName = "Equipment";
	fields[fieldIndex].mType = SAKEFieldType_BYTE;
	fields[fieldIndex].mValue.mByte = equipment;
}

static gsi_bool Setup()
{
	// Follow these helper functions into sampleCommon.c for details on
	// required setup steps.
	if (!AvailabilityCheck())
		return gsi_false;

	if (!AuthSetup())
		return gsi_false;

	if (!SakeSetup())
		return gsi_false;

	return gsi_true;
}

static gsi_bool Cleanup()
{
	// In order to keep the sample table clean we'll delete the saved game
	// record that we created.
	printf("  Deleting our saved game record...\n");
	if (!DeleteSakeRecord(SavedGameTable, newRecordId))
		return gsi_false;

	// Here we shutdown Sake and the GameSpy SDK Core, which ensures that 
	// internal objects get freed.
	SakeCleanup();

	// Keep the command line prompt open so that you can view the output.
	WaitForUserInput();

	return gsi_true;
}

// This callback is triggered when we receive a response from Sake after our 
// CreateRecord() call.
static void SampleStoreSavedGameCallback(SAKE				sake, 
										 SAKERequest		request, 
										 SAKERequestResult	result, 
										 void				* inputData, 
										 void				* outputData, 
										 void				* userData)
{
	// Convert the void pointer outputData into the appropriate structure.
	SAKECreateRecordOutput * output = (SAKECreateRecordOutput *)outputData;
	
	// We passed this int pointer for userData, so set it accordingly.
	int * recordId = (int *)userData; 

	// Something went wrong and the record wasn't created, so print a message.
	if (result != SAKERequestResult_SUCCESS) 
	{
		if (result == SAKERequestResult_RECORD_LIMIT_REACHED)
			printf("    This user already has a saved game (and the table has a 'limit per owner' of 1)");   
		else
			printf("    There was an error creating the Sake record: %d", result);
		*recordId = -1; // we'll know in our think loop that there was an error if it's -1
		return;
	}
	
	// Set the RecordId of our newly created record.
	*recordId = output->mRecordId; 
	printf("    Success - our new record has recordid %d\n", output->mRecordId);

	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(inputData);
	GSI_UNUSED(userData);
}

// This callback is triggered when we receive a response from Sake after our
// GetMyRecords() call.
static void SampleRetrieveSavedGameCallback(SAKE				sake, 
											SAKERequest			request, 
											SAKERequestResult	result, 
											void				* inputData, 
											void				* outputData, 
											void				* userData)
{
	// Convert the void pointers into the appropriate structures.
	SAKEGetMyRecordsInput * input = (SAKEGetMyRecordsInput *)inputData;
	SAKEGetMyRecordsOutput * output = (SAKEGetMyRecordsOutput *)outputData;

	// We need to set this to 1 upon success so that we'll know that we can
	// stop thinking.
	int * dataRetrieved = (int *)userData; 

	// Something went wrong and no record was retrieved, so print an error.
	if (result != SAKERequestResult_SUCCESS) 
	{
		printf("    There was an error retrieving our Sake record: %d", result);
		*dataRetrieved = -1;
		return;
	}
	
	// We got our record, now let's print it out.
	*dataRetrieved = 1;
	printf("    Success - our Sake record was retrieved: \n");
	DisplaySakeRecords(output->mRecords, output->mNumRecords, input->mNumFields);

	// Avoid strict complier warnings.
	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(userData);
}

// Use the sakeCreateRecord() call to store data in the Sake cloud.
// Note: If the player already had a record, which you can determine with the
// sakeGetMyRecords() call, you would instead use the sakeUpdateRecord() call.
static gsi_bool SampleStoreSavedGame()
{
	// This will store our fields for the record we want to create.
	SAKECreateRecordInput recordData;
	// This will get set to an object that tracks the Sake request (and will 
	// also get passed back to us in the callback) to identify the fields in 
	// the Sake table and set values for each field.
	SAKERequest request; 
	SAKEField fields[4];

	// Create some dummy data to act as our 'saved game' and populate the 
	// fields with this data.
	CreateGameData(fields);
	
	// Now that the fields have data we can set up the recordData.
	recordData.mFields = fields;
	recordData.mNumFields = 4;

	// The table (along with the fields) must first be created on the Sake Web
	// Admin Panel.
	recordData.mTableId = SavedGameTable;

	// Send off the CreateRecord request and then wait for a response via the
	// callback.
	// We pass 'newRecordId' for the userData so that we can set this in the
	// callback.
	printf("  Creating Sake record...\n");
	request = sakeCreateRecord(sake, &recordData, SampleStoreSavedGameCallback, &newRecordId);
	
	// The request could not be dispatched to the Sake server.
	if(!request) 
	{
		// This gets the result of the most recent Sake request that has been
		// made.
		SAKEStartRequestResult startRequestResult = sakeGetStartRequestResult(sake);
	    printf("  Failed to start Sake request: %d", startRequestResult);
		return gsi_false;
	}

	// Here we keep 'thinking' while we wait for the create record callback
	// (SampleStoreSavedGameCallback).
    while (newRecordId == 0)
    {
		// Normally you would be doing other game stuff here; we just sleep
		// because we have nothing better to do.
        msleep(10); 
		sakeThink();
    }
	// We set newRecordId to -1 if the callback returns an error.
	if (newRecordId < 0) 
		return gsi_false;
	
	return gsi_true;
}

static gsi_bool SampleRetrieveSavedGame()
{
	// This input object will store the fields for which we want to retrieve
	// values.
	SAKEGetMyRecordsInput myRecordToRetrieve;
	SAKERequest request; // This will be set to an object that tracks the Sake
						 // request (and will also be passed back to us in the
						 // callback).
	SAKEStartRequestResult startRequestResult;
	
	// This will be set to 1 once our callback is triggered after the
	// sakeGetMyRecords() call, if it was successful.
	int savedGameDataRetrieved = 0;

	// This will be set to 1 once our callback is triggered after the
	// sakeGetMyRecords() call, if it was successful.
	char *fieldNames[] = {"ownerid", "Level", "Class", "Location", "Equipment"};
	myRecordToRetrieve.mTableId = SavedGameTable; // This is the same table
												  // from the Sake Web Admin
												  // Panel.
	myRecordToRetrieve.mFieldNames = fieldNames;
	myRecordToRetrieve.mNumFields = 5; 

	printf("  Getting our Sake record...\n");
	// Send off the GetMyRecords() request and then wait for a response via the
	// callback.
	request = sakeGetMyRecords(sake, &myRecordToRetrieve, SampleRetrieveSavedGameCallback, &savedGameDataRetrieved);
	// The request could not be dispatched to the Sake server.
	if(!request) 
	{
		// This gets the result of the most recent Sake request that has been
		// made.
		startRequestResult = sakeGetStartRequestResult(sake);
	    printf("Failed to start Sake request: %d", startRequestResult);
		return gsi_false;
	}

	// Here we keep 'thinking' while we wait for the retrieve saved game 
	// callback.
    while (savedGameDataRetrieved == 0)
    {
		// Normally you would be doing other game stuff here; 
		// we just sleep because we have nothing better to do.
        msleep(10); 
		sakeThink();
    }

	// We set savedGameDataRetrieved to -1 if the callback returns an error.
	if (savedGameDataRetrieved < 0) 
		return gsi_false;
	
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
	printf("Storing our saved game data\n");
	if (!SampleStoreSavedGame())
		return -1;

	printf("Retrieving our saved game data\n");
	if (!SampleRetrieveSavedGame())
		return -1;

	// Shutdown the GameSpy SDK Core object, and then we'll wait for user 
	// input before exiting the app. 
	printf("Cleaning up after ourselves\n");
	if (!Cleanup())
		return -1;
    
	return 0;
}
