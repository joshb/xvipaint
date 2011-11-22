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
#include "PaintResponder.h"
#include "PaintContext.h"
#include "Exception.h"
#include "Util.h"

const char *CANVAS_PATH = "Canvas.png";

using namespace std;

PaintResponder::PaintResponder()
{
	m_updateId = 0;
	m_userCount = 0;

	m_painter = new Painter();
	try {
		m_image = Image::load(CANVAS_PATH);
	} catch(Exception ex) {
		// create a new image if one doesn't already exist
		m_image = new Image(800, 450, 3);
		m_image->save(CANVAS_PATH);
	}
	m_lastSaveTime = getMilliseconds();
}

PaintResponder::~PaintResponder()
{
	delete m_painter;
	m_image->save(CANVAS_PATH);
	delete m_image;
}

static bool
validateLines(const string &s)
{
	vector <string> lines = String::split(s, ";");
	for(unsigned int i = 0; i < lines.size(); ++i) {
		// invalid if the number of coordinates
		// given does not equal 4
		vector <string> coords = String::split(lines[i], ",");
		if(coords.size() != 4)
			return false;

		for(unsigned int j = 0; j < coords.size(); ++j) {
			for(unsigned int k = 0; k < coords[j].length(); ++k) {
				// invalid if any coordinate contains non-digits
				char c = coords[j][k];
				if(c < '0' || c > '9')
					return false;
			}
		}
	}

	return true;
}

void
PaintResponder::updateImage()
{
	// if the image was last updated more than 15 seconds ago, update it
	long time = getMilliseconds();
	if((time - m_lastSaveTime) > 15000) {
		m_image->save(CANVAS_PATH);
		m_lastSaveTime = time;

		// delete all updates more than 10 seconds old
		while(m_updates.size() != 0 && (time - m_updates[0].updateTime) > 10000)
			m_updates.erase(m_updates.begin());
	}
}

void
PaintResponder::handlePostUpdate(const HttpRequest *request,
                                 HttpResponse *response)
{
	updateImage();

	// get the update data and store it
	PaintUpdate update;
	update.lines = request->getPostDataValue("l");
	if(validateLines(update.lines)) {
		update.userId = String::toInt(request->getPostDataValue("u"));
		update.brushSize = String::toInt(request->getPostDataValue("s"));
		update.brushColor = request->getPostDataValue("c");

		update.updateTime = getMilliseconds();
		update.updateId = ++m_updateId;
		m_updates.push_back(update);

		m_painter->processUpdate(m_image, update.brushSize, Color(update.brushColor), update.lines);
	}

	response->sendResponse(200, "OK", "text/plain", "");
}

int
PaintResponder::getUpdateId() const
{
	return m_updateId;
}

string
PaintResponder::getUpdates(int userId, int userLastUpdateId)
{
	updateImage();

	// loop through updates and add the ones
	// not made by this user to the response
	string response;
	for(unsigned int i = 0; i < m_updates.size(); ++i) {
		if(m_updates[i].userId == userId)
			continue;

		if(m_updates[i].updateId <= userLastUpdateId)
			continue;

		response += String::fromInt(m_updates[i].updateId);
		response += " " + String::fromInt(m_updates[i].brushSize);
		response += " " + m_updates[i].brushColor;
		response += " " + m_updates[i].lines;
		response += "\n";
	}

	return response;
}

int
PaintResponder::getUserCount() const
{
	return m_userCount;
}

void
PaintResponder::incrementUserCount()
{
	++m_userCount;
}

void
PaintResponder::decrementUserCount()
{
	--m_userCount;
}

bool
PaintResponder::matchesRequest(const HttpRequest *request) const
{
	return (request->getPath().find("/PaintAction") != string::npos);
}

ResponderContext *
PaintResponder::respond(const HttpRequest *request, HttpResponse *response)
{
	string path = request->getPath();
	if(path.find("/PostUpdate") != string::npos) {
		handlePostUpdate(request, response);
	} else if(path.find("/GetUpdates") != string::npos) {
		return new PaintContext(request, response, this);
	} else {
		response->endResponse();
	}

	return NULL;
}

XVIWEB_RESPONDER(PaintResponder);
