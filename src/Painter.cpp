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
#include <cmath>
#include <cstdlib>
#include "Exception.h"
#include "Painter.h"

using namespace std;

Painter::Painter()
{
	// create brushes
	m_brush32 = Image::load("Images/Brush32.png");
	m_brush16 = Image::load("Images/Brush16.png");
	m_brush8 = Image::load("Images/Brush8.png");
	m_brush4 = Image::load("Images/Brush4.png");
	m_brush2 = Image::load("Images/Brush2.png");
}

Painter::~Painter()
{
	// delete brushes
	delete m_brush32;
	delete m_brush16;
	delete m_brush8;
	delete m_brush4;
	delete m_brush2;
}

Image *
Painter::brushFromSize(int size)
{
	switch(size) {
		default:
			throw Exception("Painter::brushFromSize(): Invalid brush size");
		case 2:
			return m_brush2;
		case 4:
			return m_brush4;
		case 8:
			return m_brush8;
		case 16:
			return m_brush16;
		case 32:
			return m_brush32;
	}
}

void
Painter::drawDot(Image *image, unsigned int x, unsigned int y,
                 const Color &color, int size)
{
	Image *brush = brushFromSize(size);

	unsigned int brushWidth = brush->getWidth();
	unsigned int brushHeight = brush->getHeight();
	unsigned int imageWidth = image->getWidth();
	unsigned int imageHeight = image->getHeight();

	x -= brushWidth / 2;
	y -= brushHeight / 2;

	for(unsigned int i = 0; i < brushHeight; ++i) {
		unsigned int destY = y + i;
		if(destY >= imageHeight)
			continue;

		for(unsigned int j = 0; j < brushWidth; ++j) {
			unsigned int destX = x + j;
			if(destX >= imageWidth)
				continue;

			Color tmp = brush->getPixel(j, i);
			if(tmp.a != 0) {
#if 0
				Color color1 = image->getPixel(destX, destY);
				Color color2 = tmp * color;

				float a2 = color2.getFloatA();
				float a1 = 1.0f - a2;
				float r = color1.getFloatR() * a1 + color2.getFloatR() * a2;
				float g = color1.getFloatG() * a1 + color2.getFloatG() * a2;
				float b = color1.getFloatB() * a1 + color2.getFloatB() * a2;

				image->setPixel(destX, destY, Color(r, g, b));
#else
				image->setPixel(destX, destY, color);
#endif
			}
		}
	}
}

void
Painter::drawLine(Image *image, float x1, float y1, float x2, float y2,
                  const Color &color, int size)
{
	float xdiff = (x2 - x1);
	float ydiff = (y2 - y1);

	if(xdiff == 0.0f && ydiff == 0.0f) {
		drawDot(image, (unsigned int)x1, (unsigned int)y1, color, size);
		return;
	}

	if(fabs(xdiff) > fabs(ydiff)) {
		float xmin, xmax;

		// set xmin to the lower x value given
		// and xmax to the higher value
		if(x1 < x2) {
			xmin = x1;
			xmax = x2;
		} else {
			xmin = x2;
			xmax = x1;
		}

		// draw line in terms of y slope
		float slope = ydiff / xdiff;
		for(float x = xmin; x <= xmax; x += 1.0f) {
			float y = y1 + ((x - x1) * slope);
			drawDot(image, (unsigned int)x, (unsigned int)y, color, size);
		}
	} else {
		float ymin, ymax;

		// set ymin to the lower y value given
		// and ymax to the higher value
		if(y1 < y2) {
			ymin = y1;
			ymax = y2;
		} else {
			ymin = y2;
			ymax = y1;
		}

		// draw line in terms of x slope
		float slope = xdiff / ydiff;
		for(float y = ymin; y <= ymax; y += 1.0f) {
			float x = x1 + ((y - y1) * slope);
			drawDot(image, (unsigned int)x, (unsigned int)y, color, size);
		}
	}
}

void
Painter::processLine(Image *image, int brushSize, const Color &brushColor,
                     const string &line)
{
	// parse coordinates
	int coords[4];
	char *s = (char *)line.c_str();
	coords[0] = (int)strtol(s, &s, 10);
	coords[1] = (int)strtol(s, &s, 10);
	coords[2] = (int)strtol(s, &s, 10);
	coords[3] = (int)strtol(s, NULL, 10);

	drawLine(image, (float)coords[0], (float)coords[1], (float)coords[2], (float)coords[3], brushColor, brushSize);

}

void
Painter::processUpdate(Image *image, int brushSize, const Color &brushColor,
                       const string &lines)
{
	size_t index = 0;
	size_t length;
	do {
		// get next line
		length = lines.find(';', index);
		if(length != string::npos)
			length -= index;
		string line = lines.substr(index, length);
		index += length + 1;

		for(size_t i = 0; i < line.length(); ++i) {
			if(line[i] == ',')
				line[i] = ' ';
		}
		processLine(image, brushSize, brushColor, line);
	} while(length != string::npos);
}
