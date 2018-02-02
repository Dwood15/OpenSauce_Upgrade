#Builds LibAuthService.a
# This Makefile declares the paths and the source code settings for the library to be build 
#source files.

TARGET = libAtlas.a
THREADS=no
DEBUG=no
CFLAGS:= $(CFLAGS) -W
SRCROOT = ../..
SRC =  	$(SRCROOT)/sciInterface.c \
	$(SRCROOT)/sciMain.c \
	$(SRCROOT)/sciReport.c \
	$(SRCROOT)/sciSerialize.c \
	$(SRCROOT)/sciWebServices.c

# include directories
INCLUDES =	-I$(SRCROOT) \
		-I$(SRCROOT)/.. \
		-I$(SRCROOT)/../common \
		-I$(SRCROOT)/../ghttp \
		-I$(SRCROOT)/../webservices

include ./GameSpyCommonLib.mk
