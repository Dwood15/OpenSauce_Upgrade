///////////////////////////////////////////////////////////////////////////////
// File:	sbctest.c
// SDK:		GameSpy Server Browsing SDK
//
// Copyright (c) IGN Entertainment, Inc.  All rights reserved.  
// This software is made available only pursuant to certain license terms offered
// by IGN or its subsidiary GameSpy Industries, Inc.  Unlicensed use or use in a 
// manner not expressly authorized by IGN or GameSpy is prohibited.
// ------------------------------------
// See the ReadMe file for sbctest info, and consult the GameSpy Server Browsing
// SDK documentation for more information on implementing the Server Browsing SDK.

/********
INCLUDES
********/

#include <jni.h>
#include <errno.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include "../../../sb_serverbrowsing.h"
#include "../../../../qr2/qr2.h"
#include "../../../../natneg/natneg.h"
#include "../../../../common/gsAvailable.h"

/********
DEFINES
********/
#define GAME_NAME		_T("gmtest")
#define SECRET_KEY		_T("HA6zkS")

// Android logs
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "YOUR_TAG", __VA_ARGS__))
#define GAME_NAME_ASCII	 "gmtest"

/********
GLOBAL VARS
********/
static gsi_bool UpdateFinished = gsi_false; // used to track status of server browser updates

// callback called as server browser updates process
static void SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance)
{
	int i; // for-loop counter
	gsi_char *defaultString = _T("");  // default string for SBServerGet functions - returns if specified string key is not found
	int defaultInt = 0;  // default int value for SBServerGet functions - returns if specified int key is not found
#ifdef GSI_UNICODE
	char aHostname[20] = { '\0' };
	char aGametype[20] = { '\0' };
	char aMapname[20] = { '\0' };

	UCSToAsciiString(SBServerGetStringValue(server, _T("hostname"), defaultString), aHostname);
	UCSToAsciiString(SBServerGetStringValue(server, _T("gametype"), defaultString), aGametype);
	UCSToAsciiString(SBServerGetStringValue(server, _T("mapname"), defaultString), aMapname);
#else
	const gsi_char *aHostname = SBServerGetStringValue(server, _T("hostname"), defaultString);
	const gsi_char *aGametype = SBServerGetStringValue(server, _T("gametype"), defaultString);
	const gsi_char *aMapname = SBServerGetStringValue(server, _T("mapname"), defaultString);
#endif

	switch (reason)
	{
	case sbc_serveradded:  // new SBServer added to the server browser list
		// output the server's IP and port (the rest of the server's basic keys may not yet be available)
		LOGI("Server Added: %s:%d\n", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server));
		break;
	case sbc_serverchallengereceived: // received ip verification challenge from server
		// informational, no action required
		break;
	case sbc_serverupdated:  // either basic or full information is now available for this server
		// retrieve and print the basic server fields (specified as a parameter in ServerBrowserUpdate)
		LOGI("ServerUpdated: %s:%d\n", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server));
		LOGI("  Host: %s\n", aHostname);
		LOGI("  Gametype: %s\n", aGametype);
		LOGI("  Map: %s\n", aMapname);
		LOGI("  Players/MaxPlayers: %d/%d\n", SBServerGetIntValue(server, _T("numplayers"), defaultInt), SBServerGetIntValue(server, _T("maxplayers"), defaultInt));
		LOGI("  Ping: %dms\n", SBServerGetPing(server));
		
		// if the server has full keys (ServerBrowserAuxUpdate), print them
		if (SBServerHasFullKeys(server))
		{
			// print some non-basic server info
			LOGI("  Frag limit: %d\n", SBServerGetIntValue(server, _T("fraglimit"), defaultInt));
			LOGI("  Time limit: %d minutes\n", SBServerGetIntValue(server, _T("timelimit"), defaultInt));
			LOGI("  Gravity: %d\n", SBServerGetIntValue(server, _T("gravity"), defaultInt));

			// print player info
			LOGI("  Players:\n");
			for(i = 0; i < SBServerGetIntValue(server, _T("numplayers"), 0); i++) // loop through all players on the server 
			{
#ifdef GSI_UNICODE
				char aPlayer[20] = { '\0' };
				UCSToAsciiString(SBServerGetPlayerStringValue(server, i, _T("player"), defaultString), aPlayer);
#else
				const gsi_char *aPlayer = SBServerGetPlayerStringValue(server, i, _T("player"), defaultString);
#endif
				// print player key info for the player at index i
				LOGI("    %s\n", aPlayer);
				LOGI("      Score: %d\n", SBServerGetPlayerIntValue(server, i, _T("score"), defaultInt));
				LOGI("      Deaths: %d\n", SBServerGetPlayerIntValue(server, i, _T("deaths"), defaultInt));
				LOGI("      Team (0=Red/1=Blue): %d\n", SBServerGetPlayerIntValue(server, i, _T("team"), defaultInt));
				LOGI("      Ping: %d\n", SBServerGetPlayerIntValue(server, i, _T("ping"), defaultInt));

			}
			// print team info (team name and team score)
			LOGI("  Teams (Score):\n");
			for(i = 0; i < SBServerGetIntValue(server, _T("numteams"), 0); i++) 
			{
#ifdef GSI_UNICODE
				char aTeam[20] = { '\0' };

				UCSToAsciiString(SBServerGetTeamStringValue(server, i, _T("team"), defaultString), aTeam);
#else
				const gsi_char *aTeam = SBServerGetTeamStringValue(server, i, _T("team"), defaultString);
#endif
				LOGI("    %s (%d)\n", aTeam, SBServerGetTeamIntValue(server, i, _T("score"), defaultInt));
			}
		}
		break;
	case sbc_serverupdatefailed:
		LOGI("Update Failed: %s:%d\n", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server));
		break;
	case sbc_updatecomplete: // update is complete; server query engine is now idle (not called upon AuxUpdate completion)
		LOGI("Server Browser Update Complete\r\n");
		UpdateFinished = gsi_true; // this will let us know to stop calling ServerBrowserThink
		break;
	case sbc_queryerror: // the update returned an error 
		LOGI("Query Error: %s\n", ServerBrowserListQueryError(sb));
		UpdateFinished = gsi_true; // set to true here since we won't get an updatecomplete call
		break;
	default:
		break;
	}

	GSI_UNUSED(instance);
}

