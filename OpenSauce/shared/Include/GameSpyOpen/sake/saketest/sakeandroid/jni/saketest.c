///////////////////////////////////////////////////////////////////////////////
// File:	saketest.c
// SDK:		GameSpy Sake Persistent Storage SDK
//
// Copyright (c) IGN Entertainment, Inc.  All rights reserved.  
// This software is made available only pursuant to certain license terms offered
// by IGN or its subsidiary GameSpy Industries, Inc.  Unlicensed use or use in a 
// manner not expressly authorized by IGN or GameSpy is prohibited.

#include <jni.h>
#include <errno.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include "../../../../common/gsCommon.h"
#include "../../../../common/gsAvailable.h"
#include "../../../../common/gsCore.h"
#include "../../../../common/gsSoap.h"
#include "../../../../sake/sake.h"
#include "../../../../ghttp/ghttp.h"
#include "../../../../GP/gp.h"
#include "../../../../webservices/AuthService.h"

#define URL NULL
#define GAMENAME	_T("gmtest")
#define SECRET_KEY	_T("HA6zkS")
#define GAMENAME_ASCII	"gmtest"
#define SECRET_KEY_ASCII	"HA6zkS"
#define GAMEID		0

//#define ACCESS_KEY     _T("d91f1c2c6a14197465b6b78b27f2d75e")
#define ACCESS_KEY     "39cf9714e24f421c8ca07b9bcb36c0f5"
#define PRODUCTID	0		
#define NAMESPACEID 1
#define NICKNAME    _T("saketest")
#define EMAIL       _T("saketest@saketest.com")
#define PASSWORD    _T("saketest")

#define CHECK_GP_RESULT(func, errString) if(func != GP_NO_ERROR) { LOGI("%s\n", errString); exit(0); }

#define SAKE_UPLOAD_AMOUNT (512*1024) //smaller stack size
#define SAKE_DOWNLOAD_AMOUNT (512*1024*2)
#define MAX_NUM_TESTS	100

// Android logs
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "YOUR_TAG", __VA_ARGS__))

typedef struct DataStruct
{
	char pData[SAKE_UPLOAD_AMOUNT];
} DataStruct;

typedef struct TestResults
{
	gsi_bool	pass;
	char		errString[512];
	char		requestType[512];
} TestResults;

typedef struct sakeImageUlDlInfoStruct
{
    gsi_i32             gUploadedFileId;
    SAKEContentBuffer     gImageBuffer;
} sakeImageUlDlInfoStruct ;

sakeImageUlDlInfoStruct sakeTestImageData;

int NumOperations = 0;
GPConnection * pconn;
GPProfile profileid;
GSLoginCertificate certificate;
GSLoginPrivateData privateData;
int authenticated = 0;
TestResults results[MAX_NUM_TESTS];
int gIndex = 0;
int gNumOwned = 0;
SAKE sake;
FILE * file = 0;

// Testing specific variables
int totalFailed = 0;
//=======================

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//PROTOTYPES
static void Validate(char * requestType, void *inputData, void *outputData);
static void ReportFailure(char * reason);


