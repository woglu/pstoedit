/* 
   drvlwo.cpp - Driver to output LightWave 3D Objects (LWO)
             - written by Glenn M. Lewis <glenn_AT_gmlewis.com> - 6/18/96
	       http://www.gmlewis.com/>
	       Based on... 

   drvSAMPL.cpp : This file is part of pstoedit
   Skeleton for the implementation of new backends

   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

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
#include "drvlwo.h"
#include I_fstream
#include I_stdio
#include I_stdlib

struct LWO_POLY {
public:
	LWO_POLY(): next(nullptr), r(0), g(0), b(0), num(0L), x(nullptr), y(nullptr) {} 
	~LWO_POLY() {
		delete[]x;
		x = nullptr;
		delete[]y;
		y = nullptr;
		next=nullptr;
	}
	LWO_POLY *next;
	unsigned char r, g, b;
	unsigned long num;			// Number of vertices in poly
	float *x;
	float *y;

	NOCOPYANDASSIGN(LWO_POLY)
};

drvLWO::derivedConstructor(drvLWO):
constructBase
{
	// driver specific initializations
	// and writing of header to output file
	total_vertices = 0;
	polys = nullptr;
	total_polys = 0;

	//    float           scale;
	//    float           x_offset;
	//    float           y_offset;
}

drvLWO::~drvLWO()
{
	
	


	// driver specific deallocations
	// and writing of trailer to output file
	outf << "FORM";

	unsigned long total_bytes  = 12L;			// LWOBPNTS+size
	total_bytes += (12L * total_vertices);	// PNTS section...
	total_bytes += 8L;			// POLS+size
	total_bytes += (4L * total_polys + 2L * total_vertices);	// POLS section...
	out_ulong(outf, total_bytes);	// Total file size (-8)
	outf << "LWOBPNTS";
	out_ulong(outf, 12L * total_vertices);
	// Output vertices...
	if (total_vertices > 65536) {
		errf << "ERROR!  Generated more than 65536 vertices!!!  Abort.";
		return;
	}
	LWO_POLY *p ;
	for (p = polys; p; p = p->next) {
		for (unsigned long n = 0; n < p->num; n++) {
			out_float(outf, p->x[n]);
			out_float(outf, p->y[n]);
			out_float(outf, 0.0);
		}
	}

	// Now, output polygons...
	outf << "POLS";
	out_ulong(outf, 4L * total_polys + 2L * total_vertices);
	unsigned long count  = 0;
	for (p = polys; p; p = p->next) {
		out_ushort(outf, p->num);
		for (unsigned long n = 0; n < p->num; n++)
			out_ushort(outf, count + n);
		count += p->num;
		out_ushort(outf, 0);	// which surface
	}
// cleanup memory
	LWO_POLY *pnext;
	for (p = polys; p; p = pnext) {
		pnext = p->next;
		delete p;
	}
	polys=nullptr;
	options=nullptr;
}

void drvLWO::print_coords()
{
	auto p = new LWO_POLY;
	p->r = (unsigned char) (255.0 * currentR());
	p->g = (unsigned char) (255.0 * currentG());
	p->b = (unsigned char) (255.0 * currentB());
	p->num = 0;					// initial value;
//  p->num = numberOfElementsInPath();
	p->x = new float[numberOfElementsInPath()];	// allocate a conservative amount
	p->y = new float[numberOfElementsInPath()];	// allocate a conservative amount
	p->next = polys;
	polys = p;
	total_polys++;

	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:{
				const Point & pe = elem.getPoint(0);
				// outf << "\t\tmoveto ";
				p->x[p->num] = pe.x() + x_offset;
				p->y[p->num] = pe.y() + y_offset;
				p->num++;
			}
			break;
		case lineto:{
				const Point & pe = elem.getPoint(0);
				// outf << "\t\tlineto ";
				p->x[p->num] = pe.x() + x_offset;
				p->y[p->num] = pe.y() + y_offset;
				p->num++;
			}
			break;
		case closepath:		// Not supported
			// outf << "\t\tclosepath ";
			break;
		case curveto:{			// Not supported
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvpdf " << endl;
			abort();
			break;
		}
		//    outf << endl;
	}
	total_vertices += p->num;
	// outf << "]" << endl;
}


void drvLWO::open_page()
{
	//  outf << "Opening page: " << currentPageNumber << endl;
}

void drvLWO::close_page()
{
	//  outf << "Closing page: " << (currentPageNumber) << endl;
}

void drvLWO::show_path()
{
	print_coords();
}


static DriverDescriptionT < drvLWO > D_lwo("lwo", "LightWave 3D object format", "","lwo", false,	// if backend supports subpaths, else 0
										   // if subpaths are supported, the backend must deal with
										   // sequences of the following form
										   // moveto (start of subpath)
										   // lineto (a line segment)
										   // lineto 
										   // moveto (start of a new subpath)
										   // lineto (a line segment)
										   // lineto 
										   //
										   // If this argument is set to 0 each subpath is drawn 
										   // individually which might not necessarily represent
										   // the original drawing.
										   false,	// if backend supports curves, else 0
										   false,	// if backend supports elements with fill and edges
										   false,	// backend supports text
										   DriverDescription::imageformat::noimage,	// no support for PNG file images
										   DriverDescription::opentype::binaryopen, false,	// backend supports multiple pages
										   false  /*clipping */ 
										   );
