///////////////////////////////////////////////////////////////////////////////
// File:	sb_serverbrowsing.h
// SDK:		GameSpy Server Browsing SDK
//
// Copyright (c) 2012 GameSpy Technology & IGN Entertainment, Inc. All rights
// reserved. This software is made available only pursuant to certain license
// terms offered by IGN or its subsidiary GameSpy Industries, Inc. Unlicensed
// use or use in a manner not expressly authorized by IGN or GameSpy Technology
// is prohibited.
// ------------------------------------
// Please see the GameSpy Server Browsing SDK documentation for more 
// information.

#ifndef _SB_SERVERBROWSING_H
#define _SB_SERVERBROWSING_H

#include "../common/gsCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Type Definitions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// ServerBrowser is an abstract data type used to represent the game host 
// (server) list and query engine objects.
typedef struct _ServerBrowser *ServerBrowser;

// SBServer is an abstract data type representing a single game host.
#ifndef SBServer 
typedef struct _SBServer *SBServer;
#endif	

///////////////////////////////////////////////////////////////////////////////
//  SBBool
//	Brief
//      Standard Boolean.
//
typedef enum{
	SBFalse,	// False.
	SBTrue		// True.
} SBBool;

///////////////////////////////////////////////////////////////////////////////
//  SBError
//	Brief
//      Error codes that can be returned from Server Browsing functions.
//
typedef enum{
	sbe_noerror,				// No error has occurred.
	sbe_socketerror,			// A socket function has returned an 
								// unexpected error.
	sbe_dnserror,				// DNS lookup of the master address failed.
	sbe_connecterror,			// Connection to the matchmaking service 
								// failed.
	sbe_dataerror,				// Invalid data was returned from the 
								// matchmaking service.
	sbe_allocerror,				// Memory allocation failed.
	sbe_paramerror,				// An invalid parameter was passed to a 
								// function.
	sbe_duplicateupdateerror	// Matchmaking service update was requested on 
								// a game host that was already being updated.
} SBError;	

///////////////////////////////////////////////////////////////////////////////
//  SBError
//	Brief
//      States the ServerBrowser object can be in.
//
typedef enum{
	sb_disconnected,	// Idle and not connected to the matchmaking service.
	sb_listxfer,		// Downloading list of game hosts from the matchmaking 
						// service.
	sb_querying,		// Querying game hosts.
	sb_connected		// Idle, but still connected to the matchmaking 
						// service.
} SBState;

///////////////////////////////////////////////////////////////////////////////
//  SBCallbackReason
//	Brief
//      Callbacks that can occur during server browsing operations.
//
typedef enum{
	sbc_serveradded,				// A game host was added to the list; it 
									// may just have an IP and port at this 
									// point.
	sbc_serverupdated,				// Game host information has been updated; 
									// either basic or full information is now 
									// available about this game host.
	sbc_serverupdatefailed,			// Either a direct or matchmaking service 
									// query to retrieve information about this 
									// game host failed.
	sbc_serverdeleted,				// A game host was removed from the list.
	sbc_updatecomplete,				// The server query engine is now idle.
	sbc_queryerror,					// The matchmaking service returned an 
									// error string for the provided query.
	sbc_serverchallengereceived		// Prequery IP verification challenge was 
									// received (this is informational and no 
									// action is required).
} SBCallbackReason;	

///////////////////////////////////////////////////////////////////////////////
//  SBConnectToServerState
//	Brief
//      States passed to the SBConnectToServerCallback.
//
typedef enum{
	sbcs_succeeded,		// Connected to game host successfully.
	sbcs_failed			// Failed to connect to game host.
} SBConnectToServerState;

///////////////////////////////////////////////////////////////////////////////
//  ServerBrowserCallback
//	Brief
//      The callback provided to ServerBrowserNew. This gets called as the 
//		Server Browser updates the game host list.
//  Parameters
//      sb				[in] The ServerBrowser object to which the callback 
//								refers.
//      reason			[in] The reason for being called. See the Server 
//								Browsing SDK documentation for more details. 
//      server			[in] The game host that is being referred to. 
//      instance		[in] User-provided data.
//  Remarks
//      "instance" is any game-specific data you want passed to the callback 
//		function. For example, you can pass a structure pointer or object 
//		pointer for use within the callback. If you can access any needed data 
//		within the callback already, then you can just pass NULL for 
//		"instance".
//	Example
//		Your callback function should look something like:
//		<code lang="c++">
//void SBCallback(ServerBrowser sb, SBCallbackReason reason, SBServer server, void *instance)
//		{
//			CMyGame *g = (CMyGame *)instance;
//		
//			switch (reason)
//			{
//				case sbc_serveradded :
//					g->ServerView->AddServerToList(server);
//					break;
//				case sbc_serverupdated : 
//					g->ServerView->UpdateServerInList(server);
//					break;
//				case sbc_updatecomplete:
//					g->ServerView->SetStatus("Update Complete");
//					break;
//				case sbc_queryerror:
//					g->ServerView->SetStatus("Query Error Occurred:", 
//					ServerBrowserListQueryError(sb));
//				break;
//			}
//		}
//		
//		Example use of the Callback:
//		int CMyGame::OnMultiplayerButtonClicked(...)
//		{
//		    m_ServerBrowser = ServerBrowserNew("mygame", "mygame", "123456", 
//												0, 10, QVERSION_QR2, 
//												SBCallBack, this);
//		}
//		</code>
//  See Also 
//      ServerBrowserNew
//
typedef void (*ServerBrowserCallback)(
	ServerBrowser		sb, 
	SBCallbackReason	reason, 
	SBServer			server, 
	void				* instance
);

///////////////////////////////////////////////////////////////////////////////
//  SBConnectToServerCallback
//	Brief
//      The callback provided to ServerBrowserConnectToServer. Gets called 
//		when the state of the connect attempt changes.
//  Parameters
//      sb				[in] The ServerBrowser object to which the callback 
//								refers.
//      state			[in] The state of the connect attempt.
//      gameSocket		[in] A UDP socket, ready for use to communicate with 
//								the game host. 
//      instance		[in] User-provided data.
//  Remarks
//      "instance" is any game-specific data you want passed to the callback 
//		function. For example, you can pass a structure pointer or object 
//		pointer for use within the callback. If you can access any needed data 
//		within the callback already, then you can just pass NULL for 
//		"instance".
//  See Also 
//      ServerBrowserConnectToServer
//
typedef void (*SBConnectToServerCallback)(
	ServerBrowser			sb, 
	SBConnectToServerState	state, 
	SOCKET					gamesocket, 
	SOCKADDR_IN				* remoteaddr, 
	void					* instance
);

	// Maximum length for the SQL filter string.
#define MAX_FILTER_LEN 511

	// Version defines for query protocol.
#define QVERSION_GOA 0
#define QVERSION_QR2 1

	/*******************
	ServerBrowser Object Functions
	********************/

