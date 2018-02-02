///////////////////////////////////////////////////////////////////////////////
// File:	qr2main.c
// SDK:		GameSpy Query and Reporting SDK
//
// Copyright (c) IGN Entertainment, Inc.  All rights reserved.
// This software is made available only pursuant to certain license terms offered
// by IGN or its subsidiary GameSpy Industries, Inc.  Unlicensed use or use in a
// manner not expressly authorized by IGN or GameSpy is prohibited.
// ------------------------------------
// See the ReadMe file for QR2NativeActivity info, and consult the GameSpy QR2
// SDK documentation for more information on implementing the QR2 SDK.

//BEGIN_INCLUDE(all)
#include <jni.h>
#include <errno.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include "../../../qr2.h"
#include "../../../../natneg/natneg.h"
#include "../../../../common/gsCommon.h"
#include "../../../../common/gsAvailable.h"
#include "../../../../common/gsCore.h"
#include "../../../../common/gsSoap.h"
#include "../../../../ghttp/ghttp.h"
#include "../../../../webservices/AuthService.h"

/********
DEFINES
********/

// Android Log defines
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "YOUR_TAG", __VA_ARGS__))

// Here we set some of the fixed server keys.
#define GAME_VERSION		_T("2.00")
#define GAMEID				0
#define GAME_NAME			_T("gmtest")
#define SECRET_KEY			_T("HA6zkS")
#define SECRET_KEY_ASCII	"HA6zkS"
#define MAX_PLAYERS			32
#define MAX_TEAMS			2
#define BASE_PORT			11111

#define ACCESS_KEY     "39cf9714e24f421c8ca07b9bcb36c0f5"
#define PRODUCTID	0
#define NAMESPACEID 1
#define NICKNAME    _T("saketest")
#define EMAIL       _T("saketest@saketest.com")
#define PASSWORD    _T("saketest")

// define our additional keys, making sure not to overwrite the reserved standard key ids
// standard keys use 0-NUM_RESERVED_KEYS (defined in qr2regkeys.h)
#define GRAVITY_KEY 100
#define RANKINGON_KEY 101
#define TIME__KEY 102
#define AVGPING_T_KEY 103

// define for callback checks
#define KEYLIST_OK	0x01
#define SERVER_OK	0x02
#define COUNT_OK	0x04
#define PLAYER_OK	0x08
#define TEAM_OK		0x10
#define BACKEND_OK	0x20
#define CALLBACK_OK	0x3F

/********
TYPEDEFS
********/
//representative of a game player structure
typedef struct
{
	gsi_char pname[80];
	int pfrags;
	int pdeaths;
	int ptime;
	int pping;
	int pteam;
} player_t;

//representative of a team structure
typedef struct
{
	gsi_char tname[80];
	int tscore;
	int avgping;

} team_t;

//representative of a game data structure
typedef struct
{
	player_t players[MAX_PLAYERS];
	team_t teams[MAX_TEAMS];
	gsi_char mapname[20];
	gsi_char hostname[120];
	gsi_char gamemode[200];
	gsi_char gametype[30];
	int numteams;
	int numplayers;
	int maxplayers;
	int fraglimit;
	int timelimit;
	int teamplay;
	int rankingson;
	int gravity;
	int hostport;
} gamedata_t;

/********
GLOBAL VARS
********/
// just to give us bogus data
gsi_char *constnames[MAX_PLAYERS]=
{
	_T("Joe Player"), _T("L33t 0n3"), _T("Raptor"), _T("Gr81"),
	_T("Flubber"),    _T("Sarge"),    _T("Void"),   _T("runaway"),
	_T("Ph3ar"),      _T("wh00t"),    _T("gr1nder"),_T("Mace"),
	_T("stacy"),      _T("lamby"),    _T("Thrush"), _T("Leeroy")
};
gamedata_t gamedata;  // to store all the server/player/teamkeys

// for sample
gsi_time  aStartTime = 0;

// check to confirm all callbacks
unsigned char callback_check = 0;

// Place for AuthService Data
int authenticated = 0;
GSLoginCertificate certificate;
GSLoginPrivateData privateData;

