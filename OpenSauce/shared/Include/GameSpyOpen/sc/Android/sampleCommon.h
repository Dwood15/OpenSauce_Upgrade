///////////////////////////////////////////////////////////////////////////////
// File:	sampleCommon.h
// SDK:		GameSpy Authentication Service SDK
//
// Copyright (c) IGN Entertainment, Inc.  All rights reserved.  
// This software is made available only pursuant to certain license terms offered
// by IGN or its subsidiary GameSpy Industries, Inc.  Unlicensed use or use in a 
// manner not expressly authorized by IGN or GameSpy is prohibited.

// ATLAS Related Includes
#include "../../common/gsCommon.h"
#include "../../common/gsAvailable.h"
#include "../../webservices/AuthService.h"
#include "../../common/gsCore.h"
#include "../../sc/sci.h"
#include "../../sc/sc.h"

// Android Native Activity Includes
#include <jni.h>
#include <errno.h>
#include <android/log.h>
#include <android_native_app_glue.h>

// Android logs
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "YOUR_TAG", __VA_ARGS__))

static const char * SAMPLE_GAMENAME = "atlasSamples";
static const int SAMPLE_GAME_ID = 1649;
static const char * SAMPLE_SECRET_KEY = "Zc0eM6";
static const char * SAMPLE_ACCESS_KEY = "39cf9714e24f421c8ca07b9bcb36c0f5";

// user credentials
static char * SAMPLE_UNIQUENICK[3] = {"gsSampleName", "gsSampleName2", "gsSampleName3"};
static const char * SAMPLE_PASSWORD = "gspy";

GSLoginCertificate certificate;
GSLoginPrivateData privateData;
GSLoginCertificate certificates[3];
GSLoginPrivateData privateDatas[3];

gsi_bool AvailabilityCheck();

void SampleAuthenticationCallback(GHTTPResult httpResult, WSLoginResponse * theResponse, void * theUserData);

gsi_bool SampleAuthenticatePlayer(const char * uniqueNick, const char * password);

gsi_bool AuthSetup();

void CoreCleanup();

gsi_bool AtlasReportingSetup(SCInterfacePtr * statsInterface, int playerIndex, char * sessionId);
gsi_bool AtlasQuerySetup(SCInterfacePtr * statsInterface);
void DisplayAtlasPlayerRecords(SCPlayerStatsQueryResponse * response);
void AtlasCleanup(SCInterfacePtr statsInterface);

void WaitForUserInput();
