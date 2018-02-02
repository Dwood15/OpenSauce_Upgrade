///////////////////////////////////////////////////////////////////////////////
// File:	sakeRequest.h
// SDK:		GameSpy Sake Persistent Storage SDK
//
// Copyright (c) 2012 GameSpy Technology & IGN Entertainment, Inc.  All rights 
// reserved. This software is made available only pursuant to certain license 
// terms offered by IGN or its subsidiary GameSpy Industries, Inc.  Unlicensed
// use or use in a manner not expressly authorized by IGN or GameSpy Technology
// is prohibited.

#ifndef __SAKEREQUEST_H__
#define __SAKEREQUEST_H__

#include "sakeMain.h"
#include "sakeRequestInternal.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus)
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define GSI_SAKE_SERVICE_NAMESPACE_COUNT     1
#define GSI_SAKE_SERVICE_NAMESPACE           "ns1"
#define GSI_SAKE_SERVICE_NAMESPACE_URL       "http://gamespy.net/sake"
extern const char * GSI_SAKE_SERVICE_NAMESPACES[GSI_SAKE_SERVICE_NAMESPACE_COUNT];
#define SAKEI_SOAP_URL_FORMAT   "https://" GSI_OPEN_DOMAIN_NAME "/SakeStorageServer/Public/StorageServer.asmx"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef enum{
	SAKEIRequestType_CREATE_RECORD,
	SAKEIRequestType_UPDATE_RECORD,
	SAKEIRequestType_DELETE_RECORD,
	SAKEIRequestType_SEARCH_FOR_RECORDS,
	SAKEIRequestType_GET_MY_RECORDS,
	SAKEIRequestType_GET_SPECIFIC_RECORDS,
	SAKEIRequestType_GET_RANDOM_RECORD,
	SAKEIRequestType_RATE_RECORD,
	SAKEIRequestType_GET_RECORD_LIMIT,
	SAKEIRequestType_GET_RECORD_COUNT,
	SAKEIRequestType_REPORT_RECORD
} SAKEIRequestType;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef struct SAKERequestInternal{
	SAKE                mSake;
	SAKEIRequestType    mType;
	void                *mInput;
	void                *mOutput;
	SAKERequestCallback mCallback;
	void                *mUserData;
	GSXmlStreamWriter   mSoapRequest;
	GSXmlStreamWriter   mSoapResponse;
	SAKEIRequestInfo    *mInfo;
} SAKERequestInternal;


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SAKERequest SAKE_CALL sakeiInitRequest(
	SAKE				sake, 
	SAKEIRequestType	type, 
	void				*input, 
	SAKERequestCallback callback, 
	void				*userData
);
void SAKE_CALL sakeiFreeRequest(SAKERequest request);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SAKEStartRequestResult SAKE_CALL sakeiStartReportRecordRequest(SAKERequest request);
SAKEStartRequestResult SAKE_CALL sakeiStartCreateRecordRequest(SAKERequest request);
SAKEStartRequestResult SAKE_CALL sakeiStartUpdateRecordRequest(SAKERequest request);
SAKEStartRequestResult SAKE_CALL sakeiStartDeleteRecordRequest(SAKERequest request);
SAKEStartRequestResult SAKE_CALL sakeiStartSearchForRecordsRequest(SAKERequest request);
SAKEStartRequestResult SAKE_CALL sakeiStartGetMyRecordsRequest(SAKERequest request);
SAKEStartRequestResult SAKE_CALL sakeiStartGetSpecificRecordsRequest(SAKERequest request);
SAKEStartRequestResult SAKE_CALL sakeiStartGetRandomRecordRequest(SAKERequest request);
SAKEStartRequestResult SAKE_CALL sakeiStartRateRecordRequest(SAKERequest request);
SAKEStartRequestResult SAKE_CALL sakeiStartGetRecordLimitRequest(SAKERequest request);
SAKEStartRequestResult SAKE_CALL sakeiStartGetRecordCountRequest(SAKERequest request);


SAKEStartRequestResult SAKE_CALL sakeiDownloadContent(
	SAKE									sake, 
    SAKEDownloadContentInput				*input, 
    SAKEDownloadContentCompletedCallback	completedCallback, 
    void									*userData 
);
                                                      
SAKEStartRequestResult SAKE_CALL sakeiUploadContent( 
	SAKE								sake, 
    SAKEUploadContentInput				*input, 
    SAKEUploadContentCompletedCallback	completedCallback, 
    void								*userData 
);

#if defined(__cplusplus)
} // extern "C"
#endif


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#endif // __SAKEREQUEST_H__