/********
PROTOTYPES
********/
void serverkey_callback(int keyid, qr2_buffer_t outbuf, void *userdata);
void playerkey_callback(int keyid, int index, qr2_buffer_t outbuf, void *userdata);
void teamkey_callback(int keyid, int index, qr2_buffer_t outbuf, void *userdata);
void keylist_callback(qr2_key_type keytype, qr2_keybuffer_t keybuffer, void *userdata);
int  count_callback(qr2_key_type keytype, void *userdata);
void adderror_callback(qr2_error_t error, gsi_char *errmsg, void *userdata);
void nn_callback(int cookie, void *userdata);
void cm_callback(gsi_char *data, int len, void *userdata);
void cc_callback(SOCKET gamesocket, struct sockaddr_in *remoteaddr, void *userdata);
void hr_callback(void *userdata);
void DoGameStuff(gsi_time totalTime);
int  test_main(int argc, char **argp);

// called when a server key needs to be reported
void serverkey_callback(int keyid, qr2_buffer_t outbuf, void *userdata)
{
	callback_check |= SERVER_OK;

	LOGI("Reporting server keys");

	switch (keyid)
	{
	case HOSTNAME_KEY:
		qr2_buffer_add(outbuf, gamedata.hostname);
		break;
	case GAMEVER_KEY:
		qr2_buffer_add(outbuf, GAME_VERSION);
		break;
	case HOSTPORT_KEY:
		qr2_buffer_add_int(outbuf, gamedata.hostport);
		break;
	case MAPNAME_KEY:
		qr2_buffer_add(outbuf, gamedata.mapname);
		break;
	case GAMETYPE_KEY:
		qr2_buffer_add(outbuf, gamedata.gametype);
		break;
	case NUMPLAYERS_KEY:
		qr2_buffer_add_int(outbuf, gamedata.numplayers);
		break;
	case NUMTEAMS_KEY:
		qr2_buffer_add_int(outbuf, gamedata.numteams);
		break;
	case MAXPLAYERS_KEY:
		qr2_buffer_add_int(outbuf, gamedata.maxplayers);
		break;
	case GAMEMODE_KEY:
		qr2_buffer_add(outbuf, gamedata.gamemode);
		break;
	case TEAMPLAY_KEY:
		qr2_buffer_add_int(outbuf, gamedata.teamplay);
		break;
	case FRAGLIMIT_KEY:
		qr2_buffer_add_int(outbuf, gamedata.fraglimit);
		break;
	case TIMELIMIT_KEY:
		qr2_buffer_add_int(outbuf, gamedata.timelimit);
		break;
	case GRAVITY_KEY:
		qr2_buffer_add_int(outbuf, gamedata.gravity);
		break;
	case RANKINGON_KEY:
		qr2_buffer_add_int(outbuf, gamedata.rankingson);
		break;
	default:
		qr2_buffer_add(outbuf, _T(""));
	}

	GSI_UNUSED(userdata);
}

// called when a player key needs to be reported
void playerkey_callback(int keyid, int index, qr2_buffer_t outbuf, void *userdata)
{
	callback_check |= PLAYER_OK;

	LOGI("Reporting player keys");

	//check for valid index
	if (index >= gamedata.numplayers)
	{
		qr2_buffer_add(outbuf, _T(""));
		return;
	}
	switch (keyid)
	{
	case PLAYER__KEY:
		qr2_buffer_add(outbuf, gamedata.players[index].pname);
		break;
	case SCORE__KEY:
		qr2_buffer_add_int(outbuf, gamedata.players[index].pfrags);
		break;
	case DEATHS__KEY:
		qr2_buffer_add_int(outbuf, gamedata.players[index].pdeaths);
		break;
	case PING__KEY:
		qr2_buffer_add_int(outbuf, gamedata.players[index].pping);
		break;
	case TEAM__KEY:
		qr2_buffer_add_int(outbuf, gamedata.players[index].pteam);
		break;
	case TIME__KEY:
		qr2_buffer_add_int(outbuf, gamedata.players[index].ptime);
		break;
	default:
		qr2_buffer_add(outbuf, _T(""));
		break;
	}

	GSI_UNUSED(userdata);
}

