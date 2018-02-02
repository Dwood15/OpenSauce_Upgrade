///////////////////////////////////////////////////////////////////////////////
// File:	sampleSaveRetrieveScreenshot.c
// SDK:		GameSpy Authentication Service SDK
//
// Copyright (c) 2012 GameSpy Technology & IGN Entertainment, Inc. All rights
// reserved. This software is made available only pursuant to certain license
// terms offered by IGN or its subsidiary GameSpy Industries, Inc. Unlicensed
// use or use in a manner not expressly authorized by IGN or GameSpy Technology
// is prohibited.

// This sample shows how to upload and download content using Sake. Any
// type of file can be uploaded (e.g., .txt, .bmp, .jpg, etc.). This sample
// uploads one file and creates only one record per profile. This sample
// assumes the files uploaded are screenshots. We also show how metadata
// concerning the uploaded files is created and retrieved using Sake.
// The following APIs are used in this sample:
// - sakeUploadContent()
// - sakeCreateRecord()
// - sakeDownloadContent()
// - sakeGetMyRecords()

#include "../../common/gsCommon.h"
#include "../../common/gsCore.h"
#include "../../common/gsAvailable.h"
#include "../../webservices/AuthService.h"
#include "../../sake/sake.h"
#include "../sampleCommon.h"

int newFileId	= 0;	// This will hold the Sake FileId returned from the 
						// backend after upload.
int newRecordId = 0;	// This will hold the Sake RecordId for the record that
						// will hold the FileId in Sake.

char *ScreenShotTable					= "ScreenShots"; // Name of the Sake table we're using to
														 // store content (screen shots in this
														 // sample).
char *ScreenShotFilenameWithPath		= "./gamespylogo.bmp";
char *ScreenShotFileName				= "gamespylogo.bmp";
char *ScreenShotFileType				= "bmp";

// This is a global variable that is used track whether or not the response was
// received from the backend.
int responseReceived = 0;

// Initialization for the Sake SDK.
static gsi_bool Setup()
{
	// Follow these helper functions into sampleCommon.c for details on 
	// what we need to do to set up for the required services.
	if (!AvailabilityCheck())
		return gsi_false;

	if (!AuthSetup())
		return gsi_false;

	if (!SakeSetup())
		return gsi_false;

	return gsi_true;
}

// This callback function is invoked when the content upload is completed. The
// FileId returned is a handle to the uploaded content.
static void UploadCompletedCallback(SAKE				sake, 
									SAKERequestResult	result, 
									gsi_i32				fileid, 
									SAKEFileResult		fileResult, 
									void				* userData)
{
	// If there was an error uploading our file, print a message.
	if	(result != SAKERequestResult_SUCCESS)
    {
        printf("    Upload Content failed. Result = %d \n", result);
		responseReceived = -1;
    }
    else
    {
		// If the upload completed successfully (i.e., we have a valid FileId),
		// print a message.
        if(fileid != 0) 
        {
			printf("    Success - uploaded File ID %d \n", fileid);
			
			// We received a FileId from the backend - we use the userData for
			// saving the FileId.
			*(int *)userData = fileid;

			// Record that the response has been received and completed.
			responseReceived = 1;
        }
        else // Something went wrong with our upload, so print a message.
        {
            printf("    Update of file has failed! file id is 0, file result %d\n", fileResult);
			responseReceived = -1;
        }
    }

	// Avoid strict compile warnings.
    GSI_UNUSED(sake); 
}

