=============
Release Notes
=============

File:  	ReadMe.txt
Version:	2011-09-02
Release:	v1.00

Copyright Notice: This file and the package it was included in are part of the 
GameSpy SDKs designed and developed by GameSpy Technology / IGN, Inc. 

Copyright (c) 2009 - 2011 GameSpy Technology / IGN, Inc.

========
Overview
========

GameSpy Technology is pleased to provide its Query and Reporting 2 (QR2) 
matchmaking client to game developers on the Android platform. This ReadMe and 
the files in this directory support the QR2 test app for Android. The test app 
can serve as a guide in implementing and testing GameSpy matchmaking services 
via NativeActivity Mode in your Android games.

Note: This sample application was tested with target Android Version 3.1 / API 
Level 12.

This package includes the following:

------------------
In the Jni Folder:
------------------

1. The main Query and Reporting 2 client file for Android (qr2main.c), 
   providing QR2 functionality for Android projects.
2. An Android.mk file written for the QR2 client for Android projects.
3. The Application.mk file designating the application platform version.

-------------------------------------
Other Files in the Qr2Android Folder:
-------------------------------------

Default files and directories (res\ and src\) for Android projects.

============
Code Changes
============

The following code changes were made for NativeActivity:

1. We have implemented an Event-Handler for the sample application. Below is a piece 
   of C code which implements a basic event-handler: 

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
            // shutdown code before quitting
            // Add your piece of code here
            LOGI("Done.\r\n");
            return;
        }
     } 
	 
The event-handler is a big "while-loop" where the first statement 
(ALooper_pollAll) polls for any pending events from the Android event queue. 

Keep in mind that: 

A. The first argument can be a -1 or a 0, depending on whether you want 
   the polling to be blocking or non-blocking.
B. The last parameter is the user-data, the handling of which you can 
   write a separate handler for. This will get invoked at line 6 
   (source->process(state, source);). 
C. When a user presses back button, state->destroyRequested is set to 1, so 
   you can check for it inside the code and add your piece of shutdown code 
   here.

2. We have included the following additional header files: 

   #include <jni.h> 
   #include <errno.h>
   #include <android/log.h>
   #include <android_native_app_glue.h>

3. We have defined Android logging in place of printfs, thus replacing all 
   printf statements with Android logs: 
   #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, 
   "YOUR_TAG", __VA_ARGS__))
4. We have changed the int test_main(int argc, char **argv) function to void 
   android_main(struct android_app* state); the content of the function 
   remains the same except for some additional lines of code.
5. We have added the app_dummy() function call to establish the glue between 
   NativeActivity and Android run-time. 

The following code changes are required for porting the QR2 Sample Application
to NativeActivity:

We have added the event-handler as described in the previous section. In the
previous section, we left a comment in the place where we check if we are 
exiting (line 13 onwards). In the case of QR2, the shutdown code is: 

	LOGI(_T("Shutting down - server will be removed from the master server 
	list"));
	//let gamemaster know we are shutting down (removes dead server entry from 
	the list)

	qr2_shutdown(NULL);
	#ifdef GSI_UNICODE
	// In Unicode mode we must perform additional cleanup
	qr2_internal_key_list_free();
	#endif
	// Callback all suceeded?
	if(callback_check != CALLBACK_OK)
	{
		LOGI(_T("Error with Callback Functions (callback_check = 0x%x)"), 
		callback_check);
	}
	return;

=======================
Android Button Behavior
=======================

Back: Exits the application in a clean manner, initiating proper shutdown 
      operations for all the components involved. 
Home: Minimizes the application, returning the user to the home page; the 
      user may return to the application at a later point.
List: This button is similar to the "Recent Applications" button in Windows; 
      it lists all the recent applications invoked by the current user. 

=======================
Building in Native Code
=======================

1.  Go to File -> New -> Project...
2.  Select "Android Project."
3.  Select "Create project from existing source."
4.  Browse to the ./qr2/qr2csample/qr2android/ directory.
5.  Select Finish.
6.  Go to File -> New -> Other...
7.  Select "Convert to a C/C++ Project."
8.  Select the Project, then "Makefile Project," then "-- Other Toolchain --".
9.  Right-click on Project and go to Properties.
10. Under C/C++ Build, set the Build command to where the 
    "android-ndk-r5c/ndk-build" file is located.
11. Right-click on Project, then select Run As -> Android Application.

==============
Included Files
==============

(Default Android project files and directories are omitted in the list below.)

+-OpenSDK\
  +-qr2\
    +-qr2csample\
      +-qr2android\
        +-jni\
          +-Android.mk
          +-qr2main.c


