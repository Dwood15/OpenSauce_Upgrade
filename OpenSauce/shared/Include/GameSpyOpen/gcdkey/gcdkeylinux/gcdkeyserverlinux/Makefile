# CDKey SDK Server
# Copyright 2004 GameSpy Industries

TARGET=gcdkeyserver
THREADS=yes
DEBUG=no
CFLAGS=-W -I.. -I../..
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
	../../gcdkeys.c\
	../../ServerTest/gcdkeyservertest.c\

# include directories
INCLUDES = -I$(SRCROOT) -I$(SRCROOT)/.. -I$(SRCROOT)/../common -I$(SRCROOT)/../ghttp -I$(SRCROOT)/../webservices

LDFLAGS:= $(LDFLAGS) -lAuthservice

include ../../../common/linux/MakeCommon.mk