// Upload a local file using Sake and return the FileId on success (0 
// otherwise).
static int SampleUploadScreenShot()
{
	// Initialize the UploadContent variables.
	SAKEStartRequestResult	result			= SAKEStartRequestResult_SUCCESS;
	int						uploadedFileId	= 0;
	SAKEUploadContentInput uploadContentInfo;
	SAKEContentInfo info;
    
	// Initialize the content info, specifying the file path and that it's from
	// disk instead of memory.
	info.mFileid		= 0;
	info.mStorage.mFile = ScreenShotFilenameWithPath;
	info.mType			= SAKEContentStorageType_DISK;

	// Initialize the request input, specifying the name under which to store 
	// the file and as non-blocking.
	uploadContentInfo.content = &info;
	uploadContentInfo.progressCallback = NULL;
	uploadContentInfo.remoteFileName = ScreenShotFileName;
	uploadContentInfo.transferBlocking = gsi_false;

	// Make the sakeUploadContent() call to save the file in the Sake File
	// Server.
	printf("  Uploading the screenshot file...\n");
	result = sakeUploadContent( sake, &uploadContentInfo, UploadCompletedCallback, &uploadedFileId);

	// The request could not be dispatched to the Sake server.
	if(result != SAKERequestResult_SUCCESS)
	{
	    printf("  Failed to start Sake request\n");
		return gsi_false;
	}

	// Keep 'thinking' while we wait for the uploadCompleted() callback.
	responseReceived = 0;
	while (responseReceived == 0)
    {
		// Normally you would be doing other game stuff here; we just sleep
		// because we have nothing better to do.
        msleep(10); 
		sakeThink();
    }

	// We set responseReceived to -1 if the callback returns an error.
	if (responseReceived < 0)
		return 0; 

	// Return the FileId created by Sake.
	return uploadedFileId;
}

// This callback indicates that Sake request to create a record to store our 
// file metadata has been completed.
static void StoreMetaDataCallback(	SAKE				sake, 
									SAKERequest			request, 
									SAKERequestResult	result, 
									void				*inputData, 
									void				*outputData, 
									void				*userData
								)
{
	// Initialize the new record ID.
    newRecordId = 0; 
	// If the upload attempt failed, print a message.
	if (result != SAKERequestResult_SUCCESS)
	{
		printf("    Store Content Meta Data failure. Result = %d \n", result);
		responseReceived = -1;
	}
	else // Otherwise, print and record the success.
	{
		SAKECreateRecordOutput *output = (SAKECreateRecordOutput *) outputData;
		newRecordId = output->mRecordId; // Remember the metadata RecordId.
		printf("    Stored Content Meta Data successfully. Record Id %d\n", output->mRecordId);
		responseReceived = 1;
	}

	// Avoid strict compiler warnings.
	GSI_UNUSED(userData);
	GSI_UNUSED(inputData);
	GSI_UNUSED(request);
	GSI_UNUSED(sake);
}

// With each uploaded file it is necessary to store some metadata via Sake 
// storage server (the structured part of Sake) to keep track of the FileId
// (which will be needed later for downloading) and other associated data (such
// as a file description).
static gsi_bool SampleStoreScreenshotMetaData()
{
	// Set up the parameters for the sakeCreateRecord() call.
	SAKERequest				request;
	SAKECreateRecordInput metaDataRecordInput;
	int			fieldIndex = 0;
	// Initalize the fields with field name and type.
	SAKEField	fields[4] = {	"userName", SAKEFieldType_ASCII_STRING, (char)"",	
								"myfile",	SAKEFieldType_INT,			0,
								"fileName", SAKEFieldType_ASCII_STRING,	(char)"",
								"fileType", SAKEFieldType_ASCII_STRING,	 (char)""};

	// Initialize the field values
	fields[fieldIndex++].mValue.mAsciiString = SAMPLE_UNIQUENICK[0];	// Unique player handle
	fields[fieldIndex++].mValue.mInt		 = newFileId;				// Uploaded FileId
	fields[fieldIndex++].mValue.mAsciiString = ScreenShotFileName;		// Uploaded file name
	fields[fieldIndex++].mValue.mAsciiString = ScreenShotFileType;		// Uploaded file type

	// Set the input data for the record creation.
	metaDataRecordInput.mTableId	= ScreenShotTable;					// Table name
	metaDataRecordInput.mNumFields	= fieldIndex;						// Number of fields per record
	metaDataRecordInput.mFields		= fields;							// The fields in the record

	// Now we create a Sake record to store the metadata for the new file.
	printf("  Create a Sake record with metadata for our screenshot...\n");
	request = sakeCreateRecord(sake, &metaDataRecordInput, StoreMetaDataCallback, NULL);

	// The request could not be dispatched to the Sake server.
	if(!request)
	{
		// This gets the result of the most recent Sake request that has been 
		// made.
		SAKEStartRequestResult startRequestResult = sakeGetStartRequestResult(sake);
	    printf("  Failed to start Sake request: %d", startRequestResult);
		return gsi_false;
	}

	// Keep 'thinking' while we wait for the createRecord() callback.
	responseReceived = 0;
	while (responseReceived == 0)
	{
		// Normally you would be doing other game stuff here; we just sleep
		// because we have nothing better to do.
		msleep(10); 
		sakeThink();
	}
	// We set responseReceived to -1 if the callback returns an error.
	if (responseReceived < 0)
		return gsi_false; 

	return gsi_true;
}

