///////////////////////////////////////////////////////////////////////////////
// File:	sbctest.c
// SDK:		GameSpy Server Browsing SDK
//
// Copyright (c) 2012 GameSpy Technology & IGN Entertainment, Inc. All rights
// reserved. This software is made available only pursuant to certain license
// terms offered by IGN or its subsidiary GameSpy Industries, Inc. Unlicensed
// use or use in a manner not expressly authorized by IGN or GameSpy Technology
// is prohibited.
// ------------------------------------
// See the ReadMe file for sbctest info, and consult the GameSpy Server 
// Browsing SDK documentation for more information on implementing the Server 
// Browsing SDK.

/********
INCLUDES
********/
#if defined(_WIN32)
	#include <conio.h> // Used for keyboard input.
#endif
#include "../sb_serverbrowsing.h"
#include "../../qr2/qr2.h"
#include "../../natneg/natneg.h"
#include "../../common/gsAvailable.h"

/********
DEFINES
********/
#define GAME_NAME		_T("gmtest")
#define SECRET_KEY		_T("HA6zkS")

// Ensure cross-platform compatibility for printf.
#ifdef UNDER_CE
	void RetailOutputA(CHAR *tszErr, ...);
	#define printf RetailOutputA
#elif defined(_NITRO)
	#include "../../common/nitro/screen.h"
	#define printf Printf
	#define vprintf VPrintf
#endif

/********
GLOBAL VARS
********/
static gsi_bool UpdateFinished = gsi_false; // Used to track status of server browser updates.

/********
DEBUG OUTPUT
********/
#ifdef GSI_COMMON_DEBUG
#if !defined(_MACOSX) && !defined(_IPHONE)
static void DebugCallback(GSIDebugCategory theCat, GSIDebugType theType,
						  GSIDebugLevel theLevel, const char * theTokenStr,
						  va_list theParamList)
{
	GSI_UNUSED(theLevel);
	printf("[%s][%s] ", 
		gGSIDebugCatStrings[theCat], 
		gGSIDebugTypeStrings[theType]);

	vprintf(theTokenStr, theParamList);
}
#endif
#ifdef GSI_UNICODE
static void AppDebug(const unsigned short* format, ...)
{
	// Construct text, then pass in as ASCII.
	unsigned short buf[1024];
    char tmp[2056];
	va_list aList;
	va_start(aList, format);
	_vswprintf(buf, 1024, format, aList);

    UCS2ToAsciiString(buf, tmp);
	gsDebugFormat(GSIDebugCat_App, GSIDebugType_Misc, GSIDebugLevel_Notice,
		"%s", tmp);
}
#else
static void AppDebug(const char* format, ...)
{
	va_list aList;
	va_start(aList, format);
	gsDebugVaList(GSIDebugCat_App, GSIDebugType_Misc, GSIDebugLevel_Notice,
		format, aList);
}
#endif
#else
	#define AppDebug _tprintf
#endif

/********
PROTOTYPES - Avoids strict compiler warnings.
********/
int test_main(int argc, char **argp);

