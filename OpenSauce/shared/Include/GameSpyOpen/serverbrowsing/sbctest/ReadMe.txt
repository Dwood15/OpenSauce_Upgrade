=============
Release Notes
=============

File: 	ReadMe.txt
Version:	2011-11-11
Release:	v1.00

Copyright Notice: This file and the package it was included in are part of the
GameSpy SDKs designed and developed by GameSpy Technology / IGN, Inc. 

Copyright (c) 2009 - 2011 GameSpy Technology / IGN, Inc.

========
Overview
========

GameSpy Technology is pleased to provide this cross-platform console test
appliction for our ServerBrowsing (SB) SDK that harnesses the calls from the 
SDK to run a server browser, which queries the GameSpy Master Server for all 
running servers of the specified game ('gmtest' in this case). After retrieving
and outputing the basic server information for all listed servers, the server 
list is then sorted by ping time and redisplayed. After this, the server list 
is refreshed, this time using a server filter to narrow down the servers 
retrieved. Then, if the QR2 sample is running, an AuxUpdate is done to get
and output all of its server/player/team keys. 

=======================
Running the Application
=======================

1. Open serverbrowsing_vs2005.sln in Microsoft Visual Studio 2005.

2. In the Solution Explorer window, within the sbctest_vs2005 project,
   navigate through the Source Files directory and open sctest.c.
   
3. Go to Build -> Build Settings and Build the solution.

4. Make sure that sbctest_vs2005 is set as the Startup Project, then click 
   “Start Debugging” to run the application; a console window will pop up for
   sbctest.c.
   
5. Let sbctest.c finish running and it will automatically close the console 
   window. You can read the information that sbctest.c prints down in the 
   Output pane in Visual Studio 2005.
   
===================
Command Line Inputs
===================

None

============
Dependencies
============
* The SB test app should be run concurrently with qr2csample (wait a few 
  seconds after starting qr2csample to ensure it is listed on the Master
  Server). 
  
* For debug output, add GSI_COMMON_DEBUG to the preprocessor definitions.

* For Unicode, add GSI_UNICODE to the preprocessor definitions (or use the 
  Visual Studio Project's Unicode configuration).

=======================
Internal Functions Used
=======================
 _tprintf   - Unicode compatible version of printf
 _tcscpy    - Unicode compatible version of strcpy
 _tcscmp    - Unicode compatible version of strcmp
 _T         - used on all string literals for Unicode compatibility
 msleep     - Cross-platform compatible version of sleep
 GSI_UNUSED - used to avoid unreferenced variable compiler warnings 