#ifndef GSI_UNICODE
#define ServerBrowserNew			ServerBrowserNewA
#define ServerBrowserUpdate			ServerBrowserUpdateA
#define ServerBrowserLimitUpdate	ServerBrowserLimitUpdateA
#define ServerBrowserAuxUpdateIP	ServerBrowserAuxUpdateIPA
#define ServerBrowserRemoveIP		ServerBrowserRemoveIPA
#define ServerBrowserSendNatNegotiateCookieToServer	ServerBrowserSendNatNegotiateCookieToServerA
#define ServerBrowserSendMessageToServer	ServerBrowserSendMessageToServerA
#define ServerBrowserSort			ServerBrowserSortA
#define SBServerGetStringValue		SBServerGetStringValueA
#define SBServerGetIntValue			SBServerGetIntValueA
#define SBServerGetFloatValue		SBServerGetFloatValueA
#define SBServerGetBoolValue		SBServerGetBoolValueA
#define SBServerGetPlayerStringValue	SBServerGetPlayerStringValueA
#define SBServerGetPlayerIntValue	SBServerGetPlayerIntValueA
#define SBServerGetPlayerFloatValue	SBServerGetPlayerFloatValueA
#define SBServerGetTeamStringValue	SBServerGetTeamStringValueA
#define SBServerGetTeamIntValue		SBServerGetTeamIntValueA
#define SBServerGetTeamFloatValue	SBServerGetTeamFloatValueA
#define ServerBrowserListQueryError	ServerBrowserListQueryErrorA
#define ServerBrowserErrorDesc		ServerBrowserErrorDescA
#define ServerBrowserGetServerByIP	ServerBrowserGetServerByIPA
#else
#define ServerBrowserNew			ServerBrowserNewW
#define ServerBrowserUpdate			ServerBrowserUpdateW
#define ServerBrowserLimitUpdate	ServerBrowserLimitUpdateW
#define ServerBrowserAuxUpdateIP	ServerBrowserAuxUpdateIPW
#define ServerBrowserRemoveIP		ServerBrowserRemoveIPW
#define ServerBrowserSendNatNegotiateCookieToServer	ServerBrowserSendNatNegotiateCookieToServerW
#define ServerBrowserSendMessageToServer	ServerBrowserSendMessageToServerW
#define ServerBrowserSort			ServerBrowserSortW
#define SBServerGetStringValue		SBServerGetStringValueW
#define SBServerGetIntValue			SBServerGetIntValueW
#define SBServerGetFloatValue		SBServerGetFloatValueW
#define SBServerGetBoolValue		SBServerGetBoolValueW
#define SBServerGetPlayerStringValue	SBServerGetPlayerStringValueW
#define SBServerGetPlayerIntValue	SBServerGetPlayerIntValueW
#define SBServerGetPlayerFloatValue	SBServerGetPlayerFloatValueW
#define SBServerGetTeamStringValue	SBServerGetTeamStringValueW
#define SBServerGetTeamIntValue		SBServerGetTeamIntValueW
#define SBServerGetTeamFloatValue	SBServerGetTeamFloatValueW
#define ServerBrowserListQueryError	ServerBrowserListQueryErrorW
#define ServerBrowserErrorDesc		ServerBrowserErrorDescW
#define ServerBrowserGetServerByIP	ServerBrowserGetServerByIPW
#endif

//////////////////////////////////////////////////////////////
// ServerBrowserNew
// Summary
//		Initialize the ServerBrowser SDK.
// Parameters
//		queryForGamename	: [in] Game hosts returned will be for this Gamename. 
//		queryFromGamename	: [in] Your assigned GameName. 
//		queryFromKey		: [in] Secret key that corresponds to the 
//									queryFromGamename.
//		queryFromVersion	: [in] Set to zero unless directed otherwise by 
//									GameSpy.
//		maxConcUpdates		: [in] The maximum number of queries the 
//									ServerBrowsing SDK will send out at one 
//									time. Must be set to 20 or greater per 
//									GameSpy Certification Process/GameSpy Open 
//									Usage Guidelines.
//		queryVersion		: [in] The QueryReporting protocol used by the
//									game host. Should be QVERSION_GOA or 
//									QVERSION_QR2. See remarks.
//		lanBrowse			: [in] The switch to turn on only LAN browsing.
//		callback			: [in] Function to be called when the operation 
//									 completes.
//		instance			: [in] Pointer to user data. This is optional and 
//									will be passed unmodified to the callback 
//									function.
// Returns
//		This function returns the initialized ServerBrowser interface. No 
//		return value is reserved to indicate an error.
// Remarks
//		The ServerBrowserNew function initializes the ServerBrowsing SDK. 
//		Developers should then use ServerBrowserUpdate or 
//		ServerBrowserLANUpdate to begin retrieving the list of registered 
//		game hosts.<p>
// Example
//		<code lang="c++">
//#include <sb_serverbrowsing.h> 
//		void main( void )
//		{ 
//			ServerBrowser aServerBrowser = SBServerBrowserNew("gmtest",
//		 "gmtest", "HA6zkS", 0, 20, QVERSION_QR2, SBFalse, SBCallback, NULL);
//
//          ServerBrowserFree(aServerBrowser);
//		}
//		</code>
// See Also
//		ServerBrowserFree, ServerBrowserUpdate, ServerBrowserLANUpdate
COMMON_API ServerBrowser ServerBrowserNew(
	const gsi_char			* queryForGamename, 
	const gsi_char			* queryFromGamename, 
	const gsi_char			* queryFromKey, 
	int						queryFromVersion, 
	int						maxConcUpdates,		
	int						queryVersion, 
	SBBool					lanBrowse,		
	ServerBrowserCallback	callback, 
	void					* instance
);

//////////////////////////////////////////////////////////////
// ServerBrowserFree
// Summary
//		Frees memory allocated by the ServerBrowser SDK. Terminates any pending 
//		queries.
// Parameters
//		sb	: [in] A ServerBrowser interface previously allocated with 
//		ServerBrowserNew.
// Remarks
//		The ServerBrowserFree function frees any allocated memory associated 
//		with the SDK and terminates any pending queries. This function must be 
//		called once for every call to ServerBrowserNew to ensure proper cleanup 
//		of the ServerBrowsing SDK.<p>
// Example
//		<code lang="c++">
//#include <sb_serverbrowsing.h>
//		
//		void main( void )
//		{
//		   ServerBrowser aServerBrowser = SBServerBrowserNew("gmtest",
//		 "gmtest", "HA6zkS", 0, 20, QVERSION_QR2, SBFalse, SBCallback, NULL);
//		
//		    ServerBrowserFree(aServerBrowser);
//		}
//		</code>
// See Also
//		ServerBrowserNew
COMMON_API void ServerBrowserFree(
	ServerBrowser	sb
);

//////////////////////////////////////////////////////////////
// ServerBrowserUpdate
// Summary
//		Retrieves the current list of games from the GameSpy matchmaking 
//		service.
// Parameters
//		sb						: [in] ServerBrowser object initialized with 
//										ServerBrowserNew.
//		async					: [in] When set to SBTrue this function will 
//										run in non-blocking mode.
//		disconnectOnComplete	: [in] When set to SBTrue this function will 
//										terminate the connection with the 
//										GameSpy matchmaking service after the 
//										download is complete.
//		basicFields				: [in] A byte array of basic field identifiers 
//										to retrieve for each server. See 
//										remarks.
//		numBasicFields			: [in] The number of valid fields in the 
//										basicFields array.
//		serverFilter			: [in] SQL-like string used to remove unwanted 
//										game hosts from the downloaded list.
// Returns
//		If an error occurs, a valid SBError error code is returned. Otherwise, 
//		sbe_noerror is returned.
// Remarks
//		The ServerBrowserUpdate function retrieves the current list of game 
//		hosts registered with the GameSpy matchmaking service. As each game 
//		host entry is received, one corresponding call to the SBCallback 
//		function will be made with the status sbc_serveradded. If basic keys 
//		are not yet available (check with SBServerHasBasickeys), another call 
//		will be made for this game host with status sbc_serverupdated.<p>
//		This should only be called based on a user input trigger (e.g., a 
//		'Refresh' button). It should never be called on a timer.
// See Also
//		ServerBrowserNew, ServerBrowserLANUpdate
COMMON_API SBError ServerBrowserUpdate(
	ServerBrowser			sb, 
	SBBool					async, 
	SBBool					disconnectOnComplete, 
	const unsigned char		* basicFields, 
	int						numBasicFields, 
	const gsi_char			* serverFilter
);

