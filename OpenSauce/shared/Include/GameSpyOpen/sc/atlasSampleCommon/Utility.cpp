// Copyright (c) IGN Entertainment, Inc.  All rights reserved.  
// This software is made available only pursuant to certain license terms offered
// by IGN or its subsidiary GameSpy Industries, Inc.  Unlicensed use or use in a 
// manner not expressly authorized by IGN or GameSpy is prohibited.

#include "../../common/gsCommon.h"
#include "../sc.h"
#include "Utility.h"

char *SCResultToString(SCResult result)
{
	switch (result)
	{
	case SCResult_NO_AVAILABILITY_CHECK:
		return "The GameSpy Availability Check was not performed prior to initializing the ATLAS SDK.";
	case SCResult_INVALID_PARAMETERS:
		return "Parameters passed to interface function were invalid.";
	case SCResult_NOT_INITIALIZED:
		return "ATLAS SDK  was not initialized.";
	case SCResult_CORE_NOT_INITIALIZED:
		return "The core was not initialized by the application prior to initializing the ATLAS SDK.";
	case SCResult_OUT_OF_MEMORY:
		return "ATLAS SDK  could not allocate memory for its resources.";
	case SCResult_CALLBACK_PENDING:
		return "An operation is still pending.";
	case SCResult_HTTP_ERROR:
		return "There was a lower level HTTP error that occurred.  Please check HTTP result.";
	case SCResult_UNKNOWN_RESPONSE:
		return "ATLAS SDK cannot understand the code in the response from the backend.";
	case SCResult_RESPONSE_INVALID:
		return "ATLAS SDK cannot read the body of the response from the backend.";
	case SCResult_INVALID_DATATYPE:
		return "Error with data type?";
	case SCResult_REPORT_INCOMPLETE:
		return "The report sent is incomplete.";
	case SCResult_REPORT_INVALID:
		return "Part or all of report is invalid.";
	case SCResult_SUBMISSION_FAILED:
		return "Submission of report failed.";
	case SCResult_QUERY_DISABLED:
		return "Query id is disabled on the Backend Administration site.";
	case SCResult_QUERY_TYPE_MISMATCH:
		return "Query id passed is used for the wrong query type.";
	case SCResult_QUERY_INVALID:
		return "Query id is invalid or not found.  Please check text message for details.";
	case SCResult_QUERY_PARAMS_MISSING:
		return "Parameter or parameters for the specified query are missing.";
	case SCResult_QUERY_PARAMS_TOO_MANY:
		return "The number of parameters exceeds the expected number.";
	case SCResult_QUERY_PARAM_TYPE_INVALID:
		return "Parameter value that is passed does not match what the query expects.";
	case SCResult_INVALID_GAMEID:
		return "The gameid as passed to wsSetGameCredentials is invalid.";
	case SCResult_INVALID_SESSIONTOKEN:
		return "The Session Token generated from your access key is getting corrupted in the request headers.";
	case SCResult_SESSIONTOKEN_EXPIRED:
		return "The Session Token generated from your access key has expired - you need to re-authenticate via AuthService.";
	default:
		return "Error unknown to SDK. Please contact devsupport.";
	}	
}

void SetDebugLogPreferences()
{
	gsSetDebugFile(stdout);
#if _DEBUG
	gsSetDebugLevel(GSIDebugCat_All, GSIDebugType_All, GSIDebugLevel_Debug);
#else
	gsSetDebugLevel(GSIDebugCat_All, GSIDebugType_All, GSIDebugLevel_Normal);
#endif
}