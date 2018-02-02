=============
Release Notes
=============

File:  	ReadMe.txt
Version:	2011-11-09
Release:	v1.00

Copyright Notice: This file and the package it was included in are part of the
GameSpy SDKs designed and developed by GameSpy Technology / IGN, Inc. 

Copyright (c) 2009 - 2011 GameSpy Technology / IGN, Inc.

========
Overview
========

GameSpy Technology is pleased to provide this Query and Reporting 2 (QR2) 
cross-platform console test app, which reports a sample server to the backend.
TheGameSpy QR2 SDK is used to report a game host (a dedicated server or 
individual peer) to the GameSpy master server backend. To obtain the
information reported from these game hosts you must use the Server Browsing
SDK.
 
For more information on using QR2 in general, please visit our wiki or 
developer forums at poweredbygamespy.com.

=======================
Running the Application
=======================

1. Open qr2_vs2005.sln in Microsoft Visual Studio 2005. 

2. Go to line 485 (which begins with "“_tcscpy(gamedata.hostname) and change
   the hostname from “GameSpy QR2 Sample” to a different name.
   
3. Save, then go to Build -> Build Settings and Build the solution.

4. Click "Start Debugging;" the test application will run in a console window.

Once reporting, the server info (including player/team info) can be retrieved
using the ServerBrowsing SDK. After 30 seconds, the mapname is updated and
qr2_send_statechanged is called so that the master retrieves it. The sample 
shuts down after 60 seconds, removing the server from the master list. To
ensure that the server is reporting correctly, you can check the master server
list here (look for the hostname you changed on line 485 of qr2_vs2005.sln):
http://tools.poweredbygamespy.com/masterserver/

===================
Command Line Inputs
===================
Optional:
- ip (x.x.x.x:port) => specify only if you want to override your IP (otherwise
qr2 will set for you)

============
Dependencies
============

Use the master server list site for verification, or you can run this test app
concurrently with sbctest to have all the reported keys queried and displayed
(wait a few seconds after starting qr2csample to ensure it is listed on the
Master Server before running sbctest).

For debug output, add GSI_COMMON_DEBUG to the preprocessor definitions.

For Unicode, add GSI_UNICODE to the preprocessor definitions (or use the Visual
Studio Project's Unicode configuration).

=======================
Internal Functions Used
=======================

 _tprintf   - Unicode compatible version of printf
 _tcscpy    - Unicode compatible version of strcpy
 _tcscmp    - Unicode compatible version of strcmp
 _T         - used on all string literals for Unicode compatibility
 msleep     - Cross-platform compatible version of sleep
 GSI_UNUSED - used to avoid unreferenced variable compiler warnings  



