///////////////////////////////////////////////////////////////////////////////
// File:	authtest.c
// SDK:		GameSpy Authentication Service SDK
//
// Copyright (c) IGN Entertainment, Inc.  All rights reserved.  
// This software is made available only pursuant to certain license terms offered
// by IGN or its subsidiary GameSpy Industries, Inc.  Unlicensed use or use in a 
// manner not expressly authorized by IGN or GameSpy is prohibited.
#include <jni.h>
#include <errno.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include "../../../../qr2/qr2.h"
#include "../../../../natneg/natneg.h"
#include "../../../../common/gsAvailable.h"
#include "../../../../common/gsCommon.h"
#include "../../../../common/gsCore.h"
#include "../../../../common/gsAvailable.h"
#include "../../../../GP/gp.h"
#include "../../../AuthService.h"


// Android logs
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "YOUR_TAG", __VA_ARGS__))

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Hard-coded info for login credentials
#define AUTHTEST_NICK               _T("sctest01")
#define AUTHTEST_EMAIL              _T("sctest@gamespy.com")
#define AUTHTEST_PASSWORD           _T("gspy")
#define AUTHTEST_TOKEN		        _T("GMTy13lsJmiY7L19ojyN3XTM08ll0C4EWWijwmJyq3ttiZmoDUQJ0OSnar9nQCu5MpOGvi4Z0EcC2uNaS4yKrUA+h+tTDDoJHF7ZjoWKOTj00yNOEdzWyG08cKdVQwFRkF+h8oG/Jd+Ik3sWviXq/+5bhZQ7iXxTbbDwNL6Lagp/pLZ9czLnYPhY7VEcoQlx9oO")
#define AUTHTEST_CHALLENGE  	    _T("LH8c.DLe")

// Type of login tests
#define AUTHTEST_LOGIN_PROFILE		0
#define AUTHTEST_LOGIN_UNIQUE		1
#define AUTHTEST_LOGIN_REMOTEAUTH	2

// GAME IDENTIFIERS - assigned by GameSpy
#define AUTHTEST_GAMENAME           _T("gmtest")
#define AUTHTEST_GAME_ID            0
#define SECRET_KEY	"HA6zkS"
#define ACCESS_KEY     "39cf9714e24f421c8ca07b9bcb36c0f5"

#define AUTHTEST_TIMEOUT            10000
#define BUDDYSYNC_DELAY             5000

#define GAMESPY_DEFAULT_SERVICE_ID	"WM0002-NPXM00002_00"
#define CHECK_GP_RESULT(func, errString) if(func != GP_NO_ERROR) { printf("%s\n", errString); exit(0); }


