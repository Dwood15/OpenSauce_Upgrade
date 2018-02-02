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

The ReportStats app generates some sample match data for multiple players
(defaulting to 3) for a racing game, then reports it as Keys via the ATLAS SDK to
aggregate the corresponding Stats. Note that these Stats can then be retrieved
(also via the ATLAS SDK) using Queries defined on the ATLAS Web Admin Panel 
(see the QueryStats sample). You can run the Release configuration to avoid 
debug output and make it easier to follow along.

For the sake of ease we have all players report from this same app, although
for your game the players will report from their separate game clients. Also,
all players use 'Authoritative' reporting, which means that majority-rule will
decide which value to keep in the case of conflicting values in the session
reports.

Refer to http://docs.poweredbygamespy.com/wiki/ATLAS_-_Walking_through_a_Ruleset
for an overview and walk-through of the ATLAS Ruleset used by this Sample.

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