//////////////////////////////////////////////////////////////
// ServerBrowserLimitUpdate
// Summary
//		Retrieves the current limited list of games from the GameSpy 
//		matchmaking service. Useful for low-memory systems.
// Parameters
//		sb						: [in] ServerBrowser object intialized with 
//										ServerBrowserNew.
//		async					: [in] When set to SBTrue, this function will 
//										run in non-blocking mode.
//		disconnectOnComplete	: [in] When set to SBTrue this function will 
//										terminate the connection with the 
//										GameSpy matchmaking service after the 
//										download is complete.
//		basicFields				: [in] A byte array of basic field identifiers 
//										to retrieve for each game host. See 
//										remarks.
//		numBasicFields			: [in] The number of valid fields in the 
//										basicFields array.
//		serverFilter			: [in] SQL like string used to remove unwanted 
//										game hosts from the downloaded list.
//		maxServers				: [in] Maximum number of game hosts to be 
//										returned.
// Returns
//		If an error occurs, a valid SBError error code is returned. Otherwise, 
//		sbe_noerror is returned.
// Remarks
//		The ServerBrowserLimitUpdate function retrieves a limited set of the 
//		game hosts registered with the GameSpy matchmaking service. This is 
//		mostly useful for low memory systems such as the DS which may not be 
//		capable of loading an entire game host list.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate
COMMON_API SBError ServerBrowserLimitUpdate(
	ServerBrowser			sb, 
	SBBool					async, 
	SBBool					disconnectOnComplete, 
	const unsigned char		* basicFields, 
	int						numBasicFields, 
	const gsi_char			* serverFilter, 
	int						maxServers
);


//////////////////////////////////////////////////////////////
// ServerBrowserThink
// Summary
//		Allows ServerBrowsingSDK to continue internal processing, including 
//		processing query replies.
// Parameters
//		sb	: [in] ServerBrowser object initialized with ServerBrowserNew.
// Returns
//		If an error occurs, a valid SBError error code is returned. 
//		Otherwise, sbe_noerror is returned.
// Remarks
//		The ServerBrowserThink function is required for the SDK to process 
//		incoming data. Because of the single-threaded design of the GameSpy 
//		SDKs, all data is processed during this call, and processing is 
//		paused when this call is complete. When updating game host lists, this 
//		function should be called as frequently as possible to reduce the 
//		latency associated with game host response times. If this function is 
//		not called often enough, game host pings may be inflated due to 
//		processing delays; ~10ms is ideal.
// See Also
//		ServerBrowserNew
COMMON_API SBError ServerBrowserThink(
	ServerBrowser	sb
);

//////////////////////////////////////////////////////////////
// ServerBrowserLANUpdate
// Summary
//		Retrieves the current list of games broadcasting on the local network.
// Parameters
//		sb				: [in] ServerBrowser object initialized with 
//								ServerBrowserNew.
//		async			: [in] When set to SBTrue this function will run in 
//								non-blocking mode.
//		startSearchPort	: [in] The lowest port the SDK will listen to 
//		broadcasts from, in network byte order.
//		endSearchPort	: [in] The highest port the SDK will listen to
//								broadcasts from, in network byte order.
// Returns
//		If an error occurs, a valid SBError error code is returned. Otherwise, 
//		sbe_noerror is returned.
// Remarks
//		The ServerBrowserLANUpdate function listens for broadcast packets on 
//		the local network. game hosts that are broadcasting within the 
//		specified port range will be detected. As each game host broadcast is 
//		received, one corresponding call to the SBCallbackfunction will be made 
//		with the status sbc_serveradded.<p>
//		Generally this should start with your standard query port, and range 
//		above it, since the QR and QR2 SDKs will automatically allocate higher 
//		port numbers when running multiple game hosts on the same machine. You 
//		should limit your search to 100 ports or less in most cases to prevent 
//		flooding of the LAN with broadcast packets.
// See Also
//		ServerBrowserNew, ServerBrowserUpdate
COMMON_API SBError ServerBrowserLANUpdate(
	ServerBrowser	sb, 
	SBBool			async, 
	unsigned short	startSearchPort, 
	unsigned short	endSearchPort
);

//////////////////////////////////////////////////////////////
// ServerBrowserAuxUpdateIP
// Summary
//		Queries key-values from a single game host.
// Parameters
//		sb			: [in] ServerBrowser object returned from ServerBrowserNew.
//		ip			: [in] Address string of the game host.
//		port		: [in] Query port of the game host.
//		viaMaster	: [in] Set to SBTrue to retrieve cached values from the 
//							matchmaking service.
//		async		: [in] Set to SBTrue to run in non-blocking mode.
//		fullUpdate	: [in] Set to SBTrue to retrieve the full set of key-values 
//							from the game host.
// Returns
//		This function returns sbe_noerror for success. On an error condition, 
//		this function will return an SBError code. If the async option is 
//		SBTrue, the status condition will be reported to the SBCallback.
// Remarks
//		The ServerBrowserAuxUpdateIP function is used to retrieve information 
//		about a specific game host. Information returned is in the form of 
//		key-value pairs and may be accessed through the standard SBServer 
//		object accessors.<p>
// See Also
//		ServerBrowserUpdate, ServerBrowserLANUpdate, ServerBrowserAuxUpdateServer
COMMON_API SBError ServerBrowserAuxUpdateIP(
	ServerBrowser	sb, 
	const gsi_char	* ip, 
	unsigned short	port, 
	SBBool			viaMaster, 
	SBBool			async, 
	SBBool			fullUpdate
);

//////////////////////////////////////////////////////////////
// ServerBrowserAuxUpdateServer
// Summary
//		Query key-values from a single game host that has already been added to 
//		the internal list.
// Parameters
//		sb			: [in] ServerBrowser object returned from ServerBrowserNew.
//		server		: [in] SBServer object for the matchmaking service to 
//							update (usually obtained from SBCallback).
//		async		: [in] Set to SBTrue to run in non-blocking mode.
//		fullUpdate	: [in] Set to SBTrue to retrieve the full set of 
//							key-values from the game host.
// Returns
//		This function returns sbe_noerror for success. On an error condition, 
//		this function will return an SBError code. If the async option is 
//		SBTrue, the status condition will be reported to the SBCallback.
// Remarks
//		The ServerBrowserAuxUpdateServer function is used to retrieve 
//		information about a specific game host. Information returned is in the 
//		form of key-value pairs and may be accessed through the standard 
//		SBServer object accessors.<p>
//		This function is generally used to get additional information about a 
//		game host (for example, to get full rules and player information from a 
//		game host that only has basic information so far), but can also be used 
//		to "refresh" the information about a given game host. Data will 
//		automatically be retrieved from the matchmaking service directly or 
//		from the game host as appropriate. When called asynchronously, 
//		multiple game host update requests can be queued and will be executed 
//		by the query engine in turn.
//		<p>
//		A game host update is only performed when a user selects a specific game host.
//		Full updates for all game hosts on the list are not requested automatically
//		(and ServerBrowserAuxUpdateServer() is not called from within the 
//		ServerBrowserCallback).
// See Also
//		ServerBrowserUpdate, ServerBrowserLANUpdate, ServerBrowserAuxUpdateIP
COMMON_API SBError ServerBrowserAuxUpdateServer(
	ServerBrowser	sb, 
	SBServer		server, 
	SBBool			async, 
	SBBool			fullUpdate
);


