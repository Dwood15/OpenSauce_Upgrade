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

GameSpy Technology is pleased to provide its authtest account authentication 
client to game developers on the Android platform. This ReadMe and the files
in this directory support the authtest app for Android. The test app can serve
as a guide in implementing and testing GameSpy account authentication services
via NativeActivity Mode in your Android games.

Note: This sample application was tested with target Android Version 3.1 / API 
Level 12.

This package includes the following:

------------------
In the Jni Folder:
------------------

1. The main authtest client file for Android (authtest.c), providing authtest
   functionality for Android projects.
2. An Android.mk file written for the authtest client for Android
   projects.
3. The Application.mk file designating the application platform version.

-------------------------------------
Other Files in the Authandroid Folder:
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

The following code changes are required for porting sample applications for 
authtest to NativeActivity:

In the case of authtest, we would like to abstract the implementation of our
AuthService from the developer, so we have compiled AuthService code as a 
static library, which will be included in the makefile for authtest. We have
maintained the AuthService library in the /libs folder inside OpenSDK and we
have written a script to copy the library into the authtest's workspace. The 
Android.mk file looks a bit different from that of the other clients: 

LOCAL_PATH&nbsp;:= $(call my-dir)
include $(CLEAR_VARS)

$(shell cp -f ../../../libs/androidRelease/libAuthservice.a ./obj/local/armeabi/)

LOCAL_MODULE&nbsp;:= Authservice
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE&nbsp;:= authtest
LOCAL_CFLAGS&nbsp;:= -Werror -W -Wno-missing-field-initializers -D _LINUX -D ANDROID -D GSI_COMMON_DEBUG 
                -I../../../../common -I../../../../natneg -I../../../../qr2 -I../../../../ghttp
LOCAL_SRC_FILES&nbsp;:= ./authtest.c\
        ../../../../common/gsMemory.c\
        ../../../../darray.c\
        ../../../../hashtable.c\
        ../../../../md5c.c\
        ../../../../common/gsSSL.c\
        ../../../../common/gsAvailable.c\
        ../../../../common/gsDebug.c\
        ../../../../common/gsStringUtil.c\
        ../../../../common/gsPlatform.c\
        ../../../../common/gsPlatformSocket.c\
        ../../../../common/gsPlatformThread.c\
        ../../../../common/gsPlatformUtil.c\
        ../../../../common/gsSoap.c\
        ../../../../common/gsAssert.c\
        ../../../../common/gsCore.c\
        ../../../../common/gsXML.c\
        ../../../../common/gsCrypt.c\
        ../../../../common/gsRC4.c\
        ../../../../common/gsLargeInt.c\
        ../../../../common/gsSHA1.c\
        ../../../../ghttp/ghttpBuffer.c\
        ../../../../ghttp/ghttpCallbacks.c\
        ../../../../ghttp/ghttpConnection.c\
        ../../../../ghttp/ghttpEncryption.c\
        ../../../../ghttp/ghttpMain.c\
        ../../../../ghttp/ghttpProcess.c\
        ../../../../ghttp/ghttpCommon.c\
        ../../../../ghttp/ghttpPost.c


LOCAL_LDLIBS&nbsp;:= -llog -landroid
LOCAL_STATIC_LIBRARIES&nbsp;:= android_native_app_glue Authservice

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)

Here you can see the script written for copying libAuthservice.a from the 
OpenSDK/libs directory to the local project folder for authtest. Also notice 
that include$(BUILD_STATIC_LIBRARY) comes before 
include$(BUILD_SHARED_LIBRARY). This enables the Android NDK to add the 
Authservice static library as a dependency for authtest. 

Now we come to the actual code changes required for porting authtest to 
NativeActivity. We have to add an event handler as described in the previous 
section. The code for exiting authtest is: 

        // Check if we are exiting.
        if (state->destroyRequested != 0)
        {
            // shutdown core before quitting
            gsCoreShutdown();

            // Wait for core shutdown
            // (should be instantaneous unless you have multiple cores)
            while(gsCoreIsShutdown() == GSCore_SHUTDOWN_PENDING)
            {
                gsCoreThink(0);
                msleep(5);
            }
            LOGI("Done.\r\n");
            return;
        }

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
4.  Browse to the ./webservices/authtest/authandroid/ directory.
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
  +-webservices\
    +-authtest\
      +-authandroid\
        +-jni\
          +-Android.mk
          +-authtest.c

