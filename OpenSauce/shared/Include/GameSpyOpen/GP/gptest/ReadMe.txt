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

GameSpy Technology is pleased to provide this MFC test application for our
Presence and Messaging (or GP for GameSpy Presence) SDK. The GP SDK can 
be used by a game to provide account creation, account sign-in, and "buddy" 
functionality, such as buddy lists, statuses, and instant messaging.

For more information about the C-language GP SDK, please visit our wiki or 
developer forums at poweredbygamespy.com.

=======================
Running the Application
=======================

1. Open gp_vs2005.sln in Microsoft Visual Studio 2005. 

2. In the Solution Explorer window, navigate through the Source Files directory
   and open gptest.cpp.
   
3. Go to Build -> Build Settings and Build the solution.

4. Click “Start Debugging” to run the application, and a window will pop up for
   the GP test app.   
   
5. Enter an email, nick, and password, then click "Initialize" and "Connect" to
   connect to the GP service. If you need to add a new user, enter an e-mail,
   password, uniquenick, and use "0" for the productid, then click the "New
   User" button.
   
Note: You can simulate multiple players using Presence and Messaging features 
by running multiple instances of the GP MFC test application and connecting 
with different credentials in each instance.

------------
Login Errors
------------
You'll notice that there's an error pane in the upper-right corner of the GP
test application. The most common set of errors you might receive while using
the GP test app are related to bad login credentials:

* 0x0102 (GP_LOGIN_BAD_NICK): The nickname provided is incorrect.

* 0x0103 (GP_LOGIN_BAD_EMAIL): The e-mail address provided is incorrect.

* 0x0104 (GP_LOGIN_BAD_PASSWORD): The password provided is incorrect.

* 0x0109 (GP_LOGIN_BAD_UNIQUENICK): The uniquenick provided is incorrect.


========
Features
========

------
Search
------
GP allows players to search for other players by entering any of the following:
nick, uniquenick, real names, or e-mail. You'll see search results in the 
"RESULTS" pane.

--------------
Buddy Requests
--------------
Once you've searched for a user, you can use the "Send Buddy Request" button to
Send a Buddy Request (or block communication with a user with the "Add Block" 
button). 
* To send a request, select a name from the search results and click on
  “Send Buddy Request.”
  
* You also have the option to customize the message delivered with the buddy 
  request by entering a message in the “reason” field.
  
* The target player will receive a buddy request with your message, and if they
  accept it, you will receive an acceptance message in return.
  
Note that unlike Facebook, buddy relationships in GP are non-symmetrical; if
player A adds player B as a buddy that doesn't mean player B has player A as a
buddy.

------------------
Player Information
------------------
Once someone has accepted your buddy request, you can click on their name in
the “Buddies” window to view their status and information.

---------------
Player Messages
---------------
GP also allows you to send messages to a buddy you select by typing in a 
message in the Messaging pane and clicking "Send."

-------------
Player Status
-------------
You can also set your status by entering text in the “statusString” field and
clicking “Set Status.” When a buddy clicks your name in his buddy list, he’ll
see your updated status.

--------------------
Additional Functions
--------------------

The GameSpy Presence and Messaging SDK has a great deal of additional 
functionality, most of which you can exercise through this test application. We
recommend exploring the application and its functions to learn even more about
GP.