//////////////////////////////////////////////////////////////
// ServerBrowserDisconnect
// Summary
//		Disconnect from the GameSpy matchmaking service.
// Parameters
//		sb	: [in] ServerBrowser object initialized with 
//					ServerBrowserNew.
// Remarks
//		The ServerBrowserDisconnect function disconnects a maintained
//		 connection to the GameSpy matchmaking service.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate
COMMON_API void ServerBrowserDisconnect(
	ServerBrowser	sb
);


//////////////////////////////////////////////////////////////
// ServerBrowserState
// Summary
//		Gets current state of the Server Browser object.
// Parameters
//		sb	: [in] ServerBrowser object initialized with ServerBrowserNew.
// Returns
//		Returns the current state.
// Remarks
//		Descriptions of the possible state values can be found in the main 
//		header file.<p>
COMMON_API SBState ServerBrowserState(
	ServerBrowser	sb
);

//////////////////////////////////////////////////////////////
// ServerBrowserRemoveIP
// Summary
//		Removes a game host from the local list.
// Parameters
//		sb		: [in] ServerBrowser object initialized with ServerBrowserNew.
//		ip		: [in] The address of the game host to remove.
//		port	: [in] The port of the game host to remove, in network byte 
//						order.
// Remarks
//		The ServerBrowserRemoveIP function removes a single SBServer from the 
//		local list. This does not affect the matchmaking service or remote 
//		users. Please refer to the QR2 SDK for removing a registered game host 
//		from the matchmaking service list.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserRemoveServer
COMMON_API void ServerBrowserRemoveIP(
	ServerBrowser	sb, 
	const gsi_char	* ip, 
	unsigned short	port
);

//////////////////////////////////////////////////////////////
// ServerBrowserRemoveServer
// Summary
//		Removes a game host from the local list.
// Parameters
//		sb		: [in] ServerBrowser object initialized with ServerBrowserNew.
//		server	: [in] The game host to remove.
// Remarks
//		The ServerBrowserRemoveServer function removes a single SBServer from 
//		the local list. This does not affect the matchmaking service or remote 
//		users. Please refer to the QR2 SDK for removing a registered game host 
//		from the matchmaking service list.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserRemoveIP
COMMON_API void ServerBrowserRemoveServer(
	ServerBrowser	sb, 
	SBServer		server
);

//////////////////////////////////////////////////////////////
// ServerBrowserHalt
// Summary
//		Stop an update in progress. 
// Parameters
//		sb	: [in] ServerBrowser object initialized with ServerBrowserNew.
// Remarks
//		The ServerBrowserHalt function will stop an update in progress. This 
//		is often tied to a "cancel" button presented to the user on the game 
//		host list screen. This function clears any game hosts queued to be 
//		queried, and disconnects from the matchmaking service.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate
COMMON_API void ServerBrowserHalt(
	ServerBrowser	sb
);

//////////////////////////////////////////////////////////////
// ServerBrowserClear
// Summary
//		Clear the current server list.
// Parameters
//		sb	: [in] ServerBrowser object initialized with ServerBrowserNew.
// Remarks
//		The ServerBrowserClear function empties the current list of game hosts 
//		in preparation for a ServerBrowserUpdate or other list populating 
//		call.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserFree
COMMON_API void ServerBrowserClear(
	ServerBrowser	sb
);

//////////////////////////////////////////////////////////////
// ServerBrowserErrorDesc
// Summary
//		Returns a human readable string for the specified SBError.
// Parameters
//		sb		: [in] ServerBrowser object initialized with ServerBrowserNew.
//		error	: [in] A valid SBError code.
// Returns
//		For a valid SBError, this function will return a human readable 
//		description. Otherwise this function returns an empty string.
// Remarks
//		The ServerBrowserErrorDesc function is useful for displaying error 
//		information to a user that might not understand SBError codes. These 
//		descriptions are in English. For localization purposes, you will need 
//		to provide your own translation functions.<p>
// See Also
//		ServerBrowserNew, ServerBrowserListQueryError
COMMON_API const gsi_char *ServerBrowserErrorDesc(
	ServerBrowser	sb, 
	SBError			error
);

//////////////////////////////////////////////////////////////
// ServerBrowserListQueryError
// Summary
//		Returns the ServerList error string, if any.
// Parameters
//		sb	: [in] ServerBrowser object initialized with ServerBrowserNew.
// Returns
//		If a list error has occurred, a string description of the error is 
//		returned. Otherwise, an empty string "" is returned.
// Remarks
//		The ServerBrowserListQueryError function returns the last string 
//		error reported by the matchmaking service. For localization purposes, 
//		you may safely assume that this string will not change, and you can 
//		test for it as you would a numeric error code.<p>
// See Also
//		ServerBrowserNew
COMMON_API const gsi_char *ServerBrowserListQueryError(
	ServerBrowser	sb
);

//////////////////////////////////////////////////////////////
// ServerBrowserGetServer
// Summary
//		Returns the SBServer object at the specified index.
// Parameters
//		sb		: [in] ServerBrowser object initialized with ServerBrowserNew.
//		index	: [in] The array index.
// Returns
//		Returns the SBServer at the specified array index. If index is greater 
//		than the bounds of the array, NULL is returned.
// Remarks
//		Use ServerBrowserCount to retrieve the current number of game hosts in 
//		the array. This index is zero-based, so a list containing 5 game hosts 
//		will have the valid indexes 0 through 4. This list is usually populated 
//		using one of the list retrieval methods such as ServerBrowserUpdate.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate
COMMON_API SBServer ServerBrowserGetServer(
	ServerBrowser	sb, 
	int				index
);

//////////////////////////////////////////////////////////////
// ServerBrowserGetServerByIP
// Summary
//		Returns the SBServer with the specified IP.
// Parameters
//		sb		: [in] ServerBrowser object initialized with ServerBrowserNew.
//		ip		: [in] The dotted IP address of the game host (e.g., 
//						"1.2.3.4").
//		port	: [in] The query port of the game host, in network byte order.
// Returns
//		Returns the game host if found, otherwise NULL;
COMMON_API SBServer ServerBrowserGetServerByIP(
	ServerBrowser	sb, 
	const gsi_char	* ip, 
	unsigned short	port
);

//////////////////////////////////////////////////////////////
// ServerBrowserCount
// Summary
//		Retrieves the current list of games from the GameSpy matchmaking 
//		service.
// Parameters
//		sb	: [in] ServerBrowser object initialized with ServerBrowserNew.
// Returns
//		Returns the number of game hosts in the current list. The index is 
//		zero-based when referencing.
// Remarks
//		The ServerBrowserCount function returns the number of game hosts in the 
//		current list. This may be a combination of game hosts returned by 
//		ServerBrowserUpdate and game hosts added manually by 
//		ServerBrowserAuxUpdateIP. Please note that index functions such as 
//		ServerBrowserGetServer use a zero-based index. The actual valid indexes 
//		are 0 to ServerBrowserCount()-1.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API int ServerBrowserCount(
	ServerBrowser	sb
);

