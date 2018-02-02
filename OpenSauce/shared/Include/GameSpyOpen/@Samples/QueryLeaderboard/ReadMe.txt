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

The QueryLeaderboard app uses the ATLAS Query system to make a simple
leaderboard query, retrieving the top 5 players, sorted by win-loss ratio. The
query was created via the ATLAS Web Admin Panel within the Ruleset described in
the following Wiki article:
http://docs.poweredbygamespy.com/wiki/ATLAS_-_Walking_through_a_Ruleset

You can run the Release configuration to avoid debug output and make it easier
to follow along.

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

ATLAS_SAMPLE - ensures the ATLAS-related common helper functions get defined.



 
 