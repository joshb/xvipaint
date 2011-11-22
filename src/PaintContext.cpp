/*
 * Copyright (C) 2011 Josh A. Beam
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <iostream>
#include <xviweb/String.h>
#include "PaintContext.h"
#include "PaintResponder.h"

using namespace std;

PaintContext::PaintContext(const HttpRequest *request,
                           HttpResponse *response,
                           PaintResponder *responder)
{
	m_responder = responder;

	m_userId = String::toInt(request->getQueryStringValue("u"));
	m_userLastUpdateId = String::toInt(request->getQueryStringValue("i"));

	m_lastUserCount = 0;
	m_responder->incrementUserCount();

	response->setStatus(200, "OK");
	response->setContentType("text/plain");
	string tmp = "hi:";
	for(int i = 0; i < 2048; ++i)
		tmp += "z";
	response->sendString(tmp + "\n");
	continueResponse(request, response);
}

PaintContext::~PaintContext()
{
	m_responder->decrementUserCount();
}

ResponderContext *
PaintContext::continueResponse(const HttpRequest * /*request*/,
                               HttpResponse *response)
{
	string updates = m_responder->getUpdates(m_userId, m_userLastUpdateId);

	// send the current user count along with any updates
	int userCount = m_responder->getUserCount();
	if(userCount != m_lastUserCount) {
		m_lastUserCount = userCount;
		updates = string("uo:") + String::fromInt(userCount) + '\n' + updates;
	}

	if(updates.length() != 0)
		response->sendString(updates);

	m_userLastUpdateId = m_responder->getUpdateId();
	return this;
}

long
PaintContext::getResponseInterval() const
{
	return 50;
}