//////////////////////////////////////////////////////////////
// ServerBrowserPendingQueryCount
// Summary
//		Retrieves the number of game hosts with outstanding queries. Use this 
//		to check progress while asynchronously updating the game host list.
// Parameters
//		sb	: [in] ServerBrowser object initialized with ServerBrowserNew.
// Returns
//		Returns the number of game hosts that have not yet been queried.
// Remarks
//		The ServerBrowserPendingQueryCount function is most useful when 
//		updating a large list of game hosts. Use this function to display 
//		progress information to the user (e.g., "1048/2063 game hosts updated", 
//		or as a progress bar graphic).<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate
COMMON_API int ServerBrowserPendingQueryCount(
	ServerBrowser	sb
);

//////////////////////////////////////////////////////////////
// ServerBrowserGetMyPublicIP
// Summary
//		Returns the local client's external (firewall) address.
// Parameters
//		sb	: [in] ServerBrowser object initialized with ServerBrowserNew.
// Returns
//		The local game client's external (firewall) address. This may be 
//		returned as a string or integer address.
// Remarks
//		The ServerBrowserGetMyPublicIP and ServerBrowserGetMyPublicIPAddr 
//		functions return the external address of the local client, as report by 
//		the GameSpy matchmaking service. Because of this, the return value is 
//		only valid after a successful call to ServerBrowserUpdate. The reason 
//		for this is that a client cannot determine their external address 
//		without first sending an outgoing packet. It is up to that packet's
//		receiver to report the public address back to the local client.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate
COMMON_API char *ServerBrowserGetMyPublicIP(
	ServerBrowser	sb
);

//////////////////////////////////////////////////////////////
// ServerBrowserGetMyPublicIPAddr
// Summary
//		Returns the local client's external (firewall) address.
// Parameters
//		sb	: [in] ServerBrowser object initialized with 
//					ServerBrowserNew.
// Returns
//		The local clients external (firewall) address. This may be returned 
//		as a string or integer address.
// Remarks
//		The ServerBrowserGetMyPublicIP and ServerBrowserGetMyPublicIPAddr 
//		functions return the external address of the local client, as reported 
//		by the GameSpy matchmaking service.	Because of this, the return value 
//		is only valid after a successful call to ServerBrowserUpdate. The 
//		reason for this is that a client cannot determine their external 
//		address without first sending an outgoing packet. It is up to that 
//		packet's receiver to report the public address back to the local 
//		client.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate
COMMON_API unsigned int ServerBrowserGetMyPublicIPAddr(
	ServerBrowser	sb
);


//////////////////////////////////////////////////////////////
// ServerBrowserSendNatNegotiateCookieToServer
// Summary
//		Sends a NAT negotiation cookie to the game host. The cookie is sent via 
//		the matchmaking service.
// Parameters
//		sb		: [in] ServerBrowser object initialized with ServerBrowserNew.
//		ip		: [in] Address of the game host in string form (e.g., 
//						"xxx.xxx.xxx.xxx").
//		port	: [in] The query port of the game host to which to relay the 
//						NatNeg cookie, in network byte order.
//		cookie	: [in] An integer cookie value. See remarks.
// Returns
//		If an error occurs, a valid SBError error code is returned. Otherwise, 
//		sbe_noerror is returned.
// Remarks
//		The ServerBrowserSendNatNegotiateCookieToServer function can be used to 
//		relay a NatNegotiation cookie value to a game host behind a firewall. 
//		This cookie is sent through the matchmaking service, since direct 
//		communication with the game host is not always possible. This cookie 
//		may then be used to initiate a nat negotiation attempt. Please refer to 
//		the NatNegotiation SDK documentation for more info.<p>
// See Also
//		ServerBrowserNew, ServerBrowserLANUpdate
COMMON_API SBError ServerBrowserSendNatNegotiateCookieToServer(
	ServerBrowser	sb, 
	const gsi_char	* ip, 
	unsigned short	port, 
	int				cookie
);


//////////////////////////////////////////////////////////////
// ServerBrowserSendMessageToServer
// Summary
//		Sends a game specific message to the specified IP/port. This message is 
//		routed through the matchmaking service.
// Parameters
//		sb		: [in] ServerBrowser object initialized with ServerBrowserNew.
//		ip		: [in] Address of the game host in string form (e.g., 
//						"xxx.xxx.xxx.xxx").
//		port	: [in] The query port of the game host to which to send the 
//						message, in network byte order.
//		data	: [in] The raw data buffer.
//		len		: [in] The length of the data buffer.
// Returns
//		If an error occurs, a valid SBError error code is returned. Otherwise, 
//		sbe_noerror is returned.
// Remarks
//		The ServerBrowserSendMessageToServer function can be used to relay a 
//		raw data buffer to a game host behind a firewall. The raw buffer is 
//		sent through the matchmaking service since direct communication with 
//		the game host is not always possible. The buffer is sent in raw form 
//		to the game host's query port and does not contain any header 
//		information. This message is mostly useful in a shared-socket 
//		QR2 implementation.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate,
//		 ServerBrowserSendNatNegotiateCookieToServer
COMMON_API SBError ServerBrowserSendMessageToServer(
	ServerBrowser	sb, 
	const gsi_char	* ip, 
	unsigned short	port, 
	const char		* data, 
	int				len
);


//////////////////////////////////////////////////////////////
// ServerBrowserConnectToServer
// Summary
//		Connects to a game host.
// Parameters
//		sb			: [in] ServerBrowser object returned from ServerBrowserNew.
//		server		: [in] SBServer object for the server to connect to.
//		callback	: [in] The callback to call when the attempt completes.
// Returns
//		This function returns sbe_noerror for success. On an error condition, 
//		this function will return an SBError code. If there is an error, the 
//		callback will not be called.
//		<p>
//		This function should only be initiated when a player has selected a game 
//		host to connect to.	It should never be used when a player is browsing 
//		for a game host.
// Remarks
//		Connects to a game host, internally using Nat Negotiation if necessary.
//		The callback will be called when the connection attempt completes. If 
//		the attempt is successful, the game host will have its
//		 qr2_clientconnectedcallback_t called.<p>
// See Also
//		QR2\qr2_clientconnectedcallback_t, SBConnectToServerCallback
COMMON_API SBError ServerBrowserConnectToServer(
	ServerBrowser				sb, 
	SBServer					server, 
	SBConnectToServerCallback	callback
);


//////////////////////////////////////////////////////////////
// ServerBrowserConnectToServerWithSocket
// Summary
//		Connects to a game host with the gamesocket provided.
// Parameters
//		sb			: [in] ServerBrowser object returned from ServerBrowserNew.
//		server		: [in] SBServer object for the game host to connect to.
//      gamesocket  : [in] Socket to be used in establishing connection with 
//							the specified game host. This socket must have 
//							already been initialized. 
//		callback	: [in] The callback to call when the attempt completes.
// Returns
//		This function returns sbe_noerror for success. On an error condition, 
//		this function will return an SBError code. If there is an error, the 
//		callback will not be called.
// Remarks
//		Connects to a game host, internally using Nat Negotiation if necessary. 
//      The gamesocket passed in will be used in establishing connection with 
//		the specified game host. The developer is responsible for receiving 
//		traffic on this socket and passing received NN messages to 
//		NNProcessData. NN packets can be identified by the 6 magic bytes that 
//		are used at the beginning of every packet. These are defined in 
//		natneg.h, starting with NN_MAGIC_0.	The callback will be called when 
//		the connection attempt completes. If the attempt is successful, the 
//		game host will have its qr2_clientconnectedcallback_t called.<p>
// See Also
//		QR2\qr2_clientconnectedcallback_t, SBConnectToServerCallback,
//		 natneg\NNProcessData
COMMON_API SBError ServerBrowserConnectToServerWithSocket(
	ServerBrowser				sb, 
	SBServer					server, 
	SOCKET						gamesocket, 
	SBConnectToServerCallback	callback
);