// Callback called as server browser updates process.
static void SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance)
{
	int i; // for-loop ctr
	gsi_char * defaultString = _T("");  // Default string for SBServerGet functions - returns if specified string key is not found.
	int defaultInt = 0;  // Default int value for SBServerGet functions - returns if specified int key is not found.
	gsi_char anAddress[20] = { '\0' };  // Used to store the server IP.

	// Retrieve the server ip.
#ifdef GSI_UNICODE
	if (server)
		AsciiToUCS2String(SBServerGetPublicAddress(server),anAddress);
#else
	if (server)
		strcpy(anAddress, SBServerGetPublicAddress(server));
#endif

	switch (reason)
	{
	case sbc_serveradded:  // New SBServer added to the server browser list.
		// Output the server's IP and port (the rest of the server's basic keys 
		// may not yet be available).
		AppDebug(_T("Server Added: %s:%d\n"), anAddress, SBServerGetPublicQueryPort(server));
		break;
	case sbc_serverchallengereceived: // Received ip verification challenge from server.
		// Informational, no action required.
		break;
	case sbc_serverupdated:  // Either basic or full information is now available for this server.
		// Retrieve and print the basic server fields (specified as a parameter 
		// in ServerBrowserUpdate).
		AppDebug(_T("ServerUpdated: %s:%d\n"), anAddress, SBServerGetPublicQueryPort(server));
		AppDebug(_T("  Host: %s\n"), SBServerGetStringValue(server, _T("hostname"), defaultString));
		AppDebug(_T("  Gametype: %s\n"), SBServerGetStringValue(server, _T("gametype"), defaultString));
		AppDebug(_T("  Map: %s\n"), SBServerGetStringValue(server, _T("mapname"), defaultString));
		AppDebug(_T("  Players/MaxPlayers: %d/%d\n"), SBServerGetIntValue(server, _T("numplayers"), defaultInt), SBServerGetIntValue(server, _T("maxplayers"), defaultInt));
		AppDebug(_T("  Ping: %dms\n"), SBServerGetPing(server));
		
		// If the server has full keys (ServerBrowserAuxUpdate), print them.
		if (SBServerHasFullKeys(server))
		{
			// Print some non-basic server info.
			AppDebug(_T("  Frag limit: %d\n"), SBServerGetIntValue(server, _T("fraglimit"), defaultInt));
			AppDebug(_T("  Time limit: %d minutes\n"), SBServerGetIntValue(server, _T("timelimit"), defaultInt));
			AppDebug(_T("  Gravity: %d\n"), SBServerGetIntValue(server, _T("gravity"), defaultInt));

			// Print player info.
			AppDebug(_T("  Players:\n"));
			for(i = 0; i < SBServerGetIntValue(server, _T("numplayers"), 0); i++) // Loop through all players on the server 
			{
				// Print player key info for the player at index i.
				AppDebug(_T("    %s\n"), SBServerGetPlayerStringValue(server, i, _T("player"), defaultString));
				AppDebug(_T("      Score: %d\n"), SBServerGetPlayerIntValue(server, i, _T("score"), defaultInt));
				AppDebug(_T("      Deaths: %d\n"), SBServerGetPlayerIntValue(server, i, _T("deaths"), defaultInt));
				AppDebug(_T("      Team (0=Red/1=Blue): %d\n"), SBServerGetPlayerIntValue(server, i, _T("team"), defaultInt));
				AppDebug(_T("      Ping: %d\n"), SBServerGetPlayerIntValue(server, i, _T("ping"), defaultInt));

			}
			// Print team info (team name and team score).
			AppDebug(_T("  Teams (Score):\n"));
			for(i = 0; i < SBServerGetIntValue(server, _T("numteams"), 0); i++) 
			{
				AppDebug(_T("    %s (%d)\n"), SBServerGetTeamStringValue(server, i, _T("team"), defaultString),
					     SBServerGetTeamIntValue(server, i, _T("score"), defaultInt));
			}
		}
		break;
	case sbc_serverupdatefailed:
		AppDebug(_T("Update Failed: %s:%d\n"), anAddress, SBServerGetPublicQueryPort(server));
		break;
	case sbc_updatecomplete: // Update is complete; server query engine is now idle (not called upon AuxUpdate completion).
		AppDebug(_T("Server Browser Update Complete\r\n")); 
		UpdateFinished = gsi_true; // This will let us know to stop calling ServerBrowserThink.
		break;
	case sbc_queryerror: // The update returned an error 
		AppDebug(_T("Query Error: %s\n"), ServerBrowserListQueryError(sb));
		UpdateFinished = gsi_true; // Set to true here since we won't get an updatecomplete call.
		break;
	default:
		break;
	}

	GSI_UNUSED(instance);
}