void android_main(struct android_app* state)
{
	ServerBrowser sb;  // server browser object initialized with ServerBrowserNew

	/* ServerBrowserNew parameters */
	int version = 0;           // ServerBrowserNew parameter; set to 0 unless otherwise directed by GameSpy
	int maxConcUpdates = 20;	// max number of queries the ServerBrowsing SDK will send out at one time
	SBBool lanBrowse = SBFalse;   // set true for LAN only browsing
	void * userData = NULL;       // optional data that will be passed to the SBCallback function after updates
	LOGI("Entered the android_main !!!!!!!");

	/* ServerBrowserUpdate parameters */
	SBBool async = SBTrue;     // we will run the updates asynchronously
	SBBool discOnComplete = SBTrue; // disconnect from the master server after completing update 
	                                // (future updates will automatically re-connect)

	// these will be the only keys retrieved on server browser updates
	unsigned char basicFields[] = {HOSTNAME_KEY, GAMETYPE_KEY,  MAPNAME_KEY, NUMPLAYERS_KEY, MAXPLAYERS_KEY};
	int numFields = sizeof(basicFields) / sizeof(basicFields[0]); 
	gsi_char serverFilter[100] = {'\0'};  // filter string for server browser updates

	/* ServerBrowserSort parameters */
	SBBool ascending = SBTrue;        // sort in ascending order
	gsi_char * sortKey = _T("ping"); // sort servers based on ping time
	SBCompareMode compareMode = sbcm_int;  // we are sorting integers (as opposed to floats or strings)

	/* ServerBrowserAuxUpdateServer parameter */
	SBBool fullUpdate = SBTrue;

	GSIACResult result;	// used for backend availability check
	int i; // for-loop counter
	SBServer server = NULL; // used to hold each server when iterating through the server list
	int totalServers; // keep track of the total number of servers in our server list
	gsi_char * defaultString = _T(""); // default string for SBServerGet functions - returns if specified string key is not found
    gsi_time startTime = 0; 
    SBError nError = sbe_noerror;
    int ident;
    int events;
    struct android_poll_source* source;
    int serverFound = 0; // set to 1 if GameSpy QR2 Sample server is in the list

	// Make sure glue isn't stripped.
   	app_dummy();

   	// Set debug levels
	gsSetDebugLevel(GSIDebugCat_All, GSIDebugType_All, GSIDebugLevel_Verbose);

	// check that the game's backend is available
	GSIStartAvailableCheck(GAME_NAME);
	while((result = GSIAvailableCheckThink()) == GSIACWaiting)
	{
		msleep(5);
	}
	if(result != GSIACAvailable)
	{
		LOGI("The backend is not available\n");
		return ;
	}
	
	LOGI("Creating server browser for %s\n\n", GAME_NAME_ASCII);
	// create a new server browser object
	sb = ServerBrowserNew(GAME_NAME, GAME_NAME, SECRET_KEY, version, maxConcUpdates, QVERSION_QR2, lanBrowse, SBCallback, userData);

/** Populate the server browser's server list by doing an Update **/
	LOGI("Starting server browser update\n");
	// begin the update (async)
	nError = ServerBrowserUpdate(sb, async, discOnComplete, basicFields, numFields, serverFilter);
	if(nError)
	{
		LOGI("ServerBrowserUpdate Error 0x%x\n", nError);
		return ;
	}
	
	// think while the update is in progress
	while (!UpdateFinished)
	{
		nError = ServerBrowserThink(sb);
		if(nError)
		{
			LOGI("ServerBrowserThink Error 0x%x\n", nError);
			return ;
		}
		msleep(10);  // think should be called every 10-100ms; quicker calls produce more accurate ping measurements
	}

/** End Update **/

/** Sort the server list by ping time in ascending order **/
	LOGI("\nSorting server list by ping\n");
	// sorting is typically done based on user input, such as clicking on the column header of the field to sort
	ServerBrowserSort(sb, ascending, sortKey, compareMode); 

	totalServers = ServerBrowserCount(sb); // total servers in our server list
	if (totalServers == 0)
	{
		LOGI("There are no %s servers running currently\n", GAME_NAME_ASCII);
	}
	else 
	{
		LOGI("Sorted list:\n");
		// display the server list in the new sorted order
		for(i = 0; i < totalServers; i++)
		{
#ifdef GSI_UNICODE
			char aHostname[20] = { '\0' };

			UCSToAsciiString(SBServerGetStringValue(server, _T("hostname"), defaultString), aHostname);
#else
			const gsi_char *aHostname = SBServerGetStringValue(server, _T("hostname"), defaultString);
#endif
			server = ServerBrowserGetServer(sb, i);  // get the SBServer object at index 'i' in the server list
			if(!server)
			{
				LOGI("ServerBrowserGetServer Error!\n");
				return ;
			}
			// print the server host along with its ping
			LOGI("  %s  ping: %dms\n", aHostname, SBServerGetPing(server));
		}
	}
/** End server list sorting **/

/** Refresh the server list, this time using a server filter **/
	LOGI("\nRefreshing server list and applying a filter: ");
	ServerBrowserClear(sb); // need to clear first so we don't end up with duplicates
	
	LOGI("US servers with more than 5 players, or servers with a hostname containing 'GameSpy'\n\n");
    // filter in US servers that have more than 5 players, or any server containing 'GameSpy' in the hostname
	_tcscpy(serverFilter,_T("(country = 'US' and numplayers > 5) or hostname like '%GameSpy%'"));
	// note that filtering by "ping" is not possible, since ping is determined by the client - not the master server

	// begin the update (async)
	nError = ServerBrowserUpdate(sb, async, discOnComplete, basicFields, numFields, serverFilter);
	if(nError)
	{
		LOGI("ServerBrowserUpdate w/ Filters Error 0x%x\n", nError);
		return ;
	}
	UpdateFinished = gsi_false; // this was set to true from the last update, so we set it back until the new update completes

	// think once again while the update is in progress
	while (!UpdateFinished)
	{
		nError = ServerBrowserThink(sb);
		if(nError)
		{
			LOGI("ServerBrowserThink Error 0x%x\n", nError);
			return ;
		}
		msleep(10);  // think should be called every 10-100ms; quicker calls produce more accurate ping measurements
	}
	/** End refresh with filter **/

/** If the qr2 sample server is running, we will do an AuxUpdate to retrieve its full keys **/
	LOGI("\nLooking for GameSpy QR2 Sample server\n");
	totalServers = ServerBrowserCount(sb); // total servers in our server list
	if (totalServers == 0)
	{
		LOGI("There are no %s servers running currently\n", GAME_NAME_ASCII);
	}
	else 
	{
		// iterate through the server list looking for GameSpy QR2 Sample
		for(i = 0; i < totalServers; i++)
		{
			server = ServerBrowserGetServer(sb, i);  // get the SBServer object at index 'i' in the server list
			if(!server)
			{
				LOGI("ServerBrowserGetServer Error!\n");
				return ;
			}
			// check if the hostname server key is "GameSpy QR2 Sample"
			if (!(_tcscmp(SBServerGetStringValue(server, _T("hostname"), defaultString), _T("GameSpy QR2 Sample")))) 
			{  
				LOGI("Found it!\n\nRunning AuxUpdate to get more specific server info:\n\n");
				// update the qr2 sample server object to contain its full keys 
				nError = ServerBrowserAuxUpdateServer(sb, server, async, fullUpdate);
				if(nError)
				{
					LOGI("ServerBrowserUpdate Error 0x%x\n", nError);
					return ;
				}
				// Note: Only call this on a server currently in the server list; otherwise call ServerBrowserAuxUpdateIP

				// think once again while the update is in progress; done once the server object has full keys
				while (!SBServerHasFullKeys(server)) 
				{
					nError = ServerBrowserThink(sb);
					if(nError)
					{
						LOGI("ServerBrowserThink Error on AuxUpdateServer 0x%x\n", nError);
						return ;
					}
					msleep(10);  // think should be called every 10-100ms; quicker calls produce more accurate ping measurements
				}
				serverFound = 1;
				break;  // already found the qr2 sample server, no need to loop through the rest 
			}
		}
	}
/** End AuxUpdate **/
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
        	if(totalServers != 0)
        	{
        		if (!serverFound)
        		{
        			LOGI("Gamespy QR2 Sample server is not running\n");
        		}
        	}
        	ServerBrowserFree(sb); // clean up
			LOGI("ServerBrowser Freed");
        	return;
        }
    }
	// Finished
	return;
}
