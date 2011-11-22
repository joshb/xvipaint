/*
 * Copyright (C) 2010-2011 Josh A. Beam
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

#include <xviweb/String.h>
#include "Color.h"

using namespace std;

Color::Color()
{
	r = g = b = a = 255;
}

Color::Color(uint8_t rParam, uint8_t gParam, uint8_t bParam, uint8_t aParam)
{
	this->r = rParam;
	this->g = gParam;
	this->b = bParam;
	this->a = aParam;
}

Color::Color(uint32_t c)
{
	r = (c >> 24) & 0xff;
	g = (c >> 16) & 0xff;
	b = (c >> 8) & 0xff;
	a = c & 0xff;
}

Color::Color(float rParam, float gParam, float bParam, float aParam)
{
	this->r = (uint8_t)(rParam * 255.0f);
	this->g = (uint8_t)(gParam * 255.0f);
	this->b = (uint8_t)(bParam * 255.0f);
	this->a = (uint8_t)(aParam * 255.0f);
}

Color::Color(const string &s)
{
	if(s.length() == 7) {
		r = (uint8_t)String::hexToUInt(s, 1, 2);
		g = (uint8_t)String::hexToUInt(s, 3, 2);
		b = (uint8_t)String::hexToUInt(s, 5, 2);
		a = 255;
	} else {
		r = g = b = a = 255;
	}
}

Color
Color::operator * (const Color &color) const
{
	Color tmp;

	tmp.setR(getFloatR() * color.getFloatR());
	tmp.setG(getFloatG() * color.getFloatG());
	tmp.setB(getFloatB() * color.getFloatB());
	tmp.setA(getFloatA() * color.getFloatA());

	return tmp;
}

void
Color::operator *= (const Color &color)
{
	setR(getFloatR() * color.getFloatR());
	setG(getFloatG() * color.getFloatG());
	setB(getFloatB() * color.getFloatB());
	setA(getFloatA() * color.getFloatA());
}

Color
Color::operator * (float f) const
{
	Color tmp;

	tmp.setR(getFloatR() * f);
	tmp.setG(getFloatG() * f);
	tmp.setB(getFloatB() * f);
	tmp.setA(getFloatA() * f);

	return tmp;
}

void
Color::operator *= (float f)
{
	setR(getFloatR() * f);
	setG(getFloatG() * f);
	setB(getFloatB() * f);
	setA(getFloatA() * f);
}

float
Color::getFloatR() const
{
	return (float)r / 255.0f;
}

float
Color::getFloatG() const
{
	return (float)g / 255.0f;
}

float
Color::getFloatB() const
{
	return (float)b / 255.0f;
}

float
Color::getFloatA() const
{
	return (float)a / 255.0f;
}

void
Color::setR(uint8_t value)
{
	r = value;
}

void
Color::setR(float value)
{
	r = (uint8_t)(value * 255.0f);
}

void
Color::setG(uint8_t value)
{
	g = value;
}

void
Color::setG(float value)
{
	g = (uint8_t)(value * 255.0f);
}

void
Color::setB(uint8_t value)
{
	b = value;
}

void
Color::setB(float value)
{
	b = (uint8_t)(value * 255.0f);
}

void
Color::setA(uint8_t value)
{
	a = value;
}

void
Color::setA(float value)
{
	a = (uint8_t)(value * 255.0f);
}

Color
Color::invert() const
{
	return Color(255 - r, 255 - g, 255 - b, a);
}

uint32_t
Color::toUInt32() const
{
	return ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | (uint32_t)a;
}
