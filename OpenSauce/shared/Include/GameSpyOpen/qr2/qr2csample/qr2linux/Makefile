# Query & Reporting 2 SDK Makefile
# Copyright 2004 GameSpy Industries

TARGET=qr2csample
THREADS=no
DEBUG=no
CFLAGS:=$(CFLAGS) -W -Wno-missing-field-initializers
SRCROOT = ../..
SRC = \
	../../../md5c.c\
	../../../darray.c\
	../../../hashtable.c\
	../../../common/linux/LinuxCommon.c\
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
	../../../ghttp/ghttpBuffer.c\
	../../../ghttp/ghttpCallbacks.c\
	../../../ghttp/ghttpConnection.c\
	../../../ghttp/ghttpEncryption.c\
	../../../ghttp/ghttpMain.c\
	../../../ghttp/ghttpProcess.c\
	../../../ghttp/ghttpCommon.c\
	../../../ghttp/ghttpPost.c\
	../../qr2.c\
	../../qr2regkeys.c\
	../../../natneg/natneg.c\
	../../../natneg/NATify.c\
	../qr2csample.c

# include directories
INCLUDES = -I$(SRCROOT) -I$(SRCROOT)/.. -I$(SRCROOT)/../common -I$(SRCROOT)/../ghttp -I$(SRCROOT)/../gt2 -I$(SRCROOT)/../webservices

LDFLAGS:= $(LDFLAGS) -lAuthservice

include ../../../common/linux/MakeCommon.mk

