/*
   psimage.cpp : This file is part of pstoedit.
  
   Copyright (C) 1997- 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

   Support for Image::writeIdrawImage by Scott Johnston

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
#include "cppcomp.h"

#ifdef HAVE_LIBGD
#include <gd.h>
#endif

#include <cmath>
#include <algorithm>

#include "drvbase.h"
#include "pstoedit_config.h"
 
#include I_iomanip

using std::dec;
using std::setw;
using std::setfill;
using std::hex;

#if 0
//{ should be obsolete - since now PNG images are written directly by Ghostscript


//#define HAVE_LIBPNG
#ifdef HAVE_LIBPNG
bool PSImage::PNGSupported()
{
	return true;
}

#include "png.h"
static void user_error_ptr(png_structp, png_const_charp)
{
}
static void user_error_fn(png_structp, png_const_charp)
{
}
static void user_warning_fn(png_structp, png_const_charp)
{
}


void PSImage::writePNGImage(const char *FileNameP, const char *source,
						  const char *title, const char *generator) const
{
	FILE *fp = fopen(FileNameP, "wb");
	if (!fp) {
		cerr << "error in open " << FileName << endl;
		return;
	}

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
												  (png_voidp) user_error_ptr, user_error_fn,
												  user_warning_fn);
	if (!png_ptr) {
		cerr << "error in png_create_write_struct " << endl;
		return;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		cerr << "error in png_create_info_struct " << endl;
		png_destroy_write_struct(&png_ptr, nullptr);
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		cerr << "error in libpng " << endl;
		png_destroy_write_struct(&png_ptr, nullptr);
		return;
	}

	png_init_io(png_ptr, fp);
//  const int width  = imageinfo.width;
//  const int height = imageinfo.height;
	const int depth = 8;		// bits; (4) - getcomponent scales to 0..255
	// otherwise we need more space as well below.
//  const int numcolors = ncomp; // assume 3 for the moment (RGB)
	const bool isgray = (ncomp == 1);

	png_set_IHDR(png_ptr, info_ptr, width,	// width
				 height,		// height
				 depth,			// depth
				 isgray ? PNG_COLOR_TYPE_GRAY : PNG_COLOR_TYPE_RGB,	// color_type
				 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	{
		png_text text[3];

		text[0].compression = PNG_TEXT_COMPRESSION_NONE;
		text[0].key = "Title";
		text[0].text = (char *) title;

		text[1].compression = PNG_TEXT_COMPRESSION_NONE;
		text[1].key = "Software";
		text[1].text = (char *) generator;

		text[2].compression = PNG_TEXT_COMPRESSION_NONE;
		text[2].key = "Source";
		text[2].text = (char *) source;

		png_set_text(png_ptr, info_ptr, text, 3);
	}

	const unsigned int nrofcolorsforpng = isgray ? 1 : 3;

	png_bytepp row_pointers = (png_bytepp) png_malloc(png_ptr,
													  height * sizeof(png_bytep) *
													  nrofcolorsforpng);
	{
		for (unsigned int y = 0; y < height; y++) {
			row_pointers[y] =
				(png_bytep) png_malloc(png_ptr, width * sizeof(png_byte) * nrofcolorsforpng);
			png_bytep row = row_pointers[y];
			// cout << endl << "%% " << width << " " << height << " " << nrofcolorsforpng << " " << bits << " " << ncomp << " " << isgray << endl;
			for (unsigned int x = 0; x < width; x++) {

				if (isgray) {
					row[x] = getComponent(x, y, 0);
					// cout << (((unsigned int) row[x]) ? ' ' : '#' );
				} else {

					unsigned char r, g, b;
					// how many components?
					switch (ncomp) {
					case 1:
						// should not happen anymore (see if isgray)
						r = g = b = getComponent(x, y, 0);
						break;
					case 3:
						r = getComponent(x, y, 0);
						g = getComponent(x, y, 1);
						b = getComponent(x, y, 2);
						break;

					case 4:
						{
							// need int for CMY in order to allow += later on
							unsigned int C = getComponent(x, y, 0);
							unsigned int M = getComponent(x, y, 1);
							unsigned int Y = getComponent(x, y, 2);
							unsigned int K = getComponent(x, y, 3);

							// account for key
							// see pslrm page 307
							C += K;
							M += K;
							Y += K;

							if (C > 255)
								C = 255;
							if (M > 255)
								M = 255;
							if (Y > 255)
								Y = 255;

							// convert color
							r = 255 - C;
							g = 255 - M;
							b = 255 - Y;
						}
						break;
					default:
						cerr << "\t\tFatal: unexpected case in image (line " << __LINE__ << ")" <<
							endl;
						abort();
						return;
					}

					row[3 * x] = r;
					row[3 * x + 1] = g;
					row[3 * x + 2] = b;
				}
			}
		}
	}

	png_set_rows(png_ptr, info_ptr, row_pointers);

	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

// free memory  !!!!!!!!!!!!!!!!

	{
		for (unsigned int y = 0; y < height; y++) {
			png_free(png_ptr, row_pointers[y]);
		}
	}
	png_free(png_ptr, row_pointers);

	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);
}


#else
bool PSImage::PNGSupported()
{
	return false;
}
void PSImage::writePNGImage(const char *FileName, const char *source,
						  const char *title, const char *generator) const
{
}

#endif

// end obsolete }
#endif

unsigned char PSImage::getComponent(unsigned int x, unsigned int y, char numComponent) const
{
	if (isFileImage) {
		cerr << "Image::getComponent not yet supported for PNG File Image objects" << endl;
		return 0;
	}

	assert(data != nullptr);
	// all data in one row are filled up to by a multiple of 8
	// see PS manual §4.10.2

	// which position?
	const long tmp = (long) bits * ncomp * width;
	const long paddedBytesPerRow = (tmp + 7) / 8;
	const long pos = paddedBytesPerRow * 8 * y + (long) bits * (ncomp * x + numComponent);

	// which byte?
	long bytePos = pos / 8;

	// which bit in byte?
	int bitPos = pos % 8;

	// now fetch all bits for requested component
	unsigned char result = 0;
	const unsigned int numBits = (bits > 8) ? 8 : bits;	// get only most significant bits 
	for (unsigned int resultBit = 0; resultBit < numBits; resultBit++) {
		if (data[bytePos] & (1 << (7 - bitPos))) {
			result |= 1 << (numBits - resultBit - 1);
		}

		bitPos++;
		if (bitPos > 7) {
			bitPos = 0;
			bytePos++;
		}
	}

	assert(numBits>0);
	// scale result into full range [0-255]
	return (unsigned char) ((long) result * 255L / ((1L << numBits) - 1L));	//lint !e795 //  Conceivable division by 0
}


void PSImage::calculateBoundingBox()
{
//  if (isFileImage) { 
//      cerr << "Image::calculateBoundingBox not yet supported for PNG File Image objects" << endl;
//      return;
//  }
	// calculate image bounding box in device space
	const int border = 0;		// subtract 1 or 0;
	const Point P1(0.0f, 0.0f);
	const Point P1T = P1.transform(normalizedImageCurrentMatrix);
	const Point P2((float) (width - border), (float) (height - border));
	const Point P2T = P2.transform(normalizedImageCurrentMatrix);
	const Point P3(0.0f, (float) (height - border));
	const Point P3T = P3.transform(normalizedImageCurrentMatrix);
	const Point P4((float) (width - border), 0.0f);
	const Point P4T = P4.transform(normalizedImageCurrentMatrix);

	ur = Point( std::max(std::max(P1T.x(), P2T.x()), std::max(P3T.x(), P4T.x())),
	            std::max(std::max(P1T.y(), P2T.y()), std::max(P3T.y(), P4T.y())));
	ll = Point( std::min(std::min(P1T.x(), P2T.x()), std::min(P3T.x(), P4T.x())),
	            std::min(std::min(P1T.y(), P2T.y()), std::min(P3T.y(), P4T.y())));
}


void PSImage::writeEPSImage(ostream & outi) const
{
   if (isFileImage) {
#if 1
		static bool first = true;
		if (first) {
			cerr << "Seems like some meant to be dead code for PNG to EPS image file conversion is still needed. "
				    "Please notify author of pstoedit." << endl;
			first = false;
		}
		assert(false);
		return;

#else
// this code looks to be obsolete now. Only driver using png for internal image transfer
// was drvasy. Changed that to memoryeps - so "isFileImage" should be false then
// when writeEPSImage is being called from drvasy.
// wogl: 180513
#ifdef HAVE_LIBGD
		FILE* in = fopen(FileName.c_str(),"rb");
		if (!in) { 
			cerr << "Error opening file " <<FileName.c_str() << endl;
			return; 
		}
//		cerr << "Reading: "<<FileName.c_str() << endl;
		gdImagePtr im=gdImageCreateFromPng(in);
		assert(im);
		fclose(in);
		
		const int w=gdImageSX(im);
		const int h=gdImageSY(im);
		
		outi << "%!PS-Adobe-2.0 EPSF-2.0\n";
		outi << "%%Title: image created by pstoedit\n";
		outi << "%%Creator: pstoedit version " << PACKAGE_VERSION << endl;
		outi << "%%BoundingBox: " 
		     << floor(ll.x_) << " " << floor(ll.y_) << " "
		     << ceil(ur.x_) << " " << ceil(ur.y_) << endl;

		outi << "%%Pages: 1\n";
		outi << "%%EndComments\n";

		outi << "%%Page: 1 1\n";
		
		outi << "gsave\n";

		outi << "1 dict begin % temp dict for storing str1\n";
		outi << "% transformation matrix" << endl;
		outi << "[ ";
		{
			for (unsigned int i = 0; i < 6; i++)
				outi << normalizedImageCurrentMatrix[i] << " ";
		}
		outi << "] concat" << endl;
		outi << "[ 1 0 0 -1 0 " << h << " ] concat" << endl;
		outi << "/str1 1 string def\n";
			
		outi << "0 " << h << " translate\n";
		outi<< w << " " << -h << " scale\n";
		outi<< w << " "<< h << " 8" << endl;
		outi<< "[ " << w << " 0 0 " << h << " 0 0 ]\n";
		
		outi << "{currentfile str1 readhexstring pop} % decoding procedure" << endl;
		outi << "false 3 % has many sources, number of color components" << endl;
		outi << "% number of hex bytes " << w*h*3 << endl;
		outi << "colorimage" << endl;
		
		int x,y,c,i=0;
		for(y=0; y < h; y++)
			for(x=0; x < w; x++)
			{
				if (i % (12 * ncomp) == 0) outi << endl;
				c=gdImageGetPixel(im,x,y);
				outi << setw(2) << setfill('0') << hex << (int)gdImageRed(im,c);
				i++;
				outi << setw(2) << setfill('0') << hex << (int)gdImageGreen(im,c);
				i++;
				outi << setw(2) << setfill('0') << hex << (int)gdImageBlue(im,c);
				i++;
			}
		outi << setfill(' ') << dec; // reset
		outi << endl << endl;
		outi << "% restore previous state" << endl;
		outi << "end " << endl;
		outi << "grestore" << endl << endl;
		outi << "%%Trailer" << endl;
		outi << "%%EOF" << endl;
		outi << dec;
		
		gdImageDestroy(im);
		return;
#else
		static bool first=true;
		if(first) {
			cerr << "Reconfigure with libgd installed to support PNG to EPS image file conversion" << endl;
			first=false;
		}
		return;
#endif
#endif
   } else {
	assert(data);
	// output the image data along with decoding procedure
	// into a separate *.eps file
	outi << "%!PS-Adobe-2.0 EPSF-2.0" << endl;
	outi << "%%Title: image created by pstoedit" << endl;
	outi << "%%Creator: pstoedit version " << PACKAGE_VERSION << endl;
	outi << "%%BoundingBox: " << floor(ll.x()) << " " << floor(ll.y()) << " "
	     << ceil(ur.x()) << " " << ceil(ur.y()) << endl;
	outi << "%%Pages: 1" << endl;
	outi << "%%EndComments" << endl << endl;
	outi << "%%Page: 1 1" << endl << endl;
	outi << "% save current state" << endl;
	outi << "gsave" << endl << endl;
	outi << "1 dict begin % temp dict for storing str1" << endl;
	outi << "% transformation matrix" << endl;
	outi << "[ ";
	{
		for (unsigned int i = 0; i < 6; i++)
			outi << normalizedImageCurrentMatrix[i] << " ";
	}
	outi << "] concat" << endl << endl;

	switch (type) {
	case ImageType::colorimage:
		outi << "/str1 1 string def" << endl << endl;
		outi << "% display color image" << endl;
		outi << width << " " << height << " " << bits << " % width, height, bits/component" << endl;
		outi << "[ 1 0 0 1 0 0 ] %image matrix" << endl;
		outi << "{currentfile str1 readhexstring pop} % decoding procedure" << endl;
		outi << "false " << ncomp << " % has many sources, number of color components" << endl;
		outi << "% number of data " << nextfreedataitem << endl;
		outi << "colorimage" << endl;
		{
			for (unsigned int i = 0; i < nextfreedataitem; i++) {
				if (i % (12 * ncomp) == 0)
					outi << endl;	// debug " " << dec << i << endl;
				outi << setw(2) << setfill('0') << hex << (int) data[i];
			}
			outi << setfill(' ') << dec; // reset
		}
		break;
	case ImageType::imagemask:
		// just treat it as a normal image for the moment
	case ImageType::normalimage:
		outi << "/str1 1 string def" << endl << endl;
		outi << "% display normal image" << endl;
		outi << width << " " << height << " " << bits << " % width, height, bits/component" << endl;
		outi << "% number of data " << nextfreedataitem << endl;
		outi << "[ 1 0 0 1 0 0 ] %image matrix" << endl;
		outi << "{currentfile str1 readhexstring pop} % decoding procedure" << endl;
		outi << "image" << endl;
		{
			for (unsigned int i = 0; i < nextfreedataitem; i++) {
				if (i % (12 * 3) == 0)
					outi << endl;
				outi << setw(2) << setfill('0') << hex << (unsigned int)
					data[i];
			}
			outi << setfill(' ') << dec; // reset
		}
		break;
	default:
		break;
	}

	outi << endl << endl;
	outi << "% restore previous state" << endl;
	outi << "end " << endl;
	outi << "grestore" << endl << endl;
	outi << "%%Trailer" << endl;
	outi << "%%EOF" << endl;
	outi << dec;
   }
}

void PSImage::writeIdrawImage(ostream & outi, float scalefactor) const
{
	if (isFileImage) {
		cerr << "Image::writeIdrawImage not yet supported for PNG File Image objects" << endl;
		return;
	}
	assert(data);
	/* scale raster transformer by a ps-to-idraw factor */
	/* translate the raster transformer to zero out x,y center of the raster */
	/* then flip images vertically by multiplying transformer */
	/* by a vertical inverting affine transform of 1,0,0,-1,0,0 */
	/* then add the original offset back in */
	float scaledMatrix[6] = {};
	float finalMatrix[6]  = {};
	{
		for (unsigned int i = 0; i < 6; i++) {
			scaledMatrix[i] = normalizedImageCurrentMatrix[i] * scalefactor;
			finalMatrix[i] = scaledMatrix[i];	// initial value
		}
	}

	/* compute the scaled, transformed center and subtract it out. */
	int mat00 = 0, mat01 = 1, mat10 = 2, mat11 = 3, mat20 = 4, mat21 = 5;
	const float centerx = width / 2.0f;
	const float centery = height / 2.0f;
	const float ncx = centerx * scaledMatrix[mat00] + centery * scaledMatrix[mat10] + scaledMatrix[mat20];
	const float ncy = centerx * scaledMatrix[mat01] + centery * scaledMatrix[mat11] + scaledMatrix[mat21];
	finalMatrix[4] -= ncx;
	finalMatrix[5] -= ncy;

	const float verticalInvertingMatrix[6] = { 1, 0, 0, -1, 0, 0 };
	float tmp =
		finalMatrix[mat00] * verticalInvertingMatrix[mat01] +
		finalMatrix[mat01] * verticalInvertingMatrix[mat11];
	finalMatrix[mat00] =
		finalMatrix[mat00] * verticalInvertingMatrix[mat00] +
		finalMatrix[mat01] * verticalInvertingMatrix[mat10];
	finalMatrix[mat01] = tmp;

	tmp =
		finalMatrix[mat10] * verticalInvertingMatrix[mat01] +
		finalMatrix[mat11] * verticalInvertingMatrix[mat11];
	finalMatrix[mat10] =
		finalMatrix[mat10] * verticalInvertingMatrix[mat00] +
		finalMatrix[mat11] * verticalInvertingMatrix[mat10];
	finalMatrix[mat11] = tmp;

	tmp =
		finalMatrix[mat20] * verticalInvertingMatrix[mat01] +
		finalMatrix[mat21] * verticalInvertingMatrix[mat11];
	finalMatrix[mat20] =
		finalMatrix[mat20] * verticalInvertingMatrix[mat00] +
		finalMatrix[mat21] * verticalInvertingMatrix[mat10];
	finalMatrix[mat21] = tmp;

	finalMatrix[mat20] += verticalInvertingMatrix[mat20] + ncx;
	finalMatrix[mat21] += verticalInvertingMatrix[mat21] + ncy;

	// output the image data along with decoding procedure
	// into a separate *.eps file
	outi << "Begin %I Rast" << endl;
	outi << "%I t" << endl;
	outi << "[ ";
	outi << finalMatrix[0] << " ";
	outi << finalMatrix[1] << " ";
	outi << finalMatrix[2] << " ";
	outi << finalMatrix[3] << " ";
	outi << finalMatrix[4] << " ";
	outi << finalMatrix[5] << " ";
	outi << "] concat" << endl;
	outi << "%I" << endl;
	const int perline = width;

	outi << width << " " << height << " " << "8" <<
		" Rast { currentfile " << perline << " string readhexstring pop }" << endl;
	outi << "image";

	/* unpack 4 bit rgb data, skipping last nybble on odd-width images */
	unsigned char *dataptr = data;
	unsigned char *udata = data;
	if (bits == 4) {
		udata = new unsigned char[nextfreedataitem * 2];
		assert(udata);
		dataptr = udata;

		unsigned int curnybble = 0;	/* 0=r,1=g,2=b */
		unsigned int curcol = 0;
		//    unsigned int currow = 0;
		unsigned int udata_loc = 0;

		for (unsigned int i = 0; i < nextfreedataitem; i++) {

			/* replicate most-significant nybble first */
			udata[udata_loc++] = (data[i] & 0xf0) | ((data[i] & 0xf0) >> 4);

			curnybble++;
			if (curnybble == 3) {
				curnybble = 0;
				curcol++;
			}

			/* replicate least-significant nybble if not at end of odd-width row */
			if (curcol != width || !(width % 2)) {
				udata[udata_loc++] = (data[i] & 0x0f) | ((data[i] & 0x0f) << 4);

				curnybble++;
				if (curnybble == 3) {
					curnybble = 0;
					curcol++;
				}
			}

			/* skip nybble at end of odd-width row */
			if (curcol == width) {
				//     currow++;
				curcol = 0;
			}

		}
	}

	/* output graylevel image for B&W printers */
	int cur = 0;
	{
		for (unsigned int row = 0; row < height; row++) {
			outi << endl << "%I ";
			for (unsigned int col = 0; col < width; col++) {
				unsigned int grayval;
				if (type == ImageType::colorimage) {
					grayval = (unsigned int)
						(.299 * dataptr[cur]
						 + .587 * dataptr[cur+1]
						 + .114 * dataptr[cur+2]);
						 cur+=3;
				} else
					grayval = dataptr[cur++];
				outi << setw(2) << setfill('0') << hex << grayval << dec;
			}
		}
	}

	/* output color image for use of idraw */
	outi << endl << "%I colorimage";
	cur = 0;
	{
		for (unsigned int row = 0; row < height; row++) {
			outi << endl << "%I ";
			for (unsigned int col = 0; col < width; col++) {
				if (type == ImageType::colorimage) {
					outi << setw(2) << setfill('0') << hex << (unsigned int) dataptr[cur++] << dec;
					outi << setw(2) << setfill('0') << hex << (unsigned int) dataptr[cur++] << dec;
					outi << setw(2) << setfill('0') << hex << (unsigned int) dataptr[cur++] << dec;

				} else {
					outi << setw(2) << setfill('0') << hex << (unsigned int) dataptr[cur] << dec;
					outi << setw(2) << setfill('0') << hex << (unsigned int) dataptr[cur] << dec;
					outi << setw(2) << setfill('0') << hex << (unsigned int) dataptr[cur++] << dec;
				}
			}
			outi << setfill(' ') << dec; // reset
		}
	}

	if (udata != data) {
		assert(bits == 4);
		delete[] udata;
	}
	outi << endl << "End " << endl << endl;


}