// called when a team key needs to be reported
void teamkey_callback(int keyid, int index, qr2_buffer_t outbuf, void *userdata)
{
	callback_check |= TEAM_OK;

	LOGI("Reporting team keys");

	//check for valid index
	if (index >= gamedata.numteams)
	{
		qr2_buffer_add(outbuf, _T(""));
		return;
	}
	switch (keyid)
	{
	case TEAM_T_KEY:
		qr2_buffer_add(outbuf, gamedata.teams[index].tname);
		break;
	case SCORE_T_KEY:
		qr2_buffer_add_int(outbuf, gamedata.teams[index].tscore);
		break;
	case AVGPING_T_KEY:
		qr2_buffer_add_int(outbuf, gamedata.teams[index].avgping);
		break;
	default:
		qr2_buffer_add(outbuf, _T(""));
		break;
	}

	GSI_UNUSED(userdata);
}

// called when we need to report the list of keys we report values for
void keylist_callback(qr2_key_type keytype, qr2_keybuffer_t keybuffer, void *userdata)
{
	callback_check |= KEYLIST_OK;

	LOGI("Reporting keylist");

	//need to add all the keys we support
	switch (keytype)
	{
	case key_server:
		qr2_keybuffer_add(keybuffer, HOSTNAME_KEY);
		qr2_keybuffer_add(keybuffer, GAMEVER_KEY);
		qr2_keybuffer_add(keybuffer, HOSTPORT_KEY);
		qr2_keybuffer_add(keybuffer, MAPNAME_KEY);
		qr2_keybuffer_add(keybuffer, GAMETYPE_KEY);
		qr2_keybuffer_add(keybuffer, NUMPLAYERS_KEY);
		qr2_keybuffer_add(keybuffer, NUMTEAMS_KEY);
		qr2_keybuffer_add(keybuffer, MAXPLAYERS_KEY);
		qr2_keybuffer_add(keybuffer, GAMEMODE_KEY);
		qr2_keybuffer_add(keybuffer, TEAMPLAY_KEY);
		qr2_keybuffer_add(keybuffer, FRAGLIMIT_KEY);
		qr2_keybuffer_add(keybuffer, TIMELIMIT_KEY);
		qr2_keybuffer_add(keybuffer, GRAVITY_KEY); //a custom key
		qr2_keybuffer_add(keybuffer, RANKINGON_KEY); //a custom key
		break;
	case key_player:
		qr2_keybuffer_add(keybuffer, PLAYER__KEY);
		qr2_keybuffer_add(keybuffer, SCORE__KEY);
		qr2_keybuffer_add(keybuffer, DEATHS__KEY);
		qr2_keybuffer_add(keybuffer, PING__KEY);
		qr2_keybuffer_add(keybuffer, TEAM__KEY);
		qr2_keybuffer_add(keybuffer, TIME__KEY); //a custom key
		break;
	case key_team:
		qr2_keybuffer_add(keybuffer, TEAM_T_KEY);
		qr2_keybuffer_add(keybuffer, SCORE_T_KEY);
		qr2_keybuffer_add(keybuffer, AVGPING_T_KEY); //a custom key
		break;
	default: break;
	}

	GSI_UNUSED(userdata);
}

// called when we need to report the number of players and teams
int count_callback(qr2_key_type keytype, void *userdata)
{
	callback_check |= COUNT_OK;

	GSI_UNUSED(userdata);

	LOGI("Reporting number of players/teams");

	if (keytype == key_player)
		return gamedata.numplayers;
	else if (keytype == key_team)
		return gamedata.numteams;
	else
		return 0;

}

// called if our registration with the GameSpy master server failed
void adderror_callback(qr2_error_t error, gsi_char *errmsg, void *userdata)
{
	GS_ASSERT(errmsg);
	LOGI("Error adding server: %d, %s", error, errmsg);
	GSI_UNUSED(userdata);
}

