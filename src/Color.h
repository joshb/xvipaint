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

#ifndef __COLOR_H__
#define __COLOR_H__

#include <string>

class Color
{
	public:
		uint8_t r, g, b, a;

		Color();
		Color(uint8_t rParam, uint8_t gParam, uint8_t bParam, uint8_t aParam = 255);
		Color(uint32_t c);
		Color(float rParam, float gParam, float bParam, float aParam = 1.0f);
		Color(const std::string &s);

		Color operator * (const Color &color) const;
		void operator *= (const Color &color);

		Color operator * (float f) const;
		void operator *= (float f);

		float getFloatR() const;
		float getFloatG() const;
		float getFloatB() const;
		float getFloatA() const;

		void setR(uint8_t value);
		void setR(float value);

		void setG(uint8_t value);
		void setG(float value);

		void setB(uint8_t value);
		void setB(float value);

		void setA(uint8_t value);
		void setA(float value);

		Color invert() const;

		uint32_t toUInt32() const;
};

#endif /* __COLOR_H__ */