///////////////////////////////////////////////////////////////////////////////
//  SBCompareMode
//	Brief
//      Comparison types for the ServerBrowserSort function.
// See Also
//		ServerBrowserSort
typedef enum{
	sbcm_int,		// Assume the values are int and do an integer comparison.
	sbcm_float,		// Assume the values are float and do a float comparison.
	sbcm_strcase,	// Assume the values are strings and do a case-sensitive 
					// string comparison.
	sbcm_stricase	// Assume the values are strings and do a case-insensitive 
					// string comparison.
} SBCompareMode;


//////////////////////////////////////////////////////////////
// ServerBrowserSort
// Summary
//		Sort the current list of game hosts.
// Parameters
//		sb			: [in] ServerBrowser object initialized with ServerBrowserNew.
//		ascending	: [in] When set to SBTrue this function will sort in 
//							ascending order (i.e., "a-b-c" order, not "c-b-a").
//		sortkey		: [in] The "key" of the key-value pair to sort by.
//		comparemode	: [in] Specifies the data type of the sortkey. See remarks.
// Remarks
//		The ServerBrowserSort function will order the game host list, sorting 
//		by the specified sortkey. Sorting may be in ascending or descending 
//		order and various data-types are supported.
//		SBCompareMode may be one of the following values:<p>
//		sbcm_int: Uses integer comparison ("1,2,3,12,15,20").
//		sbcm_float: Similar to above, but considers decimal values 
//					("1.1,1.2,2.1,3.0").
//		sbcm_strcase: Uses case-sensitive string comparison. Uses strcmp. 
//		sbcm_stricase: Non-case-sensitive string comparision. Uses _stricmp 
//						or equivalent.<p>
//		Please note that calling this function repeatedly for a large game host 
//		list may impact performance due to the standard qsort algorithm being 
//		ineffecient when sorting an already ordered list. This performance 
//		issue is rarely a cause for concern, but certain optimizations may be 
//		made if performance is noticeably impacted.
// See Also
//		ServerBrowserUpdate, ServerBrowserThink
COMMON_API void ServerBrowserSort(
	ServerBrowser	sb, 
	SBBool			ascending, 
	const gsi_char	* sortkey, 
	SBCompareMode	comparemode
);

//////////////////////////////////////////////////////////////
// ServerBrowserLANSetLocalAddr
// Summary
//		Sets the network adapter to use for LAN broadcasts (optional).
// Parameters
//		sb		: [in] ServerBrowser object initialized with ServerBrowserNew.
//		theAddr	: [in] The address to use.
COMMON_API void ServerBrowserLANSetLocalAddr(
	ServerBrowser	sb, 
	const char		* theAddr
);


/*******************
SBServer Object Functions
********************/

//////////////////////////////////////////////////////////////
// SBServerKeyEnumFn
// Summary
//		Callback function used for enumerating the key-value pairs for a game 
//		host.
// Parameters
//		key			: [in] The enumerated key.
//		value		: [in] The enumerated value.
//		instance	: [in] User-provided data.
// See Also
//		SBServerEnumKeys
typedef void (*SBServerKeyEnumFn)(
	gsi_char	* key, 
	gsi_char	* value, 
	void		* instance
);

//////////////////////////////////////////////////////////////
// SBServerGetConnectionInfo
// Summary
//		Checks to see if NAT Negotiation is required, based on whether the 
//		match is a LAN game, a public IP is present and several other factors. 
//		This function fills a supplied pointer with an IP string to use for NAT 
//		Negotiation, or a direct connection if possible.
// Parameters
//		gSB				: [in] ServerBrowser object returned from 
//								ServerBrowserNew.
//		server			: [in] A valid SBServer object.
//		portToConnectTo	: [in] The game port to connect to.
//		ipstring_out	: [out] An IP String you can use for a direct 
//								connection, or with which to attempt NAT 
//								negotiation.
// Returns
//		Returns SBTrue if NAT negotiation is required, SBFalse if not.
// Remarks
//		The connection test will result in one of three scenarios, based on the 
//		return value of the function.<p>
//		Returns SBFalse:
//		1) LAN game: connect using the IP string.
//		2) Internet game with a direct connection available: connect using
//			the IP string.<p>		
//		Returns SBTrue:
//		3) NAT traversal required, perform NAT negotiation with the IP string 
//			before connecting.

COMMON_API SBBool SBServerGetConnectionInfo(
	ServerBrowser	gSB, 
	SBServer		server, 
	gsi_u16			PortToConnectTo, 
	char			* ipstring_out
);


