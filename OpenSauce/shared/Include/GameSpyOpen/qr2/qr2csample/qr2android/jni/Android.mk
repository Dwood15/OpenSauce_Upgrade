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

LOCAL_MODULE    := 	qr2native
LOCAL_CFLAGS    := 	-Werror -W -Wno-missing-field-initializers -D _LINUX -D ANDROID -D GSI_COMMON_DEBUG -I../../../../ -I../../../../common -I../../../../natneg -I../../../../qr2 -I../../../../ghttp -I../../../../gt2 -I../../../../webservices
LOCAL_SRC_FILES := 	qr2main.c \
					../../../../md5c.c\
					../../../../darray.c\
					../../../../hashtable.c\
					../../../../common/gsAssert.c\
					../../../../common/gsAvailable.c\
					../../../../common/gsCore.c\
					../../../../common/gsDebug.c\
					../../../../common/gsCrypt.c\
					../../../../common/gsLargeInt.c\
					../../../../common/gsStringUtil.c\
					../../../../common/gsPlatform.c\
					../../../../common/gsPlatformSocket.c\
					../../../../common/gsPlatformThread.c\
					../../../../common/gsPlatformUtil.c\
					../../../../common/gsMemory.c\
					../../../../common/gsXML.c\
					../../../../common/gsSHA1.c\
					../../../../common/gsRC4.c\
					../../../../common/gsSSL.c\
					../../../../common/gsSoap.c\
					../../../../common/linux/gsThreadLinux.c \
					../../../../ghttp/ghttpBuffer.c\
					../../../../ghttp/ghttpCallbacks.c\
					../../../../ghttp/ghttpConnection.c\
					../../../../ghttp/ghttpEncryption.c\
					../../../../ghttp/ghttpMain.c\
					../../../../ghttp/ghttpProcess.c\
					../../../../ghttp/ghttpCommon.c\
					../../../../ghttp/ghttpPost.c\
					../../../../qr2/qr2.c \
					../../../../qr2/qr2regkeys.c \
    				../../../../natneg/natneg.c \
    				../../../../natneg/NATify.c
LOCAL_LDLIBS    := 	-llog -landroid
LOCAL_STATIC_LIBRARIES := android_native_app_glue Authservice

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
