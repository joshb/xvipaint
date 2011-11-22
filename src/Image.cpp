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

#include <cstring>
#include <png.h>
#include "Exception.h"
#include "PngImage.h"

using namespace std;

/*
 * Image
 */
Image::Image()
{
	m_data = NULL;
	m_width = m_height = m_colorComponents = 0;
}

Image::Image(unsigned int width, unsigned int height,
             int colorComponents)
{
	m_width = width;
	m_height = height;
	m_colorComponents = colorComponents;

	m_data = new uint8_t[m_width * m_height * m_colorComponents];
	for(unsigned int i = 0; i < m_width * m_height * m_colorComponents; ++i)
		m_data[i] = 255;
}

Image::~Image()
{
	if(m_data)
		delete [] m_data;
}

string
Image::getFilename() const
{
	return m_filename;
}

const uint8_t *
Image::getData() const
{
	return m_data;
}

unsigned int
Image::getWidth() const
{
	return m_width;
}

unsigned int
Image::getHeight() const
{
	return m_height;
}

int
Image::getNumComponents() const
{
	return m_colorComponents;
}

Color
Image::getPixel(unsigned int x, unsigned int y) const
{
	if(!m_data)
		throw Exception("Image::getPixel(): No image data available");

	if(x >= m_width || y >= m_height)
		throw Exception("Image::getPixel(): Pixel is out of image bounds");

	Color pixel;
	int tmp;

	switch(m_colorComponents) {
		default:
			break;
		case 1:
			pixel.r = pixel.g = pixel.b = m_data[(m_width * y) + x];
			pixel.a = 255;
			break;
		case 2:
			tmp = ((m_width * y) + x) * 2;
			pixel.r = pixel.g = pixel.b = m_data[tmp + 0];
			pixel.a = m_data[tmp + 1];
			break;
		case 3:
			tmp = ((m_width * y) + x) * 3;
			pixel.r = m_data[tmp + 0];
			pixel.g = m_data[tmp + 1];
			pixel.b = m_data[tmp + 2];
			pixel.a = 255;
			break;
		case 4:
			tmp = ((m_width * y) + x) * 4;
			pixel.r = m_data[tmp + 0];
			pixel.g = m_data[tmp + 1];
			pixel.b = m_data[tmp + 2];
			pixel.a = m_data[tmp + 3];
			break;
	}

	return pixel;
}

void
Image::setPixel(unsigned int x, unsigned int y, Color c)
{
	if(!m_data)
		throw Exception("Image::setPixel(): No image data available");

	if(x >= m_width || y >= m_height)
		throw Exception("Image::setPixel(): Pixel is out of image bounds");

	switch(m_colorComponents) {
		default:
			break;
		case 1:
			m_data[(m_width * y) + x] = c.r;
			break;
		case 2:
			m_data[(m_width * 2 * y) + (x * 2) + 0] = c.r;
			m_data[(m_width * 2 * y) + (x * 2) + 1] = c.a;
			break;
		case 3:
			m_data[(m_width * 3 * y) + (x * 3) + 0] = c.r;
			m_data[(m_width * 3 * y) + (x * 3) + 1] = c.g;
			m_data[(m_width * 3 * y) + (x * 3) + 2] = c.b;
			break;
		case 4:
			m_data[(m_width * 4 * y) + (x * 4) + 0] = c.r;
			m_data[(m_width * 4 * y) + (x * 4) + 1] = c.g;
			m_data[(m_width * 4 * y) + (x * 4) + 2] = c.b;
			m_data[(m_width * 4 * y) + (x * 4) + 3] = c.a;
			break;
	}
}

Image *
Image::scale(unsigned int width, unsigned int height)
{
	// make sure the dimensions are valid
	if(width == 0 || height == 0)
		throw Exception("Image::scale(): Invalid width/height (both must be non-zero)");

	// create the new image
	Image *image = new Image(width, height, m_colorComponents);

	// set the pixels of the new image
	for(unsigned int y = 0; y < height; ++y) {
		for(unsigned int x = 0; x < width; ++x) {
			unsigned int src_y = (unsigned int)(((float)y / (float)height) * (float)m_height);
			unsigned int src_x = (unsigned int)(((float)x / (float)width) * (float)m_width);

			image->setPixel(x, y, getPixel(src_x, src_y));
		}
	}

	return image;
}

void
Image::copyFrom(Image *image)
{
	// get smallest dimensions
	unsigned int w = image->getWidth();
	unsigned int h = image->getHeight();
	if(m_width < w)
		w = m_width;
	if(m_height < h)
		h = m_height;

	// copy pixels
	for(unsigned int y = 0; y < h; ++y) {
		for(unsigned int x = 0; x < w; ++x)
			setPixel(x, y, image->getPixel(x, y));
	}
}

void
Image::save(const char *filename)
{
	int colorType;
	switch(m_colorComponents) {
		default:
			throw Exception("Image::save(): Invalid number of color components");
			break;
		case 1:
			colorType = PNG_COLOR_TYPE_GRAY;
			break;
		case 2:
			colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
			break;
		case 3:
			colorType = PNG_COLOR_TYPE_RGB;
			break;
		case 4:
			colorType = PNG_COLOR_TYPE_RGB_ALPHA;
			break;
	}

	FILE *fp = fopen(filename, "wb");
	if(!fp) {
		throw Exception(string("Image::save(): Unable to open ") + filename + " for writing");
		return;
	}

	png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png) {
		fclose(fp);
		throw Exception("Image::save(): png_create_write_struct failed");
		return;
	}

	png_infop info = png_create_info_struct(png);
	if(!info) {
		png_destroy_write_struct(&png, NULL);
		fclose(fp);
		throw Exception("Image::save(): png_create_info_struct failed");
		return;
	}

	if(setjmp(png_jmpbuf(png))) {
		png_destroy_write_struct(&png, &info);
		fclose(fp);
		throw Exception("Image::save(): setjmp failed");
		return;
	}

	png_init_io(png, fp);

	png_set_IHDR(png, info, m_width, m_height, 8, colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	png_bytepp rows = new png_bytep[m_height];
	for(unsigned int i = 0; i < m_height; ++i)
		rows[i] = m_data + (m_width * m_colorComponents * i);

	png_set_rows(png, info, rows);
	png_write_png(png, info, PNG_TRANSFORM_IDENTITY, NULL);

	png_destroy_write_struct(&png, &info);
	fclose(fp);

	delete [] rows;
}

void
Image::save(const string &filename)
{
	save(filename.c_str());
}

Image *
Image::load(const string &filename)
{
	// get file extension position
	size_t tmp = filename.find_last_of('.');
	if(tmp == string::npos)
		throw Exception(string("Image::load(): No file extension: ") + filename);

	// get file extension and use it to call the
	// appropriate function to load the image
	string extension = filename.substr(tmp);
	if(strcasecmp(extension.c_str(), ".png") == 0)
		return PngImage::load(filename.c_str());
	else
		throw Exception(string("Image::load(): Unsupported file extension: ") + filename);
}

Image *
Image::load(const char *filename)
{
	return load(string(filename));
}