// Globals
gsi_u32 gWaitCount = 0;
gsi_u32 gNumPassed = 0;
gsi_u32 gNumTests = 0;
char gRemoteAuthToken[GP_AUTHTOKEN_LEN];         
char gPartnerChallenge[GP_PARTNERCHALLENGE_LEN];

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static gsi_bool isBackendAvailable()
{
    // Do Availability Check - Make sure backend is available
    /////////////////////////////////////////////////////////
    GSIACResult aResult = GSIACWaiting; 
    GSIStartAvailableCheck(AUTHTEST_GAMENAME);
    while(aResult == GSIACWaiting)
    {
        aResult = GSIAvailableCheckThink();
        msleep(5);
    }

    if (aResult == GSIACUnavailable)
    {
        LOGI("Online Services for AuthTest are no longer available\n");

        return gsi_false;
    }

    if (aResult == GSIACTemporarilyUnavailable)
    {
        LOGI("Online Services for AuthTest are temporarily down for maintenance\n");
        return gsi_false;
    }

    return gsi_true;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void myLoginCallback(GHTTPResult httpResult, WSLoginResponse * theResponse, void * theUserData)
{
    if (httpResult != GHTTPSuccess)
        LOGI("* %s * Failed on player login, HTTP error: %s (%d)\n", (char*)theUserData, ghttpResultString(httpResult), httpResult);
    else if (theResponse->mLoginResult != WSLogin_Success)
		LOGI("* %s * Failed on player login, Login result: %s (%d)\n", (char*)theUserData, wsLoginValueString(theResponse->mLoginResult), theResponse->mLoginResult);
    else //Login worked!
    {
        _tprintf(_T("* %s * Player '%s' logged in.\n"), (char*)theUserData, theResponse->mCertificate.mUniqueNick);
        gNumPassed++;

        // validate certificate
        if (wsLoginCertIsValid(&theResponse->mCertificate))
        	LOGI("GSLoginCertificate is valid!\n");
        else
        	LOGI("GSLoginCertificate is *INVALID*!\n");
    }

    gWaitCount--;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void myPlayerLogin(gsi_u8 logintype, const gsi_char * nick, const gsi_char * password)
{
    gsi_u32 result;

    switch(logintype)
    {
        case AUTHTEST_LOGIN_PROFILE:
            // login using default partnercode, namespaceid
            result = wsLoginProfile(AUTHTEST_GAME_ID, WSLogin_PARTNERCODE_GAMESPY, WSLogin_NAMESPACE_SHARED_UNIQUE, 
                nick, AUTHTEST_EMAIL, password, _T(""), myLoginCallback, _T("wsLoginProfile"));
            break;

        case AUTHTEST_LOGIN_UNIQUE:
            // login using default partnercode, namespaceid
            result = wsLoginUnique(AUTHTEST_GAME_ID, WSLogin_PARTNERCODE_GAMESPY, WSLogin_NAMESPACE_SHARED_UNIQUE, 
                nick, password, _T(""), myLoginCallback, _T("wsLoginUnique"));
            break;

        case AUTHTEST_LOGIN_REMOTEAUTH:
            // -- *NOTE* the values passed in here for namespaceid/partnerid are ignored by
            // -- the AuthService for RemoteAuth logins. These values are obtained from the 
            // -- AuthToken itself.
			result = wsLoginRemoteAuth(AUTHTEST_GAME_ID, 0, 0, nick, password, myLoginCallback, _T("wsLoginRemoteAuth"));
            break;

        default:
        	LOGI("Invalid login type\n");
            return;
    }

    gNumTests++;

    if (result != WSLogin_Success)
    {
    	LOGI("Failed on login. Result: %d\n", result);
        return;
    }

    // wait for it to complete
    gWaitCount++;
    while (gWaitCount > 0)
    {
        msleep(10);
        gsCoreThink(0);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Login tests for non-PS3. Executes each one in turn with pre-defined identifiers.
void RunLoginTests(int argc, char *argv[])
{
	wsSetGameCredentials(ACCESS_KEY, AUTHTEST_GAME_ID, SECRET_KEY);

    myPlayerLogin(AUTHTEST_LOGIN_REMOTEAUTH, AUTHTEST_TOKEN, AUTHTEST_CHALLENGE);
    myPlayerLogin(AUTHTEST_LOGIN_UNIQUE, AUTHTEST_NICK, AUTHTEST_PASSWORD);
    myPlayerLogin(AUTHTEST_LOGIN_PROFILE, AUTHTEST_NICK, AUTHTEST_PASSWORD);

    GSI_UNUSED(argc);
    GSI_UNUSED(argv);
}



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void PrintResults()
{
	LOGI("> --------------------------------\n");
	LOGI("> FINISHED: %d/%d PASSED\n", gNumPassed, gNumTests);
	LOGI("> --------------------------------\n");
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void android_main(struct android_app* state)
{	
    int ident;
    int events;
    struct android_poll_source* source;

	app_dummy();

   	// Set debug levels
	gsSetDebugLevel(GSIDebugCat_All, GSIDebugType_All, GSIDebugLevel_Verbose);


    // Check backend availability
    if (!isBackendAvailable())
        return;

    // Initialize SDK core/common objects for the auth service and 
    gsCoreInitialize();

    // Run the tests
    RunLoginTests(0, 0);

    // Print results of the test
    PrintResults();

    while ((ident=ALooper_pollAll(-1, NULL, &events, (void**)&source)) >= 0)
    {
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
            // shutdown core before quitting
            gsCoreShutdown();

            // Wait for core shutdown
            //   (should be instantaneous unless you have multiple cores)
            while(gsCoreIsShutdown() == GSCore_SHUTDOWN_PENDING)
            {
                gsCoreThink(0);
                msleep(5);
            }
            LOGI("Done.\r\n");
            return;
        }
    }

    return;
}
