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

The SaveAndRetrieveScreenshot sample uploads a screenshot (gamespylogo.bmp, in 
the project directory) to the Sake File Server, then stores the returned FileID
in a Sake record along with some other associated metadata. We then go through
the process of retrieving the file with the assumption that we're starting
fresh, which involves retrieving our own metadata record in order to get the 
FileID, and then downloading the file. You can run the Release configuration to
avoid debug output and make it easier to follow along.

Note that for this sample we created a Sake table (via the Sake Web Admin
Panel) that we called "ScreenShots"; this is a Profile-owned table with a Limit
Per Owner set to 1, which means that our sample user can only store one record.
This is the table we use to store the file metadata.

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