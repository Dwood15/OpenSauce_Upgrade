=============
Release Notes
=============

File:		ReadMe.txt
Version:	2011-11-09
Release:	v1.00

Copyright Notice: This file and the package it was included in are part of the
GameSpy SDKs designed and developed by GameSpy Technology / IGN, Inc. 

Copyright (c) 2009 - 2011 GameSpy Technology / IGN, Inc.

========
Overview
========

The StoreSavedGameAndRetrieve sample uses Sake to create a record with dummy
data intended to emulate a saved game. Once this record is created, the sample
then retrieves the record (using sakeGetMyrecords) and iterates through the 
data, printing it to the console. Note that to accomplish this in your game 
you'll want to use sakeUpdateRecord (rather than sakeCreateRecord) if the 
player's record already exists. You can run the Release configuration to avoid 
debug output and make it easier to follow along.

Note: These samples are very simple implementations intended to be used as a 
basic reference to help you learn and implement the SDKs. They are not cross-
platform and don't have Unicode configurations. You can check out the test apps
within each SDK directory for a more thorough conglomeration of cross-platform
calls with Unicode support.

=================== 
Command Line Inputs
===================

None

============
Dependencies
============

This is a standalone app.

===========================
Custom Preprocessor Defines
===========================

SAKE_SAMPLE - ensures the Sake-related common helper functions get defined.
 
 