//////////////////////////////////////////////////////////////
// SBServerHasPrivateAddress
// Summary
//		Tests to see if a private address is available for the game host.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		Returns SBTrue if the game host has a private address; otherwise 
//		it returns SBFalse.
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API SBBool SBServerHasPrivateAddress(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerDirectConnect
// Summary
//		Indicates whether the game host supports direct UDP connections.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		Returns SBTrue if a direct connection is possible, otherwise SBFalse.
// Remarks
//		A return of SBFalse usually means that NAT negotiation is required. 
//		Note: this function should only be used to check public game hosts 
//		(where SBServerHasPrivateAddress returns SBFalse).<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API SBBool SBServerDirectConnect(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerGetPing
// Summary
//		Returns the stored ping time for the specified game host.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		The stored game host response time.
// Remarks
//		The SBServerGetPing function will return the stored response time of 
//		the game host. This response time is caculated from the last game host 
//		update.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API int SBServerGetPing(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerGetPublicAddress
// Summary
//		Returns the external address of the SBServer, if any. For users behind 
//		a NAT or firewall, this is the address of the outermost NAT or firewall 
//		layer.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		The public address of the SBServer, in string or integer form.
// Remarks
//		When a client machine is behind a NAT or Firewall device, communication 
//		must go through the public address. The public address of the SBServer 
//		is the address of the outermost Firewall or NAT device.<p> The SBServer 
//		object may be obtained during the SBCallback from ServerBrowserUpdate, 
//		or by calling ServerBrowserGetServer. An SBServer object will only be 
//		accessible for game hosts in the list. 
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API char *SBServerGetPublicAddress(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerGetPrivateAddress
// Summary
//		Returns the internal address of the SBServer, if any. For users behind 
//		a NAT or firewall, this is the local DHCP or assigned IP address of 
//		the machine.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		The private address of the SBServer, in string or integer form.
// Remarks
//		When a client machine is behind a NAT or Firewall device, 
//		communication must go through the public address. The private address 
//		may be used by clients behind the same NAT or Firewall, and may be used 
//		to specifically identify two clients with the same public address. 
//		Often the private address is of the form "192.168.###.###" and is not 
//		usable for communication outside the local network.<p>
//		The SBServer object may be obtained during the SBCallback from 
//		ServerBrowserUpdate, or by calling ServerBrowserGetServer. An SBServer 
//		object will only be accessible for game hosts in the list. 
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API char *SBServerGetPrivateAddress(
	SBServer 	server
);

//////////////////////////////////////////////////////////////
// SBServerGetPublicInetAddress
// Summary
//		Returns the external address of the SBServer, if any. For users behind 
//		a NAT or firewall, this is the address of the outermost NAT or firewall 
//		layer.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		The public address of the SBServer, in string or integer form.
// Remarks
//		When a client machine is behind a NAT or Firewall device, 
//		communication must go through the public address. The public address of 
//		the SBServer is the address of the outermost Firewall or NAT device.<p>
//		The SBServer object may be obtained during the SBCallback from 
//		ServerBrowserUpdate, or by calling ServerBrowserGetServer. An SBServer 
//		object will only be accessible for game hosts in the list. 
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API unsigned int SBServerGetPublicInetAddress(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerGetPrivateInetAddress
// Summary
//		Returns the internal address of the SBServer, if any. For users behind 
//		a NAT or firewall, this is the local DHCP or assigned IP address of the 
//		machine.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		The private address of the SBServer, in string or integer form.
// Remarks
//		When a client machine is behind a NAT or Firewall device, communication 
//		must go through the public address.	The private address may be used by 
//		clients behind the same NAT or Firewall, and may be used to 
//		specifically identify two clients with the same public address.
//		Often the private address is of the form "192.168.###.###" and is not 
//		usable for communication outside the local network.<p>
//		The SBServer object may be obtained during the SBCallback from 
//		ServerBrowserUpdate, or by calling ServerBrowserGetServer. An SBServer 
//		object will only be accessible for game hosts in the list. 
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API unsigned int SBServerGetPrivateInetAddress(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerGetPublicQueryPort
// Summary
//		Returns the public query port of the specified game host. This is the 
//		external port on which the GameSpy matchmaking service communicates 
//		with the game host.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		The public query port.
// Remarks
//		The SBServerGetPublicQueryPort function will return the public query 
//		port of the game host.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API unsigned short SBServerGetPublicQueryPort(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerGetPrivateQueryPort
// Summary
//		Returns the private query port of the specified game host. This is the 
//		internal port on which the game host communicates to the GameSpy 
//		matchmaking service.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		The private query port.
// Remarks
//		The SBServerGetPrivateQueryPort function will return the private query 
//		port of the game host.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API unsigned short SBServerGetPrivateQueryPort(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerHasBasicKeys
// Summary
//		Determine if basic information is available for the specified game 
//		host.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		SBTrue if available; otherwise SBFalse.
// Remarks
//		The SBServerHasBasicKeys function is used to determine if the game 
//		host object has been populated with the 'basicFields' keys as passed to 
//		the ServerBrowserUpdate. Information may not be available if a game 
//		host query is still pending.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API SBBool SBServerHasBasicKeys(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerHasFullKeys
// Summary
//		Determine if full information is available for the specified game host.
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		SBTrue if available; otherwise SBFalse.
// Remarks
//		The SBServerHasFullKeys function is used to determine if the game host 
//		object has been populated with all keys reported by the game host. 
//		'Full' game host information is retrieved after a 
//		ServerBrowserAuxUpdate call. Information may not be available if a game 
//		host query is still pending.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API SBBool SBServerHasFullKeys(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerHasValidPing
// Summary
//		Determines if a game host has a valid ping value (otherwise the ping 
//		will be 0).
// Parameters
//		server	: [in] A valid SBServer object.
// Returns
//		SBTrue if the game host has a valid ping value, otherwise SBFalse.
COMMON_API SBBool SBServerHasValidPing(
	SBServer	server
);

//////////////////////////////////////////////////////////////
// SBServerGetStringValue
// Summary
//		Returns the value associated with the specified key. This value is 
//		returned as the appropriate type. SBBool, float, int, or string.
// Parameters
//		server	: [in] A valid SBServer object.
//		keyname	: [in] The value associated with this key will be returned.
//		def		: [in] The value to return if the key is not found. Note: this 
//						default string will be returned if the key has been 
//						reported as an empty string.
// Returns
//		If the key is invalid or missing, the specified default is returned. 
//		For an existing key, the value is converted from string form to the 
//		appropriate data type.  These functions do not perform any type 
//		checking.
// Remarks
//		These functions are useful for converting custom keys to a native data 
//		type. No type checking is performed, the string value is simply cast 
//		to the appropriate data type. If a key is not found or is reported as 
//		an empty string, the supplied default is returned.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API const gsi_char *SBServerGetStringValue(
	SBServer		server, 
	const gsi_char	* keyname, 
	const gsi_char	* def
);

//////////////////////////////////////////////////////////////
// SBServerGetIntValue
// Summary
//		Returns the value associated with the specified key. This value is 
//		returned as the appropriate type: SBBool, float, int, or string.
// Parameters
//		server		: [in] A valid SBServer object.
//		key			: [in] The value associated with this key will be returned. 
//		idefault	: [in] The value to return if the key is not found.
// Returns
//		If the key is invalid or missing, the specified default is returned. 
//		For an existing key, the value is converted from string form to the 
//		appropriate data type.  These functions do not perform any type 
//		checking.
// Remarks
//		These functions are usefull for converting custom keys to a native 
//		data type. No type checking is performed, the string value is simply 
//		cast to the appropriate data type. If a key is not found, the supplied 
//		default is returned.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API int SBServerGetIntValue(
	SBServer		server, 
	const gsi_char	* key, 
	int				idefault
);

//////////////////////////////////////////////////////////////
// SBServerGetFloatValue
// Summary
//		Returns the value associated with the specified key. This value is 
//		returned as the appropriate type: SBBool, float, int, or string.
// Parameters
//		server		: [in] A valid SBServer object.
//		key			: [in] The value associated with this key will be returned.
//		fdefault	: [in] The value to return if the key is not found.
// Returns
//		If the key is invalid or missing, the specified default is returned. 
//		For an existing key, the value is converted from string form to the 
//		appropriate data type. These functions do not perform any type 
//		checking.
// Remarks
//		These functions are useful for converting custom keys to a native data 
//		type. No type checking is performed, the string value is simply cast to 
//		the appropriate data type. If a key is not found,the supplied default 
//		is returned.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API double SBServerGetFloatValue(
	SBServer		server, 
	const gsi_char	* key, 
	double			fdefault
);

//////////////////////////////////////////////////////////////
// SBServerGetBoolValue
// Summary
//		Returns the value associated with the specified key. This value is 
//		returned as the appropriate type: SBBool, float, int, or string.
// Parameters
//		server		: [in] A valid SBServer object.
//		key			: [in] The value associated with this key will be returned.
//		bdefault	: [in] The value to return if the key is not found.
// Returns
//		If the key is invalid or missing, the specified default is returned. 
//		For an existing key, the value is converted from string form to the 
//		appropriate data type. These functions do not perform any type 
//		checking.
// Remarks
//		These functions are useful for converting custom keys to a native data 
//		type. No type checking is performed, the string value is simply cast to  
//		the appropriate data type. If a key is not found,the supplied default 
//		is returned.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API SBBool SBServerGetBoolValue(
	SBServer		server,
	const gsi_char	* key, 
	SBBool			bdefault
);

//////////////////////////////////////////////////////////////
// SBServerGetPlayerStringValue
// Summary
//		Returns the value associated with the specified player's key. This 
//		value is returned as the appropriate type: float, int, or string.
// Parameters
//		server		: [in] A valid SBServer object.
//		playernum	: [in] The zero based index for the desired player.
//		key			: [in] The value associated with this key will be returned.
//		sdefault	: [in] The value to return if the key is not found. Note: 
//							This default string will be returned if the key has 
//							been reported as an empty string.
// Returns
//		If the player or key is invalid or missing, the specified default is 
//		returned. For an existing key, the value is converted from string form 
//		to the appropriate data type. These functions do not perform any type 
//		checking.
// Remarks
//		These functions are useful for converting custom player keys to a 
//		native data type. No type checking is performed, the string value is 
//		simply cast to the appropriate data type. If a key is not found or is 
//		reported as an empty string, the supplied default is returned.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API const gsi_char *SBServerGetPlayerStringValue(
	SBServer	server, 
	int			playernum, 
	const		gsi_char * key, 
	const		gsi_char * sdefault
);