// called once the server is successfully listed on the backend
void hr_callback(void *userdata)
{
	callback_check |= BACKEND_OK;

	LOGI("Server successfully listed on backend...");
	GSI_UNUSED(userdata);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void myLoginCallback(GHTTPResult httpResult, WSLoginResponse * theResponse, void * userData)
{
	if (httpResult != GHTTPSuccess)
	{
		printf("Failed on player login, HTTP error: %d", httpResult);
		exit(0);
	}
	else if (theResponse->mLoginResult != WSLogin_Success)
	{
		printf("Failed on player login, Login result: %d", theResponse->mLoginResult);
		exit(0);
	}
	else
	{
		// Copy certificate and private key.
		memcpy(&certificate, &theResponse->mCertificate, sizeof(GSLoginCertificate));
		memcpy(&privateData, &theResponse->mPrivateData, sizeof(GSLoginPrivateData));

		authenticated = 1; // This is so we know that we can stop gsCore thinking.
		_tprintf(_T("Player '") GS_USTR _T("' logged in.\n"), theResponse->mCertificate.mUniqueNick);
	}
	GSI_UNUSED(userData);
}

// Use AuthService, authentication required to use QR2 service
static void LoginAndAuthenticate()
{
	wsSetGameCredentials(ACCESS_KEY, GAMEID, SECRET_KEY_ASCII);

	if (0 != wsLoginProfile(GAMEID, GP_PARTNERID_GAMESPY, NAMESPACEID, NICKNAME, EMAIL, PASSWORD, _T(""), myLoginCallback, NULL))
	{
		printf("Failed on wsLoginProfile");
		exit(0);
	}

	while(authenticated == 0)
	{
		msleep(100);
		gsCoreThink(0);
	}

}

// initialize the gamedata structure with bogus data
static void init_game(void)
{
	int i;
	LOGI("Generating game data");
	srand((unsigned int) current_time() );
	gamedata.numplayers = rand() % 15 + 1;
	gamedata.maxplayers = MAX_PLAYERS;
	for (i = 0 ; i < gamedata.numplayers ; i++)
	{
		_tcscpy(gamedata.players[i].pname, constnames[i]);
		gamedata.players[i].pfrags = rand() % 32;
		gamedata.players[i].pdeaths = rand() % 32;
		gamedata.players[i].ptime = rand() % 1000;
		gamedata.players[i].pping = rand() % 500;
		gamedata.players[i].pteam = rand() % 2;
	}
	gamedata.numteams = 2;
	for (i = 0 ; i < gamedata.numteams ; i++)
	{
		gamedata.teams[i].tscore = rand() % 500;
		gamedata.teams[i].avgping = rand() % 500;
	}

	_tcscpy(gamedata.teams[0].tname,_T("Red"));
	_tcscpy(gamedata.teams[1].tname,_T("Blue"));
	_tcscpy(gamedata.mapname,_T("gmtmap1"));
	_tcscpy(gamedata.gametype,_T("arena"));
	_tcscpy(gamedata.hostname,_T("GameSpy QR2 Sample"));
	_tcscpy(gamedata.gamemode,_T("openplaying"));

	gamedata.fraglimit = 0;
	gamedata.timelimit = 40;
	gamedata.teamplay = 1;
	gamedata.rankingson = 1;
	gamedata.gravity = 800;
	gamedata.hostport = 25000;
}

// simulate whatever else a game server does
void DoGameStuff(gsi_time totalTime)
{
	// After 30 seconds, we will change the game map and call qr2_send_statechanged
	// This should only be called after major changes such as mapname or gametype, and
	// cannot be applied more than once every 10 seconds (subsequent calls will be delayed
	// when necessary)

	if (totalTime > 30000)
	{
		LOGI("Mapname changed, calling qr2_send_statechanged");
		if(0 == _tcscmp(gamedata.mapname,_T("gmtmap2")))
		{
			_tcscpy(gamedata.mapname,_T("gmtmap1"));
		}
		else
		{
			_tcscpy(gamedata.mapname,_T("gmtmap2"));
		}
		qr2_send_statechanged(NULL);
		aStartTime += totalTime;
	}
}

static void qr2_handle_cmd(struct android_app* app, int32_t cmd)
{
	LOGI("qr2_handle_cmd hit");
    switch (cmd)
    {
    case APP_CMD_TERM_WINDOW:
    	LOGI("TERM COMMAND ISSUED");
    	break;
    default:
    	LOGI("UNHANDLED COMMAND");
    	break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state)
{
	// Make sure glue isn't stripped.
    app_dummy();

	/* qr2_init parameters */
	gsi_char  ip[255];               // to manually set local IP
	const int isPublic = 1;          // set to '0' for a LAN game
	const int isNatNegSupported = 1; // set to '0' if you don't support Nat Negotiation

	void * userData = NULL;          // optional data that will be passed to the callback functions

	GSIACResult result;				 // result for Availability Check

    gsSetDebugLevel(GSIDebugCat_All, GSIDebugType_All, GSIDebugLevel_Hardcore);

	// Let's do availability check
	GSIStartAvailableCheck(GAME_NAME);
	while((result = GSIAvailableCheckThink()) == GSIACWaiting)
		msleep(5);
	if(result != GSIACAvailable)
	{
		LOGI(_T("The backend is not available\n"));
		return;
	}

    // Initialize the GameSpy SDK Core (required by SOAP SDKs).
	printf("Initializing the GameSpy Core\n");
	gsCoreInitialize();

	// Authenticate
	LoginAndAuthenticate();

	// register our custom keys (you do not have to register the reserved standard keys)
	LOGI("Registering custom keys");
	qr2_register_key(GRAVITY_KEY, _T("gravity")    );
	qr2_register_key(RANKINGON_KEY, _T("rankingon"));
	qr2_register_key(TIME__KEY,     _T("time_")    ); // player keys always end with '_'
	qr2_register_key(AVGPING_T_KEY, _T("avgping_t")); // team keys always end with '_t'

	// create some random game data
	init_game();

	LOGI("Initializing SDK; server should show up on the master list within 6-10 sec.");
	//Call qr2_init with the query port number and gamename, default IP address, and no user data
	//Pass NULL for the qrec parameter (first parameter) as long as you're running a single game
	//server instance per process
	//Reference gt2nat sample for qr2_init_socket implementation
	if (qr2_init(NULL,NULL,BASE_PORT,GAME_NAME, SECRET_KEY, isPublic, isNatNegSupported,
		serverkey_callback, playerkey_callback, teamkey_callback,
		keylist_callback, count_callback, adderror_callback, userData) != e_qrnoerror)
	{
		LOGI("Error starting query sockets");
		return;
	}

	// callback informs us when we've successfully advertised to the backend, (e.g. tells us when a server is listed)
	qr2_register_hostregistered_callback(NULL, hr_callback);

	// Enter the main loop
	LOGI("Sample will change mapname every 30 secs");
	aStartTime = current_time();
	while (1)
	{
	    // Read all pending events.
	    int ident;
	    int events;
	    struct android_poll_source* source;

	    while ((ident=ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0)
	    {
	    	// Process this event.
	        if (source != NULL)
	        {
	        	source->process(state, source);
	        }
	        // If a sensor has data, process it now.
	        if (ident == LOOPER_ID_USER)
	        {
	        	LOGI("Sensor has some data to be processed");
	        }
	        if (state->destroyRequested != 0)
	        {
	        	LOGI("Shutting down - server will be removed from the master server list");

	        	//let gamemaster know we are shutting down (removes dead server entry from the list)
	            qr2_shutdown(NULL);

	            #ifdef GSI_UNICODE
	            // In Unicode mode we must perform additional cleanup
	            qr2_internal_key_list_free();
	            #endif

	            // Callback all suceeded?
	            if(callback_check != CALLBACK_OK)
	            {
	            	LOGI("Error with Callback Functions (callback_check = 0x%x)", callback_check);
	            }
                return;
			}
	    }
		gsi_time totalTime = current_time() - aStartTime; // used to change the game state after 30 seconds

		// An actual game would do something between "thinks"
		DoGameStuff(totalTime);

		//check for / process incoming queries
		//should be called every 10-100 ms; quicker calls produce more accurate ping measurements
		qr2_think(NULL);

	    msleep(10);
	}
	return;
}

