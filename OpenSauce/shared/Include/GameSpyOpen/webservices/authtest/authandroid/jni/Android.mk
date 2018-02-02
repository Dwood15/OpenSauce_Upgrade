# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
$(shell cp -f ../../../libs/AuthService/androidRelease/libAuthservice.a ./obj/local/armeabi/)
LOCAL_MODULE    := 	Authservice
include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE    := 	authtest
LOCAL_CFLAGS    := 	-Werror -W -Wno-missing-field-initializers -D _LINUX -D ANDROID -D GSI_COMMON_DEBUG -I../../../../common -I../../../../natneg -I../../../../qr2 -I../../../../ghttp  
LOCAL_SRC_FILES := 		./authtest.c\
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


LOCAL_LDLIBS    := 	-llog -landroid 
LOCAL_STATIC_LIBRARIES := android_native_app_glue Authservice

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