//////////////////////////////////////////////////////////////
// SBServerGetPlayerIntValue
// Summary
//		Returns the value associated with the specified player's key. This 
//		value is returned as the appropriate type: float, int, or string.
// Parameters
//		server		: [in] A valid SBServer object.
//		playernum	: [in] The zero based index for the desired player.
//		key			: [in] The value associated with this key will be returned.
//		idefault	: [in] The value to return if the key is not found.
// Returns
//		If the player or key is invalid or missing, the specified default is 
//		returned. For an existing key, the value is converted from string form 
//		to the appropriate data type. These functions do not perform any type 
//		checking.
// Remarks
//		These functions are useful for converting custom player keys to a 
//		native data type. No type checking is performed, the string value is 
//		simply cast to the appropriate data type. If a key is not found, the 
//		supplied default is returned.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API int SBServerGetPlayerIntValue(
	SBServer		server, 
	int				playernum, 
	const gsi_char	* key, 
	int				idefault
);

//////////////////////////////////////////////////////////////
// SBServerGetPlayerFloatValue
// Summary
//		Returns the value associated with the specified player's key. This 
//		value is returned as the appropriate type, float, int, or string.
// Parameters
//		server		: [in] A valid SBServer object.
//		playernum	: [in] The zero based index for the desired player.
//		key			: [in] The value associated with this key will be returned.
//		fdefault	: [in] The value to return if the key is not found.
// Returns
//		If the player or key is invalid or missing, the specified default is 
//		returned. For an existing key, the value is converted from string form 
//		to the appropriate data type. These functions do not perform any type 
//		checking.
// Remarks
//		These functions are useful for converting custom player keys to a 
//		native data type. No type checking is performed, the string value is 
//		simply cast to the appropriate data type. If a key is not found, the 
//		supplied default is returned.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API double SBServerGetPlayerFloatValue(
	SBServer		server, 
	int				playernum, 
	const gsi_char	* key, 
	double			fdefault
);

//////////////////////////////////////////////////////////////
// SBServerGetTeamStringValue
// Summary
//		Returns the value associated with the specified team's key. This value 
//		is returned as the appropriate type: float, int, or string.
// Parameters
//		server		: [in] A valid SBServer object.
//		teamnum		: [in] The integer index of the team.
//		key			: [in] The value associated with this key will be returned.
//		sdefault	: [in] The value to return if the key is not found.
// Returns
//		If the key is invalid or missing, the specified default is returned. 
//		For an existing key, the value is converted from string form to the 
//		appropriate data type. These functions do not perform any type 
//		checking.
// Remarks
//		These functions are useful for converting custom keys to a native data 
//		type. No type checking is performed, the string value is simply cast to 
//		the appropriate data type. If a key is not found or is reported as an 
//		empty string, the supplied default is returned.<p>
//		The SBServer object may be obtained during the SBCallback from 
//		ServerBrowserUpdate, or by calling ServerBrowserGetServer. An SBServer 
//		object will only exist for game hosts in the list. IP addresses removed 
//		from the game host list will not have an associated SBServer object.<p>
//		Team indexes are determined on a per-game basis. The only requirement 
//		is that they match the server's reporting indexes.
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API const gsi_char *SBServerGetTeamStringValue(
	SBServer		server, 
	int				teamnum, 
	const gsi_char	* key, 
	const gsi_char	* sdefault
);

//////////////////////////////////////////////////////////////
// SBServerGetTeamIntValue
// Summary
//		Returns the value associated with the specified team's key. This value 
//		is returned as the appropriate type: float, int, or string.
// Parameters
//		server		: [in] A valid SBServer object.
//		teamnum		: [in] The integer index of the team.
//		key			: [in] The value associated with this key will be returned.
//		idefault	: [in] The value to return if the key is not found.
// Returns
//		If the key is invalid or missing, the specified default is returned. 
//		For an existing key, the value is converted from string form to the 
//		appropriate data type. These functions do not perform any type 
//		checking.
// Remarks
//		These functions are useful for converting custom keys to a native data 
//		type. No type checking is performed, the string value is simply cast to 
//		the appropriate data type. If a key is not found, the supplied default 
//		is returned.<p>
//		The SBServer object may be obtained during the SBCallback from
//		ServerBrowserUpdate, or by calling ServerBrowserGetServer. An SBServer 
//		object will only exist for game hosts in the list. IP addresses removed 
//		from the game host list will not have an associated SBServer object.<p>
//		Team indexes are determined on a per-game basis. The only requirement 
//		is that they match the game host's reporting indexes.
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API int SBServerGetTeamIntValue(
	SBServer		server, 
	int				teamnum, 
	const gsi_char	* key, 
	int				idefault
);

//////////////////////////////////////////////////////////////
// SBServerGetTeamFloatValue
// Summary
//		Returns the value associated with the specified team's key. This value 
//		is returned as the appropriate type: float, int, or string.
// Parameters
//		server		: [in] A valid SBServer object.
//		teamnum		: [in] The integer index of the team.
//		key			: [in] The value associated with this key will be returned.
//		fdefault	: [in] The value to return if the key is not found.
// Returns
//		If the key is invalid or missing, the specified default is returned. 
//		For an existing key, the value is converted from string form to the 
//		appropriate data type. These functions do not perform any type 
//		checking.
// Remarks
//		These functions are useful for converting custom keys to a native data 
//		type. No type checking is performed, the string value is simply cast to 
//		the appropriate data type. If a key is not found,the supplied default 
//		is returned.<p>
//		The SBServer object may be obtained during the SBCallback from 
//		ServerBrowserUpdate, or by calling ServerBrowserGetServer. An SBServer 
//		object will only exist for game hosts in the list. IP addresses removed 
//		from the game host list will not have an associated SBServer object.<p>
//		Team indexes are determined on a per-game basis. The only requirement 
//		is that they match the game host's reporting indexes.
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API double SBServerGetTeamFloatValue(
	SBServer		server, 
	int				teamnum, 
	const gsi_char	* key, 
	double			fdefault
);


//////////////////////////////////////////////////////////////
// SBServerEnumKeys
// Summary
//		Enumerates the key-value pairs for a given game host by calling 
//		KeyEnumFn with each key-value. The user-defined instance data will be 
//		passed to the KeyFn callback.
// Parameters
//		server		: [in] A valid SBServer object.
//		KeyFn		: [in] A callback that is called once for each key.
//		instance	: [in] A user-defined data value that will be passed into 
//							each call to KeyFn.
// Remarks
//		The SBServerEnumKeys function is used to list the available keys for a 
//		particular SBServer object. This is often useful when the number of 
//		keys or custom keys is unknown or variable. Most often, the number of 
//		keys is predefined and constant, making this function call unnecessary. 
//		No query is sent when enumerating keys, instead the keys are stored 
//		from the previous game host update.<p>
// See Also
//		ServerBrowserNew, ServerBrowserUpdate, ServerBrowserGetServer
COMMON_API void SBServerEnumKeys(
	SBServer			server, 
	SBServerKeyEnumFn	KeyFn, 
	void				* instance
);


#ifdef __cplusplus
}
#endif

#endif 