// This callback indicates that the response for GetMyRecords has been 
// received.
static void GetMyRecordsCallback(	SAKE				sake, 
									SAKERequest			request, 
									SAKERequestResult	result, 
									void				*inputData, 
									void				*outputData, 
									void				*userData
)
{
	// There was an error retrieving our record(s), so print a message.
	if (result != SAKERequestResult_SUCCESS) 
	{
		printf("    Get metadata failure. Result = %d \n", result);
		responseReceived = -1;
	}
	else // Success.
	{
		// Copy the record data into the userData, since the inputData and
		// outputData will be freed after the callback completes.
		SAKEGetMyRecordsInput  *input  = (SAKEGetMyRecordsInput  *) inputData;
		SAKEGetMyRecordsOutput *output = (SAKEGetMyRecordsOutput *) outputData;
		*(SAKEGetMyRecordsOutput *) userData = *(SAKEGetMyRecordsOutput *) CopySakeRecordsFromResponseData(input,output);
		
		// Print the metadata record info.
		printf("    Metadata retrieved:\n");
		DisplaySakeRecords(output->mRecords , output->mNumRecords, input->mNumFields);

		responseReceived = 1;
	}

	// Avoid strict compile warnings.
	GSI_UNUSED(sake);
	GSI_UNUSED(request);
}

// This callback shows the progress for downloading content from Sake.
static void DownloadProgressCallback(SAKE		sake, 
                                     gsi_u32	bytesTransfered, 
                                     gsi_u32	totalSize, 
                                     void		* userData
)
{
    printf("    Downloaded %d out of %d bytes....\n", bytesTransfered, totalSize);
    GSI_UNUSED(sake);
    GSI_UNUSED(userData);
}

// This callback is invoked when the download completes.
static void DownloadCompletedCallback(SAKE				sake, 
									  SAKERequestResult	result,
									  SAKEFileResult	fileResult, 
									  char				* buffer, 
									  gsi_i32			bufferLength, 
									  void				* userData
)
{

	// Something went wrong and our file could not be downloaded; check the
	// result and fileResult for details.
    if(result != SAKERequestResult_SUCCESS)
	{
        printf("    Download failed, request result = %d, file result = %d\n", result, fileResult);
		responseReceived = -1;
	}
    else
	{
		// The file downloaded successfully and will be in our working
		// directory because we downloaded to disk. If we download to
		// memory then the buffer will contain the downloaded content.
        printf("    Successfully downloaded file!!\n");
		responseReceived = 1;
	}
    
	// Avoid strict compile warnings.
	GSI_UNUSED(userData);
    GSI_UNUSED(sake);
}

// Use sakeGetMyRecords() to return our screenshot metadata record in order to
// get the FileId so that we can later download the screenshot file.
static gsi_bool SampleRetrieveScreenshotMetaData(SAKEGetMyRecordsOutput * myRecords)
{
	// Set up parameters for the getMyRecords() call.
	SAKERequest request;
	char *fields[3] = {"recordid", "myfile", "fileName"};
	SAKEGetMyRecordsInput myRecordsInput;
	myRecordsInput.mTableId = ScreenShotTable;
	myRecordsInput.mFieldNames = fields;
	myRecordsInput.mNumFields  = 3;
	
	// Retrieve our screenshot metadata, which includes the FileId; we'll copy
	// the record(s) into the userData so that we can use this metadata info
	// for downloading the file(s).
	printf("  Getting our Sake record which contains screenshot metadata...\n");
    request = sakeGetMyRecords(sake, &myRecordsInput, GetMyRecordsCallback, myRecords);
	
	// The request could not be dispatched to the Sake server.
	if(!request)
	{
		// This call gets the result of the most recent Sake call that has been
		// made.
		SAKEStartRequestResult startRequestResult = sakeGetStartRequestResult(sake);
	    printf("  Failed to start Sake request: %d", startRequestResult);
		return gsi_false;
	}

	// Keep 'thinking' while we wait for the getMyRecords() callback.
	responseReceived = 0;
	while (responseReceived == 0)
	{
		// Normally you would be doing other game stuff here; we just sleep
		// because we have nothing better to do.
		msleep(10); 
		sakeThink();
	}

	// We set responseReceived to -1 if the callback returned an error.
	if (responseReceived < 0) 
		return gsi_false; 

	return gsi_true;
}

