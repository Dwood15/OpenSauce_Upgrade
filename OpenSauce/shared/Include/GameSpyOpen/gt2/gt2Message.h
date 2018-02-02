///////////////////////////////////////////////////////////////////////////////
// File:	gt2Message.h
// SDK:		GameSpy Transport 2 SDK
//
// Copyright (c) 2012 GameSpy Technology & IGN Entertainment, Inc.  All rights 
// reserved. This software is made available only pursuant to certain license 
// terms offered by IGN or its subsidiary GameSpy Industries, Inc.  Unlicensed
// use or use in a manner not expressly authorized by IGN or GameSpy Technology
// is prohibited.

#ifndef _GT2_MESSAGE_H_
#define _GT2_MESSAGE_H_

#include "gt2Main.h"

GT2Bool gti2ReceiveMessages(GT2Socket socket);

GT2Bool gti2Send(GT2Connection connection, const GT2Byte * message, int len, GT2Bool reliable);

GT2Bool gti2SendAppReliable(GT2Connection connection, const GT2Byte * message, int len);
GT2Bool gti2SendAppUnreliable(GT2Connection connection, const GT2Byte * message, int len);
GT2Bool gti2SendClientChallenge(GT2Connection connection, const char challenge[GTI2_CHALLENGE_LEN]);
GT2Bool gti2SendServerChallenge(GT2Connection connection, const char response[GTI2_RESPONSE_LEN], const char challenge[GTI2_CHALLENGE_LEN]);
GT2Bool gti2SendClientResponse(GT2Connection connection, const char response[GTI2_RESPONSE_LEN], const char * message, int len);
GT2Bool gti2SendAccept(GT2Connection connection);
GT2Bool gti2SendReject(GT2Connection connection, const GT2Byte * message, int len);
GT2Bool gti2SendClose(GT2Connection connection);
GT2Bool gti2SendKeepAlive(GT2Connection connection);
GT2Bool gti2SendAck(GT2Connection connection);
GT2Bool gti2SendNack(GT2Connection connection, unsigned short SNMin, unsigned short SNMax);
GT2Bool gti2SendPing(GT2Connection connection);
GT2Bool gti2SendPong(GT2Connection connection, GT2Byte * message, int len);
GT2Bool gti2SendClosed(GT2Connection connection);
GT2Bool gti2SendClosedOnSocket(GT2Socket socket, unsigned int ip, unsigned short port);

GT2Bool gti2ResendMessage(GT2Connection connection, GTI2OutgoingBufferMessage * message);

GT2Bool gti2HandleConnectionReset(GT2Socket socket, unsigned int ip, unsigned short port);
GT2Bool gti2HandleHostUnreachable(GT2Socket socket, unsigned int ip, unsigned short port, GT2Bool send);
GT2Bool gti2WasMessageIDConfirmed(const GT2Connection connection, GT2MessageID messageID);

#endif
