///////////////////////////////////////////////////////////////////////////////
// File:	sakeRequestMisc.c
// SDK:		GameSpy Sake Persistent Storage SDK
//
// Copyright (c) 2012 GameSpy Technology & IGN Entertainment, Inc.  All rights 
// reserved. This software is made available only pursuant to certain license 
// terms offered by IGN or its subsidiary GameSpy Industries, Inc.  Unlicensed
// use or use in a manner not expressly authorized by IGN or GameSpy Technology
// is prohibited.

#include "sakeRequestInternal.h"
#include "sakeRequest.h"


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Rate Record
static SAKEStartRequestResult SAKE_CALL sakeiRateRecordValidateInput(SAKERequest request)
{
	SAKERateRecordInput *input = (SAKERateRecordInput *)request->mInput;

	// Check the tableid.
	if(!input->mTableId)
		return SAKEStartRequestResult_BAD_TABLEID;

	return SAKEStartRequestResult_SUCCESS;
}

static SAKEStartRequestResult SAKE_CALL sakeiRateRecordFillSoapRequest(SAKERequest request)
{
	SAKERateRecordInput *input = (SAKERateRecordInput *)request->mInput;

	// Write the table id.
	gsXmlWriteStringElement(request->mSoapRequest, GSI_SAKE_SERVICE_NAMESPACE, "tableid", input->mTableId);

	// Write the recordid.
	gsXmlWriteIntElement(request->mSoapRequest, GSI_SAKE_SERVICE_NAMESPACE, "recordid", (gsi_u32)input->mRecordId);

	// Write the rating.
	gsXmlWriteIntElement(request->mSoapRequest, GSI_SAKE_SERVICE_NAMESPACE, "rating", (gsi_u32)input->mRating);

	return SAKEStartRequestResult_SUCCESS;
}

static SAKERequestResult sakeiRateRecordProcessSoapResponse(SAKERequest request)
{
	SAKERateRecordOutput *output = (SAKERateRecordOutput *)request->mOutput;

	if(gsi_is_false(gsXmlReadChildAsInt(request->mSoapResponse, "numRatings", &output->mNumRatings)) ||
		gsi_is_false(gsXmlReadChildAsFloat(request->mSoapResponse, "averageRating", &output->mAverageRating)))
	{
		return SAKERequestResult_MALFORMED_RESPONSE;
	}

	return SAKERequestResult_SUCCESS;
}

SAKEStartRequestResult SAKE_CALL sakeiStartRateRecordRequest(SAKERequest request)
{
	static SAKEIRequestInfo info =
	{
		sizeof(SAKERateRecordOutput),
		SAKEI_FUNC_NAME_STRINGS("RateRecord"),
		sakeiRateRecordValidateInput,
		sakeiRateRecordFillSoapRequest,
		sakeiRateRecordProcessSoapResponse,
		NULL,
        NULL
	};

	return sakeiStartRequest(request, &info);
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Get Record Limit

static SAKEStartRequestResult SAKE_CALL sakeiGetRecordLimitValidateInput(SAKERequest request)
{
	SAKEGetRecordLimitInput *input = (SAKEGetRecordLimitInput *)request->mInput;

	// Check the tableid.
	if(!input->mTableId)
		return SAKEStartRequestResult_BAD_TABLEID;

	return SAKEStartRequestResult_SUCCESS;
}

static SAKEStartRequestResult SAKE_CALL sakeiGetRecordLimitFillSoapRequest(SAKERequest request)
{
	SAKEGetRecordLimitInput *input = (SAKEGetRecordLimitInput *)request->mInput;

	// Write the table id.
	gsXmlWriteStringElement(request->mSoapRequest, GSI_SAKE_SERVICE_NAMESPACE, "tableid", input->mTableId);

	return SAKEStartRequestResult_SUCCESS;
}

static SAKERequestResult sakeiGetRecordLimitProcessSoapResponse(SAKERequest request)
{
	SAKEGetRecordLimitOutput *output = (SAKEGetRecordLimitOutput *)request->mOutput;

	if(gsi_is_false(gsXmlReadChildAsInt(request->mSoapResponse, "limitPerOwner", &output->mLimitPerOwner)) ||
		gsi_is_false(gsXmlReadChildAsInt(request->mSoapResponse, "numOwned", &output->mNumOwned)))
	{
		return SAKERequestResult_MALFORMED_RESPONSE;
	}

	return SAKERequestResult_SUCCESS;
}

SAKEStartRequestResult SAKE_CALL sakeiStartGetRecordLimitRequest(SAKERequest request)
{
	static SAKEIRequestInfo info =
	{
		sizeof(SAKEGetRecordLimitOutput),
		SAKEI_FUNC_NAME_STRINGS("GetRecordLimit"),
		sakeiGetRecordLimitValidateInput,
		sakeiGetRecordLimitFillSoapRequest,
		sakeiGetRecordLimitProcessSoapResponse,
		NULL,
        NULL
	};

	return sakeiStartRequest(request, &info);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Get Record Count

static SAKEStartRequestResult SAKE_CALL sakeiGetRecordCountValidateInput(SAKERequest request)
{
	SAKEGetRecordCountInput *input = (SAKEGetRecordCountInput *)request->mInput;

	// Check the tableid.
	if(!input->mTableId)
		return SAKEStartRequestResult_BAD_TABLEID;

	return SAKEStartRequestResult_SUCCESS;
}

static SAKEStartRequestResult SAKE_CALL sakeiGetRecordCountFillSoapRequest(SAKERequest request)
{
	SAKEGetRecordCountInput *input = (SAKEGetRecordCountInput *)request->mInput;

	// Write the table id.
	gsXmlWriteStringElement(request->mSoapRequest, GSI_SAKE_SERVICE_NAMESPACE, "tableid", input->mTableId);

	// Write the filter.
	if(input->mFilter != NULL)
		gsXmlWriteTStringElement(request->mSoapRequest, GSI_SAKE_SERVICE_NAMESPACE, "filter", input->mFilter);

	// Write the cache flag.
	gsXmlWriteIntElement(request->mSoapRequest, GSI_SAKE_SERVICE_NAMESPACE, "cacheFlag", (gsi_u32)input->mCacheFlag);


	return SAKEStartRequestResult_SUCCESS;
}

static SAKERequestResult sakeiGetRecordCountProcessSoapResponse(SAKERequest request)
{
	SAKEGetRecordCountOutput *output = (SAKEGetRecordCountOutput *)request->mOutput;

    if(gsi_is_false(gsXmlReadChildAsInt(request->mSoapResponse, "count", &output->mCount)))
    {
        return SAKERequestResult_MALFORMED_RESPONSE;
    }

    return SAKERequestResult_SUCCESS;

}


SAKEStartRequestResult SAKE_CALL sakeiStartGetRecordCountRequest(SAKERequest request)
{
	static SAKEIRequestInfo info =
	{
		sizeof(SAKEGetRandomRecordOutput),
		SAKEI_FUNC_NAME_STRINGS("GetRecordCount"),
		sakeiGetRecordCountValidateInput,
		sakeiGetRecordCountFillSoapRequest,
		sakeiGetRecordCountProcessSoapResponse,
		NULL,
        NULL
	};

	return sakeiStartRequest(request, &info);
}