// Download our screenshot, using the FileId and metadata in myRecords.
// Note that we only maintain one record per owner Id for this sample
// (otherwise we would loop through all records).
static gsi_bool SampleDownloadScreenshot(SAKEGetMyRecordsOutput myRecords)
{
	// Create the parameters for the sakeDownloadContent() call.
	SAKERequestResult result;
	SAKEDownloadContentInput downloadContent;
	SAKEContentInfo content;
		
	// Set the file specific data using our metadata record.
	content.mFileid = myRecords.mRecords[0][1].mValue.mInt;
	content.mStorage.mFile =  myRecords.mRecords[0][2].mValue.mAsciiString;
	content.mType = SAKEContentStorageType_DISK; // Store the file on disk 
												 // rather than in memory.
    
	// We'll use the progress callback to show a progress percentage.
	downloadContent.progressCallback = DownloadProgressCallback;
	downloadContent.transferBlocking = gsi_false; // Non-blocking (asynchronous)
	downloadContent.content = &content;

	// Now let's download our file.
	printf("  Downloading screenshot file...\n");
	result = sakeDownloadContent(sake, &downloadContent, DownloadCompletedCallback, NULL);

	// The request could not be dispatched to the Sake server.
	if(result != SAKERequestResult_SUCCESS) 
	{
	    printf("  Failed to start Sake request\n");
		return gsi_false;
	}

	// Keep 'thinking' while we wait for the downloadCompleted() callback.
	responseReceived = 0;
	while (responseReceived == 0)
	{
		// Normally you would be doing other game stuff here; we just
		// sleep because we have nothing better to do.
		msleep(10); 
		sakeThink();
	}
	// We set responseReceived to -1 if the callback returns an error.
	if (responseReceived < 0) 
		return gsi_false;

	return gsi_true;
}