int test_main(int argc, char **argp)
{
	ServerBrowser sb;  // Server browser object initialized with ServerBrowserNew.

	// ServerBrowserNew parameters:
	int version = 0;			// ServerBrowserNew parameter; set to 0 unless otherwise directed by GameSpy.
	int maxConcUpdates = 20;	// Max number of queries the ServerBrowsing SDK will send out at one time.
	SBBool lanBrowse = SBFalse; // Set true for LAN-only browsing.
	void * userData = NULL;     // Optional data that will be passed to the SBCallback function after updates.

	// ServerBrowserUpdate parameters:
	SBBool async = SBTrue;			// We will run the updates asynchronously.
	SBBool discOnComplete = SBTrue; // Disconnect from the master server after completing update (future
									// updates will automatically re-connect).
	// These will be the only keys retrieved on server browser updates:
	unsigned char basicFields[] = {HOSTNAME_KEY, GAMETYPE_KEY,  MAPNAME_KEY, NUMPLAYERS_KEY, MAXPLAYERS_KEY};
	int numFields = sizeof(basicFields) / sizeof(basicFields[0]); 
	gsi_char serverFilter[100] = {'\0'};	// Filter string for server browser updates.

	// ServerBrowserSort parameters:
	SBBool ascending = SBTrue;				// Sort in ascending order.
	gsi_char * sortKey = _T("ping");		// Sort servers based on ping time.
	SBCompareMode compareMode = sbcm_int;	// We are sorting integers (as opposed to floats or strings).

	// ServerBrowserAuxUpdateServer parameter:
	SBBool fullUpdate = SBTrue;

	GSIACResult result;					// Used for the GameSpy Availability Check.
	int i;								// for-loop counter.
	SBServer server;					// Used to hold each server when iterating through the server list
	int totalServers;					// Keep track of the total number of servers in our server list
	gsi_char * defaultString = _T("");	// Default string for SBServerGet functions; returns if specified string key is not found.
    SBError nError = sbe_noerror;

  	// For debug output on these platforms:
#if defined (_PS3) || defined (_PS2) || defined (_PSP) || defined (_NITRO) || defined (_PSP2)
	#ifdef GSI_COMMON_DEBUG
		// Define GSI_COMMON_DEBUG if you want to view the SDK debug output.
		// Set the SDK debug log file, or set your own handler using gsSetDebugCallback.
		//gsSetDebugFile(stdout); // Output to console.
		gsSetDebugCallback(DebugCallback);

		// Set debug levels.
		gsSetDebugLevel(GSIDebugCat_All, GSIDebugType_All, GSIDebugLevel_Verbose);
	#endif
#endif

	// Perform the standard GameSpy Availability Check.
	GSIStartAvailableCheck(GAME_NAME);
	while((result = GSIAvailableCheckThink()) == GSIACWaiting)
		msleep(5);
	if(result != GSIACAvailable)
	{
		AppDebug(_T("The backend is not available\n"));
		return 1;
	}
	
	AppDebug(_T("Creating server browser for %s\n\n"), GAME_NAME);
	// Create a new server browser object.
	sb = ServerBrowserNew (GAME_NAME, GAME_NAME, SECRET_KEY, version, maxConcUpdates, QVERSION_QR2, lanBrowse, SBCallback, userData);

	// Populate the server browser's server list by doing an Update.
	AppDebug(_T("Starting server browser update\n"));
	// Begin the update (async).
	nError = ServerBrowserUpdate(sb, async, discOnComplete, basicFields, numFields, serverFilter);
	if(nError)
	{
		printf("ServerBrowserUpdate Error 0x%x\n", nError);
		return nError;
	}
	
	// Think while the update is in progress.
	while (!UpdateFinished)
	{
		nError = ServerBrowserThink(sb);
		if(nError)
		{
			printf("ServerBrowserThink Error 0x%x\n", nError);
			return nError;
		}
		msleep(10);  // Think should be called every 10-100ms; quicker calls produce more accurate ping measurements.
	}

// End Update.

	// Sort the server list by ping time in ascending order.
	AppDebug(_T("\nSorting server list by ping\n"));
	// Sorting is typically done based on user input, such as clicking on the 
	// column header of the field to sort.
	ServerBrowserSort(sb, ascending, sortKey, compareMode); 

	totalServers = ServerBrowserCount(sb); // Total count of servers in our server list.
	if (totalServers == 0)
		printf("There are no %s servers running currently\n", GAME_NAME);
	else 
	{
		printf("Sorted list:\n");
		// Display the server list in the new sorted order.
		for(i = 0; i < totalServers; i++)
		{
			server = ServerBrowserGetServer(sb, i);  // Get the SBServer object at index 'i' in the server list.
			if(!server)
			{
				printf("ServerBrowserGetServer Error!\n");
				return -1;
			}

			// Print the server host along with its ping.
			AppDebug(_T("  %s  ping: %dms\n"), SBServerGetStringValue(server, _T("hostname"), defaultString), SBServerGetPing(server));
		}
	}
// End server list sorting.

	// Refresh the server list, this time using a server filter.
	AppDebug(_T("\nRefreshing server list and applying a filter: "));
	ServerBrowserClear(sb); // We need to clear first so that we don't end up with duplicates.
	
	AppDebug(_T("US servers with more than 5 players, or servers with a hostname containing 'GameSpy'\n\n"));
    // This is a filter in US servers that have more than 5 players, or any 
	// server containing 'GameSpy' in the hostname.
	_tcscpy(serverFilter,_T("(country = 'US' and numplayers > 5) or hostname like '%GameSpy%'"));
	// Note that filtering by "ping" is not possible, since ping is determined 
	// by the client and not the master server.

	// Begin the update (async).
	nError = ServerBrowserUpdate(sb, async, discOnComplete, basicFields, numFields, serverFilter);
	if(nError)
	{
		printf("ServerBrowserUpdate w/ Filters Error 0x%x\n", nError);
		return nError;
	}

	UpdateFinished = gsi_false; // This was set to true from the last update, so we set it back until the new update completes.

	// Think once again while the update is in progress.
	while (!UpdateFinished)
	{
		nError = ServerBrowserThink(sb);
		if(nError)
		{
			printf("ServerBrowserThink Error 0x%x\n", nError);
			return nError;
		}
		msleep(10);  // Think should be called every 10-100ms; quicker calls produce more accurate ping measurements.
	}

	// End refresh with filter.

	// If the qr2 sample server is running, we will do an AuxUpdate to retrieve its full keys.
	AppDebug(_T("\nLooking for GameSpy QR2 Sample server\n"));
	totalServers = ServerBrowserCount(sb); // The total count of servers in our server list.
	if (totalServers == 0)
		AppDebug(_T("There are no %s servers running currently\n"), GAME_NAME);
	else 
	{
		int serverFound = 0; // Set this to 1 if GameSpy QR2 Sample server is in the list.

		// Iterate through the server list looking for GameSpy QR2 Sample.
		for(i = 0; i < totalServers; i++)
		{
			server = ServerBrowserGetServer(sb, i);  // Get the SBServer object at index 'i' in the server list.
			if(!server)
			{
				printf("ServerBrowserGetServer Error!\n");
				return -1;
			}
			
			// Check if the hostname server key is "GameSpy QR2 Sample".
			if (!(_tcscmp(SBServerGetStringValue(server, _T("hostname"), defaultString), _T("GameSpy QR2 Sample")))) 
			{  

				AppDebug(_T("Found it!\n\nRunning AuxUpdate to get more specific server info:\n\n"));
				// Update the qr2 sample server object to contain its full keys .
				nError = ServerBrowserAuxUpdateServer(sb, server, async, fullUpdate);
				if(nError)
				{
					printf("ServerBrowserUpdate Error 0x%x\n", nError);
					return nError;
				}
				// Note: Only call this on a server currently in the server 
				// list; otherwise call ServerBrowserAuxUpdateIP.

				// Think once again while the update is in progress; done 
				// once the server object has full keys.
				while (!SBServerHasFullKeys(server)) 
				{
					nError = ServerBrowserThink(sb);
					if(nError)
					{
						printf("ServerBrowserThink Error on AuxUpdateServer 0x%x\n", nError);
						return nError;
					}
					msleep(10);  // Think should be called every 10-100ms; 
								 // quicker calls produce more accurate ping 
								 // measurements.
				}

				serverFound = 1;
				break;  // We already found the qr2 sample server, so there's 
						// no need to loop through the rest.
			}
		}
		if (!serverFound)
		{
			AppDebug(_T("Gamespy QR2 Sample server is not running\n"));
			return -1;
		}
	}
	// End AuxUpdate.

	ServerBrowserFree(sb); // Clean up.

	GSI_UNUSED(argc);
	GSI_UNUSED(argp);

	// Finished.
	return 0;
}
