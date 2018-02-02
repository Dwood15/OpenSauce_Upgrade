# Query & Reporting 2 SDK Makefile - Mac OSX
# Copyright 2006 GameSpy Industries

PROJECT=qr2csample

DEFINES=-D_MACOSX -DGSI_NO_THREADS

DEBUG=yes

CFLAGS := $(CFLAGS) -Wno-missing-field-initializers

SRCROOT= ../..

SRC = \
	../../../md5c.c\
	../../../darray.c\
	../../../hashtable.c\
	../../../common/gsAssert.c\
	../../../common/gsAvailable.c\
	../../../common/gsCore.c\
	../../../common/gsDebug.c\
	../../../common/gsCrypt.c\
	../../../common/gsLargeInt.c\
	../../../common/gsStringUtil.c\
	../../../common/gsPlatform.c\
	../../../common/gsPlatformSocket.c\
	../../../common/gsPlatformThread.c\
	../../../common/gsPlatformUtil.c\
	../../../common/gsMemory.c\
	../../../common/gsXML.c\
	../../../common/gsSHA1.c\
	../../../common/gsRC4.c\
	../../../common/gsSSL.c\
	../../../common/gsSoap.c\
	../../../common/macosx/MacOSXCommon.c\
	../../../ghttp/ghttpBuffer.c\
	../../../ghttp/ghttpCallbacks.c\
	../../../ghttp/ghttpConnection.c\
	../../../ghttp/ghttpEncryption.c\
	../../../ghttp/ghttpMain.c\
	../../../ghttp/ghttpProcess.c\
	../../../ghttp/ghttpCommon.c\
	../../../ghttp/ghttpPost.c\
	../../../natneg/natneg.c\
	../../../natneg/NATify.c\
	../../qr2.c\
	../../qr2regkeys.c\
	../qr2csample.c

# include directories
INCLUDES = -I$(SRCROOT) -I$(SRCROOT)/.. -I$(SRCROOT)/../common -I$(SRCROOT)/../ghttp

LDFLAGS:= $(LDFLAGS) -lAuthservice

include ../../../common/macosx/MakeCommon.mk