// Saving a screenshot (or any file) consists of two steps: uploading the file
// itself, then creating a Sake record for associated data - most importantly,
// the 'FileId' received upon uploading.
static gsi_bool SampleSaveScreenshot()
{
	// *** first upload our screenshot, making sure to keep track of the file id ***

	// initialize UploadContent variables
	SAKEStartRequestResult	result			= SAKEStartRequestResult_SUCCESS;
	int						uploadedFileId	= 0;
	SAKEUploadContentInput uploadContentInfo;
	SAKEContentInfo info;

	// initialize CreateRecord variables for later
	// setup parameters for sakeCreateRecord
	SAKERequest				request;
	SAKECreateRecordInput metaDataRecordInput;
	int			fieldIndex = 0;
	// initalize the fields with field name and type; note that we use int type for FileIDs, but 
	// you use the FileID type when you create the field for your Sake table
	SAKEField	fields[4] = {	"userName", SAKEFieldType_ASCII_STRING, (char)"",	
								"myfile",	SAKEFieldType_INT,			0,
								"fileName", SAKEFieldType_ASCII_STRING,	(char)"",
								"fileType", SAKEFieldType_ASCII_STRING,	 (char)""};
    
	// initialize the content info, specifying the file path and that it's from disk instead of from memory
	info.mFileid		= 0;
	info.mStorage.mFile = ScreenShotFilenameWithPath;
	info.mType			= SAKEContentStorageType_DISK;

	// initialize the request input, specifying the name to store the file as, and non-blocking
	uploadContentInfo.content = &info;
	uploadContentInfo.progressCallback = NULL;
	uploadContentInfo.remoteFileName = ScreenShotFileName;
	uploadContentInfo.transferBlocking = gsi_false;

	// use sakeUploadContent to save the file on the Sake Storage Server
	printf("  Uploading the screenshot file...\n");
	result = sakeUploadContent( sake, &uploadContentInfo, UploadCompletedCallback, &uploadedFileId);

	if(result != SAKERequestResult_SUCCESS) // local issue prior to the request actually going out
	{
	    printf("  Failed to start Sake request\n");
	// Upload our screenshot, making sure to keep track of the FileId.
	newFileId = SampleUploadScreenShot();
	if (newFileId == 0) // There was an error uploading the file.
		return gsi_false;
	}

	// keep 'thinking' while we wait for the upload uploadCompleted callback
	responseReceived = 0;
	while (responseReceived == 0)
    {
		// normally you would be doing other game stuff here; we just sleep because we have nothing 
		// better to do
        msleep(10); 
		sakeThink();
    }

	if (responseReceived < 0) // we set responseReceived to -1 if the callback returns an error
		return 0; 

	if (uploadedFileId == 0) // error uploading file
		return gsi_false;


	// *** now create a Sake record to store data associated with the file, namely the file Id ***

	// Initialize the field values
	fields[fieldIndex++].mValue.mAsciiString = SAMPLE_UNIQUENICK[0];	// unique nick
	fields[fieldIndex++].mValue.mInt		 = uploadedFileId;				// uploaded file id
	fields[fieldIndex++].mValue.mAsciiString = ScreenShotFileName;		// uploaded file name
	fields[fieldIndex++].mValue.mAsciiString = ScreenShotFileType;		// uploaded file type

	// set the input data for the record creation
	metaDataRecordInput.mTableId	= ScreenShotTable;					// Table name
	metaDataRecordInput.mNumFields	= fieldIndex;						// Number of fields per record
	metaDataRecordInput.mFields		= fields;							// The fields in the record.

	// now create a Sake record to store our new file data
	printf("  Create a Sake record with metadata for our screenshot...\n");
	request = sakeCreateRecord(sake, &metaDataRecordInput, StoreMetaDataCallback, NULL);

	if(!request) // local issue prior to the request actually going out
	{
		// gets the result of the most recent Sake call attempted
		SAKEStartRequestResult startRequestResult = sakeGetStartRequestResult(sake);
	    printf("  Failed to start Sake request: %d", startRequestResult);
	// Create a Sake record to store data associated with the uploaded file.
	if (!SampleStoreScreenshotMetaData())
		return gsi_false;
	}

	// keep 'thinking' while we wait for the createRecord callback 
	responseReceived = 0;
	while (responseReceived == 0)
	{
		// normally you would be doing other game stuff here; we just sleep because we have nothing 
		// better to do
		msleep(10); 
		sakeThink();
	}

	if (responseReceived < 0) // we set responseReceived to -1 if the callback returns an error
		return gsi_false; 

	return gsi_true;
}