///////////////////////////////////////////////////////////////////////////////
//Big Validation function to ensure integrity of data received
static void Validate(char * requestType, void *inputData, void *outputData)
{
	SAKEField ** records;
	SAKEField * field;
	int recordIndex;
	int fieldIndex;

	if (!results[gIndex].pass) //if already failed, dont change failure reason
		return;
	

	if (strcmp(requestType,"GetMyRecords") == 0)
	{
		SAKEGetMyRecordsInput * input = (SAKEGetMyRecordsInput *)inputData;
		SAKEGetMyRecordsOutput * output = (SAKEGetMyRecordsOutput *)outputData;

		records = output->mRecords;

		//verify numrecords == numowned
		if (output->mNumRecords != gNumOwned)
			ReportFailure("num records does not match num owned");

		//verify all data has ownerid == my profileid
		for(recordIndex = 0 ; recordIndex < output->mNumRecords ; recordIndex++)
		{
			for(fieldIndex = 0 ; fieldIndex < input->mNumFields ; fieldIndex++)
			{
				field = &records[recordIndex][fieldIndex];
				if (strcmp(field->mName,"ownerid") == 0 && field->mValue.mInt != profileid)
					ReportFailure("ownerid doesnt match profileid");
			}
		}
	}

	if (strcmp(requestType,"GetSpecificRecords") == 0)
	{
		SAKEGetSpecificRecordsInput * input = (SAKEGetSpecificRecordsInput *)inputData;
		SAKEGetSpecificRecordsOutput * output = (SAKEGetSpecificRecordsOutput *)outputData;

		records = output->mRecords;

		//verify the right number of recordids where retrieved
		if (output->mNumRecords != input->mNumRecordIds)
			ReportFailure("num records does not match num record ids specified");

		//verify all recorids where retrieved
		for(recordIndex = 0 ; recordIndex < output->mNumRecords ; recordIndex++)
		{
			for(fieldIndex = 0 ; fieldIndex < input->mNumFields ; fieldIndex++)
			{
				field = &records[recordIndex][fieldIndex];
				if (strcmp(field->mName,"recordid") == 0 && field->mValue.mInt != input->mRecordIds[recordIndex])
					ReportFailure("recordid not requested");
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void ProcessTasks()
{	
	LOGI("Procesing\n");
	if (NumOperations > 1)
		LOGI("WHY ARE TWO TASKS PROCESSING HERE!?");
	
	// Think to process tasks
	while(NumOperations > 0)
	{
	    LOGI("Thinking\n");
		msleep(5);
        ghttpThink();
		gsCoreThink(0);
	}
	LOGI("Processing Done  \n");
}

///////////////////////////////////////////////////////////////////////////////

static GSIACResult CheckServices(void)
{
	GSIACResult aResult;
	GSIStartAvailableCheck(GAMENAME);

	// Continue processing while the check is in progress
	do
	{
		aResult = GSIAvailableCheckThink();
		msleep(10);
	}
	while(aResult == GSIACWaiting);

	// Check the result
	switch(aResult)
	{
	case GSIACAvailable:
		LOGI("Online Services are available\r\n");
		break;
	case GSIACUnavailable:
		LOGI("Online services are unavailable\r\n");
		LOGI("Please visit www.mygame.com for more information.\r\n");
		break;
	case GSIACTemporarilyUnavailable:
		LOGI("Online services are temporarily unavailable.\r\n");
		LOGI("Please visit www.mygame.com for more information.\r\n");
		break;
	default:
		break;
	};
		
	return aResult;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void myLoginCallback(GHTTPResult httpResult, WSLoginResponse * theResponse, void * userData)
{
	if (httpResult != GHTTPSuccess)
	{
		LOGI("Failed on player login, HTTP error: %d", httpResult);
		exit(0);
	}
	else if (theResponse->mLoginResult != WSLogin_Success)
	{
		LOGI("Failed on player login, Login result: %d", theResponse->mLoginResult);
		exit(0);
	}
	else
	{
		// copy certificate and private key
		profileid = theResponse->mCertificate.mProfileId;
		memcpy(&certificate, &theResponse->mCertificate, sizeof(GSLoginCertificate));
		memcpy(&privateData, &theResponse->mPrivateData, sizeof(GSLoginPrivateData));

		authenticated = 1; // so we know we can stop gsCore thinking
		_tprintf(_T("Player '") GS_USTR _T("' logged in.\n"), theResponse->mCertificate.mUniqueNick);
	}
	GSI_UNUSED(userData);
}

static void LoginAndAuthenticate()
{
	wsSetGameCredentials(ACCESS_KEY, GAMEID, SECRET_KEY_ASCII);

	if (0 != wsLoginProfile(GAMEID, GP_PARTNERID_GAMESPY, NAMESPACEID, NICKNAME, EMAIL, PASSWORD, _T(""), myLoginCallback, NULL))
	{
		LOGI("Failed on wsLoginProfile");
		exit(0);
	}

	while(authenticated == 0)
	{
		msleep(100);
		gsCoreThink(0);
	}

	// authenticate Sake with the login ticket and the profileid returned from login
	LOGI("Authenticating with Sake\n");
	sakeSetGame(sake, GAMENAME, GAMEID, SECRET_KEY);
	sakeSetProfile(sake, profileid, &certificate, &privateData);
}

static void PrintResults()
{
	int i;
	int passed = 0;
	int failed = 0;

	LOGI("\n=======================================================\n");
	LOGI("==================== FINAL RESULTS ====================\n");
	LOGI("=======================================================\n");
	LOGI("\n*Note: ReportRecord is expected to fail since the record will have already been reported by the same profile.\n\n");


	for (i=0; i<gIndex; i++)
	{
		LOGI("#%3d - %s: ", i+1, results[i].requestType);
		if (results[i].pass)
		{
			LOGI("PASS\n");
			passed++;
		}
		else
		{
			LOGI("FAIL\t[%s]\n", results[i].errString);
			// Since Report Record should fail and it's the first test, don't fail if it does.
			if(i != 0)
			{
				failed++;
			}
			else
			{
				passed++;
			}
		}
	}
	LOGI("TOTAL PASSED = %d\n", passed);
	LOGI("TOTAL FAILED = %d\n\n", failed);

	totalFailed = failed;

	if (failed == 0)
	{
		LOGI("         ALL TEST PASSED - WOOHOO!!!!\n\n");
	}
}

static void ReportFailure(char * reason)
{
	sprintf(results[gIndex].errString, "%s", reason);
	results[gIndex].pass = gsi_false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int UnicodeStringLen(const unsigned short * str)
{
	const unsigned short * end = str;
	while(*end++)
		{}
	return (end - str - 1);
}

static void PrintSeperator(void)
{
	LOGI("*******************\n");
}

static const char * FieldTypeToString(SAKEField * field)
{
	static char buffer[32];
	SAKEFieldType type = field->mType;

	if(type == SAKEFieldType_BYTE)
		return "byte";
	if(type == SAKEFieldType_SHORT)
		return "short";
	if(type == SAKEFieldType_INT)
		return "int";
	if(type == SAKEFieldType_FLOAT)
		return "float";
	if(type == SAKEFieldType_ASCII_STRING)
		return "asciiString";
	if(type == SAKEFieldType_UNICODE_STRING)
		return "unicodeString";
	if(type == SAKEFieldType_BOOLEAN)
		return "boolean";
	if(type == SAKEFieldType_DATE_AND_TIME)
		return "dateAndTime";
	if(type == SAKEFieldType_BINARY_DATA)
	{
		sprintf(buffer, "binaryData-%d", field->mValue.mBinaryData.mLength);
		return buffer;
	}
	if (type == SAKEFieldType_INT64)
	{
		return "int64";
	}
	return "ERROR!!  Invalid value type set";
}

static const char * FieldValueToString(SAKEField * field)
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
	{
		// cap the value if it is too long (note, this is destructive)
		if(UnicodeStringLen(value->mUnicodeString) > 20)
			value->mUnicodeString[20] = 0;
		UCS2ToAsciiString(value->mUnicodeString, buffer);
	}
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
		return "ERROR!!  Invalid value type set";

	return buffer;
}

static const char * RequestResultToString(SAKERequestResult requestResult)
{
	switch(requestResult)
	{
	case SAKERequestResult_SUCCESS:
		return "SUCCESS";
	case SAKERequestResult_CONNECTION_TIMEOUT:
		return "CONNECTION_TIMEOUT";
	case SAKERequestResult_CONNECTION_ERROR:
		return "CONNECTION_ERROR";
	case SAKERequestResult_MALFORMED_RESPONSE:
		return "MALFORMED_RESPONSE";
	case SAKERequestResult_OUT_OF_MEMORY:
		return "OUT_OF_MEMORY";
	case SAKERequestResult_DATABASE_UNAVAILABLE:
		return "DATABASE_UNAVAILABLE";
	case SAKERequestResult_LOGIN_TICKET_INVALID:
		return "LOGIN_TICKET_INVALID";
	case SAKERequestResult_LOGIN_TICKET_EXPIRED:
		return "LOGIN_TICKET_EXPIRED";
	case SAKERequestResult_TABLE_NOT_FOUND:
		return "TABLE_NOT_FOUND";
	case SAKERequestResult_RECORD_NOT_FOUND:
		return "RECORD_NOT_FOUND";
	case SAKERequestResult_FIELD_NOT_FOUND:
		return "FIELD_NOT_FOUND";
	case SAKERequestResult_FIELD_TYPE_INVALID:
		return "FIELD_TYPE_INVALID";
	case SAKERequestResult_NO_PERMISSION:
		return "NO_PERMISSION";
	case SAKERequestResult_RECORD_LIMIT_REACHED:
		return "RECORD_LIMIT_REACHED";
	case SAKERequestResult_ALREADY_RATED:
		return "ALREADY_RATED";
	case SAKERequestResult_ALREADY_REPORTED:
		return "ALREADY_REPORTED";
	case SAKERequestResult_NOT_RATEABLE:
		return "NOT_RATEABLE";
	case SAKERequestResult_NOT_OWNED:
		return "NOT_OWNED";
	case SAKERequestResult_FILTER_INVALID:
		return "FILTER_INVALID";
	case SAKERequestResult_SORT_INVALID:
		return "SORT_INVALID";
	case SAKERequestResult_TARGET_FILTER_INVALID:
		return "TARGET_FILTER_INVALID";
	case SAKERequestResult_CERTIFICATE_INVALID:
		return "CERTIFICATE_INVALID";
	case SAKERequestResult_UNKNOWN_ERROR:
		return "UNKNOWN_ERROR";
    case SAKERequestResult_REQUEST_CANCELLED:
        return "REQUEST_CANCELLED";
	case SAKERequestResult_CONTENTSERVER_FAILURE:
	    return "CONTENTSERVER_FAILURE";
	case SAKERequestResult_INVALID_GAMEID:
		return "INVALID_GAMEID";
	case SAKERequestResult_INVALID_SESSIONTOKEN:
		return "INVALID_SESSIONTOKEN";
	case SAKERequestResult_SESSIONTOKEN_EXPIRED:
		return "SESSIONTOKEN_EXPIRED";
	default:
		break;
	}

	return "Unrecognized error";
}

// prints request result, returns gsi_false for errors
static gsi_bool HandleRequestResult(SAKERequestResult requestResult, const char * requestType)
{

	LOGI("%s Callback Result %s\n", requestType, RequestResultToString(requestResult));
    
	sprintf(results[gIndex].requestType, "%s", requestType);
	
	if(requestResult != SAKERequestResult_SUCCESS)
	{
		sprintf(results[gIndex].errString, "Error - %s", RequestResultToString(requestResult));
		results[gIndex].pass = gsi_false;
		return gsi_false;
	}

	results[gIndex].pass = gsi_true;
	return gsi_true;
}

static void DisplayReadResults(SAKEField ** records, int numRecords, int numFields)
{
	SAKEField * field;
	int recordIndex;
	int fieldIndex;

	PrintSeperator();
	LOGI("Num Records: %d\n", numRecords);
	PrintSeperator();

	for(recordIndex = 0 ; recordIndex < numRecords ; recordIndex++)
	{
		for(fieldIndex = 0 ; fieldIndex < numFields ; fieldIndex++)
		{
			field = &records[recordIndex][fieldIndex];

			if (field->mType == SAKEFieldType_NULL) // this shouldn't happen unless it's an Atlas stat with no default
				LOGI("%s is %s\n", field->mName, FieldTypeToString(field));
			else
				LOGI("%s[%s]=%s\n", field->mName, FieldTypeToString(field), FieldValueToString(field));
		}

		PrintSeperator();
	}
}


static void LogReadResults(SAKEField ** records, int numRecords, int numFields)
{
	SAKEField * field;
	int recordIndex = 0;
	int fieldIndex;
	char buffer[4086] = {0};

#if !defined(NOFILE)


	if (numRecords == 0)
		return;

	//first pass, write the fields out
	for(fieldIndex = 0 ; fieldIndex < numFields ; fieldIndex++)
	{
		field = &records[recordIndex][fieldIndex];
	}

	for(recordIndex = 0 ; recordIndex < numRecords ; recordIndex++)
	{
		for(fieldIndex = 0 ; fieldIndex < numFields ; fieldIndex++)
		{
			field = &records[recordIndex][fieldIndex];
		}
	}
#else
	sprintf(buffer,"*******************\r\nNum Records: %d\r\n*******************\r\n", numRecords);
	printf("%s",buffer);

	if (numRecords == 0)
		return;

	//first pass, write the fields out
	for(fieldIndex = 0 ; fieldIndex < numFields ; fieldIndex++)
	{
		field = &records[recordIndex][fieldIndex];
		sprintf(buffer,"%15s|", field->mName);
		printf("%s",buffer);
	}
	sprintf(buffer,"\r\n");
	printf("%s",buffer);


	for(recordIndex = 0 ; recordIndex < numRecords ; recordIndex++)
	{
		for(fieldIndex = 0 ; fieldIndex < numFields ; fieldIndex++)
		{
			field = &records[recordIndex][fieldIndex];
			sprintf(buffer,"%15s|", FieldValueToString(field));
			printf("%s",buffer);
		}
		sprintf(buffer,"\r\n");
		printf("%s",buffer);
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void ReportRecordCallback(SAKE sake, SAKERequest request, SAKERequestResult result, void *inputData, void *outputData, void *userData)
{
	NumOperations--;

	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(inputData);
	GSI_UNUSED(outputData);
	GSI_UNUSED(userData);

	if(HandleRequestResult(result, "ReportRecord") == gsi_false)
		return;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void CreateRecordCallback(SAKE sake, SAKERequest request, SAKERequestResult result, void *inputData, void *outputData, void *userData)
{
	SAKECreateRecordOutput * output = (SAKECreateRecordOutput *)outputData;
	int * recordid = (int *)userData;

	NumOperations--;

	if(HandleRequestResult(result, "CreateRecord") == gsi_false)
		return;
	
	*recordid = output->mRecordId;
	LOGI("Created recordid %d\n", output->mRecordId);

	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(inputData);
	GSI_UNUSED(userData);
}

static void UpdateRecordCallback(SAKE sake, SAKERequest request, SAKERequestResult result, void *inputData, void *outputData, void *userData)
{
	NumOperations--;
	
	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(inputData);
	GSI_UNUSED(outputData);
	GSI_UNUSED(userData);

	if(HandleRequestResult(result, "UpdateRecord") == gsi_false)
		return;
}

static void DeleteRecordCallback(SAKE sake, SAKERequest request, SAKERequestResult result, void *inputData, void *outputData, void *userData)
{
	NumOperations--;

	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(inputData);
	GSI_UNUSED(outputData);
	GSI_UNUSED(userData);

	if(HandleRequestResult(result, "DeleteRecord") == gsi_false)
		return;
}

static void SearchForRecordsCallback(SAKE sake, SAKERequest request, SAKERequestResult result, void *inputData, void *outputData, void *userData)
{
	SAKESearchForRecordsInput * input = (SAKESearchForRecordsInput *)inputData;
	SAKESearchForRecordsOutput * output = (SAKESearchForRecordsOutput *)outputData;

	NumOperations--;

	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(userData);

	if(HandleRequestResult(result, "SearchForRecords") != gsi_false)
    {
	    LogReadResults(output->mRecords, output->mNumRecords, input->mNumFields);
    }

    // release the memory allocated for search
    if (input->mOwnerIds) gsifree(input->mOwnerIds);
    if (input->mFieldNames) gsifree(input->mFieldNames);
}

static void GetMyRecordsCallback(SAKE sake, SAKERequest request, SAKERequestResult result, void *inputData, void *outputData, void *userData)
{
	SAKEGetMyRecordsInput * input = (SAKEGetMyRecordsInput *)inputData;
	SAKEGetMyRecordsOutput * output = (SAKEGetMyRecordsOutput *)outputData;

	NumOperations--;

	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(userData);

	if(HandleRequestResult(result, "GetMyRecords") == gsi_false)
		return;

	DisplayReadResults(output->mRecords, output->mNumRecords, input->mNumFields);

	//Now validate the data and ensure its ours
	Validate("GetMyRecords", input, output);
}


static void RateRecordCallback(SAKE sake, SAKERequest request, SAKERequestResult result, void *inputData, void *outputData, void *userData)
{
	SAKERateRecordOutput * output = (SAKERateRecordOutput *)outputData;

	NumOperations--;

	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(inputData);
	GSI_UNUSED(userData);

	if(HandleRequestResult(result, "RateRecord") == gsi_false)
		return;

	LOGI("NumRatings: %d\n", output->mNumRatings);
	LOGI("AverageRating: %0.3f\n", output->mAverageRating);
}

static void GetRecordLimitCallback(SAKE sake, SAKERequest request, SAKERequestResult result, void *inputData, void *outputData, void *userData)
{
	SAKEGetRecordLimitOutput * output = (SAKEGetRecordLimitOutput *)outputData;

	NumOperations--;

	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(inputData);
	GSI_UNUSED(userData);

	if(HandleRequestResult(result, "GetRecordLimit") == gsi_false)
		return;

	LOGI("LimitPerOwner: %d\n", output->mLimitPerOwner);
	LOGI("NumOwned: %d\n", output->mNumOwned);

	gNumOwned = output->mNumOwned;
}

static void GetRecordCountCallback(SAKE sake, SAKERequest request, SAKERequestResult result, void *inputData, void *outputData, void *userData)
{
	SAKEGetRecordCountOutput * output = (SAKEGetRecordCountOutput *)outputData;
	int * count = (int *)userData;

	NumOperations--;
    
	GSI_UNUSED(sake);
	GSI_UNUSED(request);
	GSI_UNUSED(inputData);
	GSI_UNUSED(userData);

	if(HandleRequestResult(result, "GetRecordCount") == gsi_false)
		return;

	*count = output->mCount;
	LOGI("Record Count: %d\n", output->mCount);
}

static GHTTPBool gUploadResult;


static void UploadContentProgressCallback(SAKE sake, 
                                          gsi_u32 bytesTransfered, 
                                          gsi_u32 totalSize, 
                                          void *userData)
{
	LOGI("Uploaded %d out of %d bytes....\n", bytesTransfered, totalSize);
    GSI_UNUSED(sake);
    GSI_UNUSED(userData);
}

static void UploadContentCompletedCallback(SAKE sake, SAKERequestResult result, gsi_i32 fileid, SAKEFileResult fileResult, void *userData)
{

    gUploadResult = GHTTPFalse;
    NumOperations--;

    LOGI("============ Upload Content Response START =============\n");

    if(HandleRequestResult(result, "UploadContent") == gsi_false)
    {
    	LOGI("Upload Content failed .\n");
    }
    else
    {
        if(fileid != 0)
        {
            sakeTestImageData.gUploadedFileId = fileid;
            LOGI("Success - uploaded File ID %d \n", fileid);
            gUploadResult = GHTTPTrue;
        }
        else
        {
        	LOGI("Update of file has failed!!!!, file id is 0, file result %d\n", fileResult);
            results[gIndex].pass = gsi_false;
            sprintf(results[gIndex].errString, "Error - file id is 0, file result %d", fileResult);
        }
    }
    LOGI("============ Upload Content Response END  =============\n");
   
    GSI_UNUSED(userData);
    GSI_UNUSED(sake);
}




static void DownloadContentProgressCallback(SAKE sake, 
                                            gsi_u32 bytesTransfered, 
                                            gsi_u32 totalSize, 
                                            void *userData)
{
	LOGI("Downloaded %d out of %d bytes....\n", bytesTransfered, totalSize);
    GSI_UNUSED(sake);
    GSI_UNUSED(userData);
}

static void DownloadContentCompletedCallback(SAKE     sake, 
                                             SAKERequestResult result,
                                             SAKEFileResult fileResult, 
                                             char *buffer, 
                                             gsi_i32  bufferLength, 
                                             void     *userData)
{

    sprintf(results[gIndex].requestType, "DownloadContent");
    NumOperations--;
    LOGI("============ Download Content Response START =============\n");

    if(HandleRequestResult(result, "DownloadContent") == gsi_false)
    {
    	LOGI("Download Content failed, file result = %d .\n", fileResult);
    }
    else
    {

    	LOGI("Successfully downloaded file!!\n");
        if (buffer != NULL && bufferLength > 0)
        {
            // we must have asked for a download to memory 
            // so update the sake image test data with it.
            sakeTestImageData.gImageBuffer.mBuffer = buffer;
            sakeTestImageData.gImageBuffer.mLength = bufferLength;
        }
        results[gIndex].pass = gsi_true;
    }
    LOGI("============ Download Content Response END  =============\n");

    GSI_UNUSED(userData);
    GSI_UNUSED(sake);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void ReportRecord(SAKEReportRecordInput * input)
{
	SAKERequest request;
	SAKEStartRequestResult startRequestResult;

	LOGI("%d) Starting ReportRecord ================\n", gIndex+1);

	request = sakeReportRecord(sake, input, ReportRecordCallback, NULL);
	if(!request)
	{
		startRequestResult = sakeGetStartRequestResult(sake);
		sprintf(results[gIndex].errString, "Failed to start request: %d", startRequestResult);
		results[gIndex].pass = gsi_false;
	}
	else
	{
		NumOperations++;
	}

	ProcessTasks();
	gIndex++; //increment global task counter
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int CreateRecord(SAKECreateRecordInput * input)
{
		SAKERequest request;
		SAKEStartRequestResult startRequestResult;
	int recordid = 0;

	LOGI("%d) Starting CreateRecord ================\n", gIndex+1);

	request = sakeCreateRecord(sake, input, CreateRecordCallback, &recordid);
	if(!request)
	{
		startRequestResult = sakeGetStartRequestResult(sake);
	    sprintf(results[gIndex].errString, "Failed to start request: %d", startRequestResult);
	    results[gIndex].pass = gsi_false;
	}
	else
	{
		NumOperations++;
	}

	ProcessTasks();
	gIndex++; //increment global task counter

	return recordid;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void UpdateRecord(SAKEUpdateRecordInput * input)
{
		SAKERequest request;
		SAKEStartRequestResult startRequestResult;
		
		LOGI("%d) Starting UpdateRecord ================\n", gIndex+1);

	request = sakeUpdateRecord(sake, input, UpdateRecordCallback, NULL);
		if(!request)
		{
			startRequestResult = sakeGetStartRequestResult(sake);
		sprintf(results[gIndex].errString, "Failed to start request: %d", startRequestResult);
		results[gIndex].pass = gsi_false;
		}
		else
		{
			NumOperations++;
		}

	ProcessTasks();
	gIndex++; //increment global task counter
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void DeleteRecord(char * table, int recordid)
{
	SAKEDeleteRecordInput input;
		SAKERequest request;
		SAKEStartRequestResult startRequestResult;

	input.mTableId = table;
	input.mRecordId = recordid;

	LOGI("%d) Starting DeleteRecord ================\n", gIndex+1);

		request = sakeDeleteRecord(sake, &input, DeleteRecordCallback, NULL);
		if(!request)
		{
			startRequestResult = sakeGetStartRequestResult(sake);
		sprintf(results[gIndex].errString, "Failed to start request: %d", startRequestResult);
		results[gIndex].pass = gsi_false;
		}
		else
		{
			NumOperations++;
		}

	ProcessTasks();
	gIndex++; //increment global task counter
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void GetMyRecords(SAKEGetMyRecordsInput * input)
{

		static SAKERequest request;
		SAKEStartRequestResult startRequestResult;

		LOGI("%d) Starting GetMyRecords ================\n", gIndex+1);

	request = sakeGetMyRecords(sake, input, GetMyRecordsCallback, NULL);
		if(!request)
		{
			startRequestResult = sakeGetStartRequestResult(sake);
		sprintf(results[gIndex].errString, "Failed to start request: %d", startRequestResult);
		results[gIndex].pass = gsi_false;
		}
		else
		{
			NumOperations++;
		}

	ProcessTasks();
	gIndex++; //increment global task counter
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void SearchForRecords(SAKESearchForRecordsInput * input, const char * testType)
{
		static SAKERequest request;
		SAKEStartRequestResult startRequestResult;
	char buffer[512] = {0};

	if (testType == NULL || strlen(testType) == 0)
		sprintf(buffer, "\r\n%d) SearchForRecordsUnit Test\r\n", gIndex+1);
	else
		sprintf(buffer, "\r\n%d) SearchForRecords: TestType === %s ===\r\n", gIndex+1, testType);
#if defined(NOFILE)
	LOGI(buffer);
#endif


	request = sakeSearchForRecords(sake, input, SearchForRecordsCallback, NULL);
		if(!request)
		{
			startRequestResult = sakeGetStartRequestResult(sake);
		sprintf(results[gIndex].errString, "Failed to start request: %d", startRequestResult);
		results[gIndex].pass = gsi_false;
		}
		else
		{
			NumOperations++;
		}
	ProcessTasks();
	gIndex++; //increment global task counter
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void RateRecord(char * table, int recordid, gsi_u8 rating)
{
		static SAKERateRecordInput input;
		static SAKERequest request;
		SAKEStartRequestResult startRequestResult;

	input.mTableId = table;
	input.mRecordId = recordid;
	input.mRating = rating;

	LOGI("%d) Starting RateRecord ================\n", gIndex+1);

		request = sakeRateRecord(sake, &input, RateRecordCallback, NULL);
		if(!request)
		{
			startRequestResult = sakeGetStartRequestResult(sake);
		sprintf(results[gIndex].errString, "Failed to start request: %d", startRequestResult);
		results[gIndex].pass = gsi_false;
		}
		else
		{
			NumOperations++;
		}

	ProcessTasks();
	gIndex++; //increment global task counter
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void GetRecordLimit(char * table)
{
		static SAKEGetRecordLimitInput input;
		static SAKERequest request;
		SAKEStartRequestResult startRequestResult;

	input.mTableId = table;

	LOGI("%d) Starting GetRecordLimit ================\n", gIndex+1);

		request = sakeGetRecordLimit(sake, &input, GetRecordLimitCallback, NULL);
		if(!request)
		{
			startRequestResult = sakeGetStartRequestResult(sake);
		sprintf(results[gIndex].errString, "Failed to start request: %d", startRequestResult);
		results[gIndex].pass = gsi_false;
		}
		else
		{
			NumOperations++;
		}

	ProcessTasks();
	gIndex++; //increment global task counter
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static int GetCount(char * table, gsi_char * filter, gsi_bool cache)
{
	static SAKEGetRecordCountInput input;
	SAKERequest request;
	SAKEStartRequestResult startRequestResult;
	int count = 0;

	input.mTableId = table;
	input.mFilter = filter;
	input.mCacheFlag = cache;

	LOGI("%d) Starting GetCount ================\n", gIndex+1);

	request = sakeGetRecordCount(sake, &input, GetRecordCountCallback, &count);
	if(!request)
	{
		startRequestResult = sakeGetStartRequestResult(sake);
		sprintf(results[gIndex].errString, "Failed to start request: %d", startRequestResult);
		results[gIndex].pass = gsi_false;
	}
	else
	{
		NumOperations++;
	}

	ProcessTasks();
	gIndex++; //increment global task counter

	return count;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void UploadContent(SAKEContentInfo *contentInfo, gsi_char *remoteFilename)
{
    //GHTTPRequest request;
    gsi_char url[SAKE_MAX_URL_LENGTH];
    SAKEStartRequestResult requestResult;    
    gsi_char        *uploadFilename = NULL;
    SAKEUploadContentInput input;
    
    LOGI("%d) Starting UploadFile ================\n", gIndex+1);

    // get an upload url
    if(sakeGetFileUploadURL(sake, url))
        _tprintf(_T("Upload URL: ") GS_USTR _T("\n"), url);
    else
    	LOGI("Failed to get upload url!\n");
    

    // Reset the global uploaded file id
    sakeTestImageData.gUploadedFileId = 0;
    if (remoteFilename == NULL)
    {
        // we must me uploading from disk then check the local file name
        input.remoteFileName = contentInfo->mStorage.mFile; 
    }
    else
    {
        input.remoteFileName = remoteFilename ;
    }
    input.content = contentInfo;
    input.transferBlocking = gsi_false;
    input.progressCallback = UploadContentProgressCallback;

    // Upload from disk  
    _tprintf(_T("Uploading content. \n\t\tRemote File Name : ") GS_USTR _T("\n"), uploadFilename);
    requestResult = sakeUploadContent( sake, 
//                                 uploadFilename, 
//                                 contentInfo, 
//                                 gsi_false,
// 
//                                 UploadContentProgressCallback,
                                &input,
                                UploadContentCompletedCallback,  
                                NULL);
    if(requestResult != SAKEStartRequestResult_SUCCESS)
    {
        _tprintf(_T("Failed: Could not upload file. Check if file ") GS_USTR _T(" exists\n"), uploadFilename );
        sprintf(results[gIndex].errString, "Failed to start request: %d", requestResult);
        results[gIndex].pass = gsi_false;
    }
    else
    {
        NumOperations++;
    }

    ProcessTasks();

    gIndex++; //increment global task counter

}



static int DownloadContent(SAKEContentInfo *content)
{
    gsi_char url[SAKE_MAX_URL_LENGTH];
    SAKEStartRequestResult requestResult;
    SAKEDownloadContentInput input;
    if(gUploadResult)
    {
    	LOGI("%d) Starting DownloadContent ================\n", gIndex+1);

        // get a download url
        if(sakeGetFileDownloadURL(sake, content->mFileid, url))
            _tprintf(_T("Download URL: ") GS_USTR _T("\n"), url);
        else
        	LOGI("Failed to get download url!\n");

        //Download to memory
        // Initialize the file id to be downloaded
        content->mFileid = sakeTestImageData.gUploadedFileId;
        sakeTestImageData.gImageBuffer.mBuffer = NULL;
        sakeTestImageData.gImageBuffer.mLength = 0;

        input.content = content;
        input.transferBlocking = gsi_false;
        input.progressCallback = DownloadContentProgressCallback;

        LOGI("Download content to memory. \n\t\tFile Id : %d \n", sakeTestImageData.gUploadedFileId);
        requestResult = sakeDownloadContent(sake, 
                                    &input,
                                    DownloadContentCompletedCallback, 
                                    NULL);
        if(requestResult != SAKEStartRequestResult_SUCCESS)
        {
        	LOGI("Failed: Could not download file. Check if file %d exists\n", sakeTestImageData.gUploadedFileId );
            sprintf(results[gIndex].errString, "Failed to start request: %d", requestResult);
            results[gIndex].pass = gsi_false;
        }
        else
        {
            NumOperations++;
        }
    }
    ProcessTasks();
    gIndex++; //increment global task counter
    return requestResult;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void RunFullTests()
{
	// FULL rundown of generic tests of all the functionality
    //static SAKESearchForRecordsInput    searchInput;
	static SAKECreateRecordInput		createInput;
	static SAKEReportRecordInput		reportInput;
	static SAKEUpdateRecordInput		updateInput;
	static SAKEGetMyRecordsInput		myInput;
	static SAKEField field;
 	int recordid = 0;
  	int cachedCount = 0;
    int count = 0;
 	int numOwned = 0;

	//int numOwnerIds = 0;
#ifndef GSI_UNICODE
	//char *utf8Filter;
#endif

 	char * table = "testDontChangeThis";	// all of the full tests should be done on the test table for ease

#ifndef GSI_UNICODE
	//utf8Filter = UCS2ToUTF8StringAlloc((UCS2String)L"MyUnicodeString LIKE N'uni%'");
#endif
	//#1

	// *** report a record in the Content Moderation test table ***
	reportInput.mTableId = "CM_Test";
	reportInput.mReason = "ReportRecord testing..";
	reportInput.mRecordId = 1;
	reportInput.mReasonCode = 0;
	ReportRecord(&reportInput);

	cachedCount = GetCount(table, NULL, gsi_true); //get count before creation - cached
	count = GetCount(table, NULL, gsi_false); //get count before creation - cached
	GetRecordLimit(table); // Get record limit too (for num owned)
	numOwned = gNumOwned;
    if (cachedCount != count)
        ReportFailure("cached count didn't work");

	// *** create it ***
	createInput.mTableId = table;
	field.mName = "MyAsciiString";
	field.mType = SAKEFieldType_ASCII_STRING;
	field.mValue.mAsciiString = "this is a record";
	createInput.mFields = &field;
	createInput.mNumFields = 1;
	recordid = CreateRecord(&createInput);

	cachedCount = GetCount(table, NULL, gsi_true); //get count after creation - still cached

	GetRecordLimit(table); // Get record limit again

	// verify the new record was created
	if (numOwned == gNumOwned)
		ReportFailure("record not created");		


	// *** update and rate it ***
	{
		static char binaryData[] = "\x12\x34\x56\xAB\xCD";


        static gsi_u16 unicodeString[] = {'u','n','i','c','o','d','e','\0'};
		static SAKEField fields[16];
		int index = 0;
		updateInput.mTableId = table;
		updateInput.mRecordId = recordid;
		fields[index].mName = "MyByte";
		fields[index].mType = SAKEFieldType_BYTE;
		fields[index].mValue.mByte = 123;
		index++;
		fields[index].mName = "MyShort";
		fields[index].mType = SAKEFieldType_SHORT;
		fields[index].mValue.mShort = 12345;
		index++;
		fields[index].mName = "MyInt";
		fields[index].mType = SAKEFieldType_INT;
		fields[index].mValue.mInt = 987654321;
		index++;
		fields[index].mName = "MyFloat";
		fields[index].mType = SAKEFieldType_FLOAT;
		fields[index].mValue.mFloat = 3.14159265f;
		index++;
		fields[index].mName = "MyAsciiString";
		fields[index].mType = SAKEFieldType_ASCII_STRING;
		fields[index].mValue.mAsciiString = "ascii";
		index++;
		fields[index].mName = "MyUnicodeString";
		fields[index].mType = SAKEFieldType_UNICODE_STRING;
		fields[index].mValue.mUnicodeString = unicodeString;
		index++;
		fields[index].mName = "MyBoolean";
		fields[index].mType = SAKEFieldType_BOOLEAN;
		fields[index].mValue.mBoolean = gsi_true;
		index++;
		fields[index].mName = "MyDateAndTime";
		fields[index].mType = SAKEFieldType_DATE_AND_TIME;
		fields[index].mValue.mDateAndTime = time(NULL);
		index++;
		fields[index].mName = "MyBinaryData";
		fields[index].mType = SAKEFieldType_BINARY_DATA;
		fields[index].mValue.mBinaryData.mValue = (gsi_u8*) binaryData;
		fields[index].mValue.mBinaryData.mLength = (int)strlen(binaryData);
		index++;
		updateInput.mFields = fields;
		updateInput.mNumFields = index;


		UpdateRecord(&updateInput);

	}

	RateRecord(table, recordid, 133);
	
	// *** retrieval ***
	{
		static char *fieldNames[] = { "recordid", "ownerid", "MyByte", "MyShort",
		                              "MyInt", "MyFloat", "MyAsciiString",
		                              "MyUnicodeString", "MyBoolean", "MyDateAndTime",
		                              "MyBinaryData", "MyFileID" };
		myInput.mTableId = table;
		myInput.mFieldNames = fieldNames;
		myInput.mNumFields = (sizeof(fieldNames) / sizeof(fieldNames[0]));

		GetMyRecords(&myInput);
	} 
    DeleteRecord(table,recordid );

    {
		SAKESearchForRecordsInput mySearch = {	NULL, 
                                                NULL, 
                                                0, 
                                                NULL, 
												NULL, 
                                                0, 
                                                0, 
												NULL, 
												0, 
												NULL, 
                                                0, 
												gsi_true}; 
        mySearch.mTableId   = table;
        mySearch.mNumFields = 1;
        mySearch.mFieldNames = (char **) gsimalloc(sizeof(char *)*mySearch.mNumFields );
        mySearch.mFieldNames[0] = "MyInt";
        mySearch.mFilter = _T("MyInt = 987654321");
        mySearch.mNumOwnerIds = 1;
        mySearch.mOwnerIds = gsimalloc(sizeof(int)*mySearch.mNumOwnerIds );
        mySearch.mOwnerIds[0] = profileid ;   
        mySearch.mMaxRecords = 1;
        SearchForRecords(&mySearch, NULL);
    }


    {
            SAKEContentInfo content;
            char *data = gsimalloc(SAKE_UPLOAD_AMOUNT);

            memset(data, '\0',SAKE_UPLOAD_AMOUNT );
            memset(data, 'a', SAKE_UPLOAD_AMOUNT-1);

            content.mFileid = 0;
            content.mType   = SAKEContentStorageType_MEMORY;
            content.mStorage.mMemory = (SAKEContentBuffer *)gsimalloc(sizeof(SAKEContentBuffer));
            content.mStorage.mMemory->mBuffer = data;
            content.mStorage.mMemory->mLength = SAKE_UPLOAD_AMOUNT;

            UploadContent(&content, _T("myfile.txt"));

            // de-allocate the test data
            gsifree(content.mStorage.mMemory->mBuffer);
            gsifree(content.mStorage.mMemory);
    }

    if (sakeTestImageData.gUploadedFileId != 0) 
    {
			// for downloading to memory if NOFILE is defined
			char * downloadBuffer;

			// Download to either disk or memory
            SAKEContentInfo content2;
            content2.mFileid = sakeTestImageData.gUploadedFileId;
#ifndef NOFILE
            content2.mType   = SAKEContentStorageType_DISK;
			content2.mStorage.mFile = _T("./myfiledownload.txt");
			
			DownloadContent(&content2);

			GSI_UNUSED(downloadBuffer);
#else
			downloadBuffer = gsimalloc(SAKE_DOWNLOAD_AMOUNT);

			content2.mType = SAKEContentStorageType_MEMORY;
			content2.mStorage.mMemory = (SAKEContentBuffer *)gsimalloc(sizeof(SAKEContentBuffer));
			content2.mStorage.mMemory->mBuffer = downloadBuffer;
			content2.mStorage.mMemory->mLength = SAKE_DOWNLOAD_AMOUNT;

			DownloadContent(&content2);

			// de-allocate the test data
			gsifree(content2.mStorage.mMemory->mBuffer);
			gsifree(content2.mStorage.mMemory);
#endif
    } 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void android_main(struct android_app* state)
{
    int ident;
    int events;
    struct android_poll_source* source;



	SAKEStartupResult startupResult;

	// Make sure glue isn't stripped.
    app_dummy();

	// Set Debug Level for GameSpy SDK Layer
    gsSetDebugLevel(GSIDebugCat_All, GSIDebugType_All, GSIDebugLevel_Hardcore);
	// availability check
	LOGI("Checking availability\n");
	if(CheckServices() != GSIACAvailable)
		return;

	// Init the GameSpy SDK Core (required by SOAP SDKs)
	LOGI("Initializing the GameSpy Core\n");
	gsCoreInitialize();

	// startup sake
	LOGI("Starting up Sake\n");
	startupResult = sakeStartup(&sake);
	if(startupResult != SAKEStartupResult_SUCCESS)
	{
		LOGI("Startup failed: %d\n", startupResult);
		return;
	}

	LoginAndAuthenticate();


	// run the tests
	RunFullTests();

	// keep thinking
	LOGI("Finalizing any incomplete tasks\n");
	ProcessTasks();

	// print out final results
	PrintResults();
	LOGI("Finalizing any incomplete tasks\n");
    //getc(stdin);

    //---------------- Event Handler -----------------------
	while ((ident=ALooper_pollAll(-1, NULL, &events, (void**)&source)) >= 0)
	{
		LOGI("Entering the event-handler");
		// Process this event.
		if (source != NULL)
		{
			source->process(state, source);
		}
		// If a sensor has data, process it now.
		if (ident == LOOPER_ID_USER)
		{
			LOGI("Processing sensor events");
		}
		// Check if we are exiting.
		if (state->destroyRequested != 0)
		{
			// shutdown sake
			LOGI("Shutting down Sake\n");
			sakeShutdown(sake);

			// cleanup any specific http requests
			ghttpCleanup();

			// shutdown the core
			LOGI("Shutting down the GameSpy Core\n");
			gsCoreShutdown();

		    // Wait for core shutdown
		    //   (should be instantaneous unless you have multiple cores)
		    while(gsCoreIsShutdown() == GSCore_SHUTDOWN_PENDING)
		    {
		        gsCoreThink(0);
		        msleep(5);
		    }
			return;
		}
		msleep(10);
	}
    //---------------- Event Handler -----------------------
	return;
}
