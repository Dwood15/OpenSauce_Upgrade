# Common Makefile include for Linux
# Copyright 2009 GameSpy Industries

# Project Makefile must define the following:
# TARGET - name of output executable
# SRC - list of source files
# THREADS - "no" if project should not use threads
# DEBUG - "yes" if project should use debugging flags
#
# Optionally it may define CFLAGS and LDFLAGS for additional compiler or linker
# options, respectively, before including this file.

CC=gcc

BASE_CFLAGS= -W -D_LINUX -Wall -Werror -pipe

ifeq ($(THREADS),no)
	BASE_CFLAGS:=$(BASE_CFLAGS) -DGSI_NO_THREADS
else
	LDFLAGS:=$(LDFLAGS) -lpthread
endif

ifeq ($(DEBUG),yes)
	LIBDIR = $(SRCROOT)/../libs/AuthService/linuxDebug
	BASE_CFLAGS:=$(BASE_CFLAGS) -DGSI_COMMON_DEBUG -D_DEBUG
	CFLAGS:=$(BASE_CFLAGS) $(CFLAGS) $(INCLUDES) -g
else
	LIBDIR = $(SRCROOT)/../libs/AuthService/linuxRelease
	CFLAGS:=$(BASE_CFLAGS) $(CFLAGS) $(INCLUDES) -O2
endif

OBJ=$(SRC:.c=.o)

#############################################################################
# SETUP AND BUILD
#############################################################################

#all: dep $(OUT)
all: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@ $(LIBDIR) $(LDFLAGS)
	
$(TARGET): $(OBJ)
	ar rcs $(TARGET) $(OBJ)
	mkdir -p $(LIBDIR) ; cp -f $(TARGET) $(LIBDIR)

depend: dep

dep:
	gcc $(CFLAGS) -MM $(SRC)

.PHONY: clean
clean:	
	rm $(TARGET) $(OBJ) $(LIBDIR)/$(TARGET)