// Assuming we don't already know the FileId we want, retrieving a user's
// screenshot (or any file) also consists of two steps: retrieving the player's
// file metadata record, then downloading the file based on the FileId in the
// metadata.
static gsi_bool SampleRetrieveScreenShot( )
{
	// *** First we retrieve the Sake record that has the file Id ***

	SAKEGetMyRecordsOutput myRecords;
	
	// setup parameters for getMyRecords call
	SAKERequest request;
	char *fields[3] = {"recordid", "myfile", "fileName"};
	SAKEGetMyRecordsInput myRecordsInput;
	myRecordsInput.mTableId = ScreenShotTable;
	myRecordsInput.mFieldNames = fields;
	myRecordsInput.mNumFields  = 3;
	
	// retrieve our screenshot metadata, which includes the FileID; we'll copy the record(s) into the 
	// userData so that we can use this metadata info for downloading the file(s)
	printf("  Getting our Sake record which contains screenshot metadata...\n");
    request = sakeGetMyRecords(sake, &myRecordsInput, GetMyRecordsCallback, &myRecords);
	
	if(!request) // local issue prior to the request actually going out
	{
		// gets the result of the most recent Sake call attempted
		SAKEStartRequestResult startRequestResult = sakeGetStartRequestResult(sake);
	    printf("  Failed to start Sake request: %d", startRequestResult);
	// Get our screenshot metadata so that we have the info needed to later
	// download the screenshot.
	if (!SampleRetrieveScreenshotMetaData(&myRecords))
		return gsi_false;
	}

	// keep 'thinking' while we wait for the getMyRecords callback
	responseReceived = 0;
	while (responseReceived == 0)
	{
		// normally you would be doing other game stuff here; we just sleep because we have nothing 
		// better to do
		msleep(10); 
		sakeThink();
	}

	if (responseReceived < 0) // we set responseReceived to -1 if the callback returns an error
		return gsi_false; 

	// *** Now we download the screenshot file based on the file Id in our record *** 

	// Check whether or not there are any metadata records obtained.
	if (myRecords.mNumRecords > 0 && myRecords.mRecords[0] != NULL)
	{
		// sakeDownloadContent parameters
		SAKERequestResult result;
		SAKEDownloadContentInput downloadContent;
		SAKEContentInfo content;

		// we use this global variable to track the recordID so we can delete it later during cleanup
		newRecordId = myRecords.mRecords[0][0].mValue.mInt;
			
		// set the file specific data, using our metadata record
		content.mFileid = myRecords.mRecords[0][1].mValue.mInt;
		content.mStorage.mFile =  myRecords.mRecords[0][2].mValue.mAsciiString;
		content.mType = SAKEContentStorageType_DISK; // store file on disk rather than memory
	    
		// we'll use the progress callback to show a progress percentage
		downloadContent.progressCallback = DownloadProgressCallback;
		downloadContent.transferBlocking = gsi_false; // non-blocking (asynchronous)
		downloadContent.content = &content;

		// alas, let's download our file
		printf("  Downloading screenshot file...\n");
		result = sakeDownloadContent(sake, &downloadContent, DownloadCompletedCallback, NULL);

		if(result != SAKERequestResult_SUCCESS) // local issue prior to the request actually going out
		{
			printf("  Failed to start Sake request\n");
			return gsi_false;
		}

		// keep 'thinking' while we wait for the downloadCompleted callback
		responseReceived = 0;
		while (responseReceived == 0)
		{
			// normally you would be doing other game stuff here; we just sleep because we have nothing 
			// better to do
			msleep(10); 
			sakeThink();
		}

		if (responseReceived < 0) // we set responseReceived to -1 if the callback returns an error
		// We use this global variable to track the RecordId so that we can
		// delete it later during cleanup.
		newRecordId = myRecords.mRecords[0][0].mValue.mInt;

		// Download the screenshot.
		if (!SampleDownloadScreenshot(myRecords))
			return gsi_false;
	}

	// Free the allocated memory (from the getMyRecords() callback) for the
	// metadata.
	FreeSakeRecords(&myRecords, 3);

	return gsi_true;
}

static gsi_bool Cleanup()
{
	// To keep our sample table clean we'll delete the saved record that we
	// created.
	printf("  Deleting our file meta data record...\n");
	if (!DeleteSakeRecord(ScreenShotTable, newRecordId))
		return gsi_false;

	// Shutdown Sake and the GameSpy Core SDK (which ensures internal objects
	// get freed).
	SakeCleanup();

	// Keep the command line prompt open so you can view the output.
	WaitForUserInput();

	return gsi_true;
}

int test_main()
{	
	// First, do the prerequisite initialization so that we'll be ready to use
	// Sake to save/retrieve a screenshot.
    printf("Doing the prerequisite setup and initialization\n");
	if (!Setup())
		return -1;

	// This is the beef of the sample app. You can also reference this 
	// sample code on the developer portal 'Docs'.
	printf("Saving our screenshot\n");
	if (!SampleSaveScreenshot())
		return -1;

	printf("Retrieving our screenshot\n");
	if (!SampleRetrieveScreenShot())
		return -1;

	// Shutdown the GameSpy Core SDK object; then we'll wait for user input
	// before exiting the app.
	printf("Cleaning up after ourselves\n");
	if (!Cleanup())
		return -1;
    
	return 0;
}
