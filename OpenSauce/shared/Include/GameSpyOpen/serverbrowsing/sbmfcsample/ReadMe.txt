=============
Release Notes
=============

File:		ReadMe.txt
Version:	2011-11-11
Release:	v1.00

Copyright Notice: This file and the package it was included in are part of the
GameSpy SDKs designed and developed by GameSpy Technology / IGN, Inc. 

Copyright (c) 2009 - 2011 GameSpy Technology / IGN, Inc.

========
Overview
========

GameSpy Technology is pleased to provide this MFC test application for
our ServerBrowsing (SB) SDK. The SB SDK allows you to retrieve a list of
game hosts to use within your game. You can use this retrieval tool to
automatically grab the best game hosts and join them (i.e., in a quickmatch),
or if you prefer, display a browsing interface for your users to individually
select a game host. SB allows you to retrieve data reported by Query and
Reporting 2. As such, you can run the QR2 console test app in tandem with the
SB MFC app, which will help you better understand how the two systems integrate
with each other.

For more information about the SB SDK, please visit our Wiki or
developer forums at poweredbygamespy.com.

=======================
Running the Application
=======================

1. Open sbmfcsample_vs2005.sln in Microsoft Visual Studio 2005.

2. In the Solution Explorer window, within the sbmfcsample_vs2005 project,
   navigate through the Source Files directory and open sbmfcsample.cpp.
   
3. Go to Build -> Build Settings and Build the solution.

4. Make sure that sbmfcsample is set as the Startup Project, then click “Start
   Debugging” to run the application; a window will pop up for sbmfcsample.
   
5. Enter “gmtest” (or another gamename) in the gamename window and click
   “Refresh.”

Once the test app has run, Server Browsing will show you all the players in a
given game when you click on the corresponding server name along with the
information being reported by the Query and Reporting 2 SDK.

After you’re finished with the Server Browsing test application, click “Close” 
to end it.

========
Features
========

The ServerBrowsing test application provides a server list with a full GUI for 
testing. Your in-game server browser will most likely looking something like 
this, with the MFC code replaced by your custom GUI code. 

------------------
Server Information
------------------
Running the application and choosing a server name will show you server-
specific information such as Server Name, Pink, Players, Map, and GameType. You
will also be able to see the names of players on that server, and their pings
and scores.

----------------
Server Searching
----------------
You also have the option to search for a specific gamename and filter search
results. Some common search strings might be: 

* gametype = 'ctf' - Only returns games whose gametype key matches 'ctf'

* numplayers > 0 and numplayers != maxplayers - Only returns servers that have
  players on them but are not full
  
* password = 0 - Only returns servers that are not passworded (assuming you use
  a "password" key to indicate password protected servers)
  
* hostname like '%[gsf]%' - Only returns servers that have the string '[gsf]' 
  somewhere in their name; could be used, for example, to allow a player to
  find just the servers their clan runs
  
* (country = 'DE' or country = 'FR') and maxplayers >= 8 - Only returns servers
  located in Germany or France that support 8 or more players

----------------
Server Filtering
----------------
* You can also select the options to filter by Internet, LAN, and QR2 if you 
  are running the Query and Reporting 2 test application in tandem with 
  ServerBrowsing. 
  
* Note that if you are filtering by LAN you will also have to specify the range
  of ports on which to check for servers.
  
* You'll want to keep the QR2 filter option checked unless you're searching on 
  a very old game -- GOA is the name of the precursor to QR2 and was used over
  five years ago.