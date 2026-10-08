/*
   drvplot.cpp : This file is part of pstoedit 
   Backend for GNU metafile format and all other output formats produced
   by the GNU libplot/libplotter vector graphics library

   (GNU libplot/libplotter is the 2D vector graphics C/C++ library in the
   GNU Plotting Utilities (`plotutils') package.  `libplot' and
   `libplotter' provide C and C++ bindings, respectively.  See
   http://www.gnu.org/software/plotutils/plotutils.html .  The GNU `plot'
   program, which is part of the package, can translate GNU metafiles to
   other formats.)

   When compiling, define "-DHAVE_LIBPLOTTER" if you will be linking with
   libplotter.  Otherwise, this driver will produce its own GNU metafiles,
   and it will not be able to produce any of the other output formats that
   libplot/libplotter can produce.

   This driver supports three formats: "gmfa" (ascii [i.e., portable] GNU
   metafile), "gmfb" (binary GNU metafile), and "plot".  "plot" is
   supported only if you link with libplotter.  By default, the "plot"
   format is the same as "gmfb", but by using the "type" and the 
   (global)"pagesize" options, you may request any of the other output types that
   libplot/libplotter can produce.  For example,

   pstoedit -f "plot:-plotformat X"

   will pop up an X window, and 

   pstoedit -f "plot:-plotformat fig"

   will produce Fig output.  The supported output formats are:

   "X", "png", "pnm", "gif", "svg", "ai", "ps", "cgm", "fig",
   "pcl", "hpgl", "regis", "tek", or "meta" (the default).

   Here "pnm" means PPM/PGM/PBM, "cgm" means the new WebCGM vector graphics
   format [see http://www.cgmopen.org ], "ai" means Adobe Illustrator,
   "pcl" means PCL5, "hpgl" means HP-GL/2, and "tek" means Tektronix 4014
   format.  Tektronix 4014 format can be displayed on an xterm window that
   is in Tektronix mode; `xterm -t' will invoke such an xterm.

   The "ps" output type is not ordinary Postscript: it can also be edited
   with the idraw drawing editor.  "meta" is the default output type;
   it means a binary metafile, i.e., the same as the "gmfb" format.

   The "plot" format supports the global  "pagesize" option, which tells the driver
   the size of the PS page that it is rendering.  (The default is "letter",
   i.e., 8.5in by 11in.)  For example,

    pstoedit -f "plot:-plotformat X" -pagesize letter
    pstoedit -f "plot:-plotformat ps" -pagesize legal > output.ps
    pstoedit -f "plot:-plotformat fig" -pagesize a4 > output.fig
   
   The supported page sizes are:

	   a, b, c, d, e, a4, a3, a2, a1, a0, b5, legal, ledger

   For most output types, the "pagesize" option affects not only the
   interpretation of the input PS file, but also the sort of output file
   that is produced.  In the example immediately above, the output.fig file
   will be created on an A4-sized output page.  If you view output.fig with
   the xfig drawing editor, you will see this.

   No matter what output format is chosen, you may move the image on the
   output page by doing the following.  (1) Instead of using pstoedit's
   command-line `pagesize' option, specify the page size by setting the
   environment variable PAGESIZE.  (2) Include the desired viewport offset
   in PAGESIZE.  For example, you could set PAGESIZE to "a4,yoffset=1cm" or
   "a3,xoffset=-5mm,yoffset=2cm".

   Also in the "plot" format, you may specify any of libplot/libplotter's
   parameters on the command line.  For example,

    pstoedit -f "plot:-plotformat X BITMAPSIZE 400x200 BG_COLOR yellow" -pagesize a4 

   will pop up an X window of size 400x200, with a yellow background.
   BITMAPSIZE is also supported if the output type is "pnm" or "gif", and
   BG_COLOR if the output type is "pnm", "gif", or "cgm".

   For the "pnm" output type (i.e. PPM/PGM/PBM, whichever is
   most appropriate), setting the PNM_PORTABLE parameter to "yes" will
   yield an ascii (human-readable) output file.  Similarly, "plot:type meta
   META_PORTABLE yes", or just "plot:META_PORTABLE yes", will yield
   human-readable "gmfa" format.

   Most of the other supported libplot/libplotter parameters are not used
   very often.  You may specify "CGM_MAX_VERSION 1" to obtain a version 1
   WebCGM file, rather than version 3 (which is the default).  You may also
   specify "CGM_ENCODING clear_text" to obtain a human-readable CGM file,
   rather than binary (which is the default).

   You should specify "PCL_BEZIERS no" if you are preparing a PCL5 file to
   be printed on an old LaserJet III.  Unlike more recent LaserJets, the
   LaserJet III did not support the drawing of Bezier curves in firmware.

   As implemented, the PCL5 output type is mostly for black-and-white or
   grayscale drawings.  Colored PCL5 drawings do not print well on most
   monochrome printers that understand PCL5 format (e.g., monochrome
   LaserJets).  But if you have a color LaserJet or other high-end color
   printer that understands PCL5 format, you should be able to print
   colored PCL5 drawings, as well.  If you are producing colored PCL5
   output to send to such a printer, you may find it useful to specify
   "PCL_ASSIGN_COLORS yes".  That will give improved color fidelity, by
   allowing colors to be assigned internally to `logical pens', instead of
   producing colors by half-toning from a fixed palette of pens.

   The "gif" output type is actually pseudo-GIF.  It uses run-length
   encoding, and does not transgress the Unisys patent on LZW encoding.  If
   you are producing the "gif" output type, you can specify the parameter
   "TRANSPARENT_COLOR white" to make all white pixels transparent.
   "INTERLACE yes" will cause the output file to be an interlaced
   pseudo-GIF.

   If a specified libplot/libplotter parameter is not relevant to the
   requested output type, it is ignored.


   NOTE: If you link with libplotter, it is recommended that you use a
   version of libplotter which is at least as recent as the one that is
   part of the plotutils-2.3 package.  If you have the version that was
   part of plotutils-2.2, you may link with it, provided that you also
   specify "-DOLD_LIBPLOTTER" when compiling.  But that is not recommended,
   because not everything will not work properly.  (Output files of the
   "fig" and "pcl" types will not be properly centered on the page, and the
   "gmfb" format will be broken.)

   NOTE: If your version of libplotter has no X Window System support, you
   should specify "-DNO_LIBPLOTTER_X_SUPPORT" when compiling.

   Written by Robert S. Maier <rsm_AT_math.arizona.edu> and Wolfgang Glunz
   <wglunz35_AT_pstoedit.net>.

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

#include "drvlplot.h"

#include I_fstream
#include I_stdio
#include I_stdlib
#include <memory>

// #include "papersizes.h"  // not yet done - remove of local page size related code and replace it with the one from papersizes.h


#ifdef OLD_LIBPLOTTER
#define NO_LIBPLOTTER_CGM_SUPPORT
#endif

// new: define X missing for all systems - this make the whole lib much smaller and reduces the
// prerequesites
#define X_DISPLAY_MISSING
#define NO_LIBPLOTTER_X_SUPPORT

#if ! (defined(unix) || defined(__unix__) || defined(_unix) || defined(__unix)  || defined (NetBSD) ) && ( ! (defined(_AIX)) )
// just under Unix we have X11 support
// AIX is treated as Unix - except here
#define X_DISPLAY_MISSING
#define NO_LIBPLOTTER_X_SUPPORT
// also the following two plotters are not available in libplot outside Unix
// since the plotter needs some X11 functions as well
#define NO_LIBPLOTTER_GIF_SUPPORT
#define NO_LIBPLOTTER_PNM_SUPPORT
#endif

#ifdef HAVE_LIBPLOTTER
#ifdef OLD_LIBPLOTTER			// plotutils-2.2
#define Point PlotPoint

#define _INC_IOSTREAM
// to avoid a further inclusion (problem with STL)
#include <plotter.h>
#undef Point
#else	

#define _INC_IOSTREAM						// plotutils-2.3 and more recent
#include <plotter.h>
#endif
#endif

// create shortcuts to the libplotter drivers (see commmet at the bottom)
#define WITHSHORTCUTS

// Default page type.  This is the page for which, by default, we assume
// that the PS file was generated.  The user may override this with the
// pagesize option, e.g. by specifying
//
//     -f "gmfa" -pagesize b
// or
//     -f "plot" -pagesize a3
//
// on the command line.

#define DEFAULT_PAGE_TYPE PAGE_ANSI_A

// page types that libplot/libplotter recognizes
enum page_types { PAGE_ANSI_A, PAGE_ANSI_B, PAGE_ANSI_C, PAGE_ANSI_D, PAGE_ANSI_E,
	PAGE_ISO_A4, PAGE_ISO_A3, PAGE_ISO_A2, PAGE_ISO_A1, PAGE_ISO_A0,
	PAGE_JIS_B5, PAGE_LEGAL, PAGE_LEDGER
};

struct page_size {
	int type;					// one of the above
	const char *name;			// name (a string)
	const char *alias;			// an alternative name (a string)
	double width, height;		// width, height in inches
	double viewport_size;		// size of libplotter's square viewport, in inches
};

// libplot page size database; should be listed in the same order as the
// preceding page types.  The size of the libplot viewport, which is a
// square region on the page, is listed for each.  In plotutils-2.3, all
// Plotter types for which the PAGESIZE parameter is meaningful have the
// default viewport positioned at the center of the page, except one.
//
// HPGLPlotters are the exception.  They use the same page sizes and
// viewport sizes, but by default, the lower left corner of the viewport is
// positioned close to the lower left corner of the page, because an HP-GL
// device may have either portrait or landscape orientation (there is no
// programmatic way of selecting it).  You may shift the viewport by
// specifying a suitably modified PAGESIZE environment variable; see above.


static struct page_size known_page_sizes[] = {
	{PAGE_ANSI_A, "a", "letter",
	 8.5, 11.0, 8.0},
	{PAGE_ANSI_B, "b", "tabloid",
	 11.0, 17.0, 10.0},
	{PAGE_ANSI_C, "c", nullptr,
	 17.0, 22.0, 16.0},
	{PAGE_ANSI_D, "d", nullptr,
	 22.0, 34.0, 20.0},
	{PAGE_ANSI_E, "e", nullptr,
	 34.0, 44.0, 32.0},
	{PAGE_ISO_A4, "a4", nullptr,
	 8.27, 11.69, 7.8},
	{PAGE_ISO_A3, "a3", nullptr,
	 11.69, 16.54, 10.7},
	{PAGE_ISO_A2, "a2", nullptr,
	 16.54, 23.39, 15.6},
	{PAGE_ISO_A1, "a1", nullptr,
	 23.39, 33.11, 22.4},
	{PAGE_ISO_A0, "a0", nullptr,
	 33.11, 46.81, 32.2},
	{PAGE_JIS_B5, "b5", nullptr,
	 7.17, 10.12, 6.67},
	{PAGE_LEGAL, "legal", nullptr,
	 8.5, 14.0, 8.0},			// an unofficial US size
	{PAGE_LEDGER, "ledger", nullptr,
	 17.0, 11.0, 10.0}			// rotated ANSI B (an unofficial US size)
};

const unsigned short NUM_LIBPLOT_PAGE_SIZES = (sizeof(known_page_sizes) / sizeof(page_size)); 
// 13

#ifndef HAVE_LIBPLOTTER
// we aren't linking with libplotter, so declare a fake Plotter class
class Plotter {
private:
	// disallow copying and assignment
	NOCOPYANDASSIGN(Plotter)
	// streams (only the outstream and errstream are used)
	istream *instream;
	ostream *outstream;
	ostream *errstream;

	bool meta_portable_output;	// portable metafile output format?
	bool opened;

public:
	 Plotter(istream & in, ostream & out, ostream & err);	// ctor

	static int parampl(const char *parameter, void *value);
	static bool static_meta_portable_output;	// current class value

	// declare only the Plotter methods we may need
	int capmod(const char *s);
	int closepl(void);
	int endpath(void);
	int erase(void);
	int fbezier3(double x0, double y0, double x1, double y1, double x2,
				 double y2, double x3, double y3);
	int fbox(double x0, double y0, double x1, double y1);
	int fconcat(double m0, double m1, double m2, double m3, double m4, double m5);
	int fcont(double x, double y);
	int fillcolor(int red, int green, int blue);
	int fillmod(const char *s);
	int filltype(int level);
	int fline(double x0, double y0, double x1, double y1);
	int flinedash(int n, const double *dashes, double offset);
	int flinewidth(double new_line_width);
	int fmove(double x, double y);
	int fontname(const char *s);
	int fspace(double x0, double y0, double x1, double y1);
	int joinmod(const char *s);
	int label(const char *s);
	int linemod(const char *s);
	int openpl(void);
	int pencolor(int red, int green, int blue);
	int restorestate(void);
	int savestate(void);
	double ffontsize(double size);

private:
	// inline functions for low-level output
	inline void emit_byte(int c);
	inline void emit_float(double x);
	inline void emit_integer(int x);
	inline void emit_string(const char *s);
	inline void emit_terminator(void);

	// define op codes for graphics operations (only the ones we'll need)
	enum op_codes {
		O_BEZIER3 = 'y',
		O_BOX = 'B',
		O_CAPMOD = 'K',
		O_CLOSEPL = 'x',
		O_CONT = 'n',
		O_ENDPATH = 'E',
		O_ERASE = 'e',
		O_FBEZIER3 = ',',
		O_FBOX = '3',
		O_FCONCAT = '\\',
		O_FCONT = ')',
		O_FFONTSIZE = '7',
		O_FILLCOLOR = 'D',
		O_FILLMOD = 'g',
		O_FILLTYPE = 'L',
		O_FLINE = '8',
		O_FLINEDASH = 'w',
		O_FLINEWIDTH = '0',
		O_FMOVE = '$',
		O_FONTNAME = 'F',
		O_FONTSIZE = 'S',
		O_FSPACE = '*',
		O_JOINMOD = 'J',
		O_LABEL = 't',
		O_LINE = 'l',
		O_LINEDASH = 'd',
		O_LINEMOD = 'f',
		O_LINEWIDTH = 'W',
		O_MOVE = 'm',
		O_OPENPL = 'o',
		O_PENCOLOR = '-',
		O_RESTORESTATE = 'O',
		O_SAVESTATE = 'U',
		O_SPACE = 's'
	};
};

// if we aren't using libplotter, `MetaPlotter' is the same as `Plotter'
typedef Plotter MetaPlotter;

// define the fake Plotter (i.e. MetaPlotter) class

// a helper function (libplot only allows printable ISO characters in labels)
static inline bool GOOD_ISO(unsigned char c) {
	return ((((c) >= 0x20) && ((c) <= 0x7E)) || (((c) >= 0xA0) && ((c) <= 0xFF)));
}
static bool clean_iso_string(unsigned char *s)
{
	bool was_clean = true;
	unsigned char *t;

	for (t = s; *s; s++) {
		if (GOOD_ISO(*s)) {
			*t = *s;
			t++;
		} else
			was_clean = false;

	}
	*t = (unsigned char) '\0';

	return was_clean;
}

// ctor
Plotter::Plotter(istream & in, ostream & out, ostream & err)
{
	instream = &in;
	outstream = &out;
	errstream = &err;
	// take instance value from current static (class) value
	meta_portable_output = static_meta_portable_output;
	opened = false;
}

// a fake parampl function, which can set only the META_PORTABLE parameter
int
 Plotter::parampl(const char *parameter, void *value)
{
	if (strcmp(parameter, "META_PORTABLE") == 0) {
		if (strcmp((char *) value, "yes") == 0)
			static_meta_portable_output = true;
		else
			static_meta_portable_output = false;
	}
	return 0;
}

// a static variable in the fake Plotter class (automatically initialized
// to `false'); copied into a Plotter when it's instantiated
bool Plotter::static_meta_portable_output;

// low-level output routines

inline void Plotter::emit_integer(int x)
{
	if (meta_portable_output)
		(*outstream) << ' ' << x;
	else
		outstream->write((char *) &x, sizeof(int));
}

inline void Plotter::emit_float(double x)
{
	if (meta_portable_output)
		(*outstream) << ' ' << x;
	else {
		float f = (float) x;
		outstream->write((char *) &f, sizeof(float));
	}
}

inline void Plotter::emit_byte(int c)
{
	outstream->put((unsigned char) c);
}

inline void Plotter::emit_string(const char *s)
{
	bool has_newline;
	char *t = nullptr;				// keep compiler happy
	char *nl;
	const char *u;

	// null pointer handled specially
	if (s == 0)
		s = "(null)";

	if (strchr(s, '\n'))
		// we don't grok multiline arg strings
	{
		has_newline = true;
		t = (char *) malloc(strlen(s) + 1);
		if (!t) {
			(*errstream) << "ERROR: Can't allocate memory" << endl;
			return;
		}
		strcpy_s(t,strlen(s), s);
		nl = strchr(t, '\n');
		assert(nl);				// should be OK because also a newline was in s
		*nl = '\0';
		u = t;
	} else {
		has_newline = false;
		u = s;
	}

	(*outstream) << u << '\n';

	if (has_newline)
		free(t);
}

inline void Plotter::emit_terminator()
{
	if (meta_portable_output)
		(*outstream) << '\n';
}

// fake public methods
int Plotter::capmod(const char *s)
{
	emit_byte((int) O_CAPMOD);
	emit_string(s);
	return 0;
}

int Plotter::closepl()
{
	emit_byte((int) O_CLOSEPL);
	emit_terminator();
	return 0;
}

int Plotter::endpath()
{
	emit_byte((int) O_ENDPATH);
	emit_terminator();
	return 0;
}

int Plotter::erase()
{
	emit_byte((int) O_ERASE);
	emit_terminator();
	return 0;
}

int Plotter::fbezier3(double x0, double y0, double x1, double y1,
					  double x2, double y2, double x3, double y3)
{
	emit_byte(meta_portable_output ? (int) O_BEZIER3 : (int) O_FBEZIER3);
	emit_float(x0);
	emit_float(y0);
	emit_float(x1);
	emit_float(y1);
	emit_float(x2);
	emit_float(y2);
	emit_float(x3);
	emit_float(y3);
	emit_terminator();
	return 0;
}

int Plotter::fbox(double x0, double y0, double x1, double y1)
{
	emit_byte(meta_portable_output ? (int) O_BOX : (int) O_FBOX);
	emit_float(x0);
	emit_float(y0);
	emit_float(x1);
	emit_float(y1);
	emit_terminator();
	return 0;
}

int Plotter::fconcat(double m0, double m1, double m2, double m3, double m4, double m5)
{
	emit_byte((int) O_FCONCAT);
	emit_float(m0);
	emit_float(m1);
	emit_float(m2);
	emit_float(m3);
	emit_float(m4);
	emit_float(m5);
	emit_terminator();
	return 0;
}

int Plotter::fcont(double x, double y)
{
	emit_byte(meta_portable_output ? (int) O_CONT : (int) O_FCONT);
	emit_float(x);
	emit_float(y);
	emit_terminator();
	return 0;
}

double Plotter::ffontsize(double size)
{
	emit_byte(meta_portable_output ? (int) O_FONTSIZE : (int) O_FFONTSIZE);
	emit_float(size);
	emit_terminator();
	return size;
}

int Plotter::fillcolor(int red, int green, int blue)
{
	emit_byte((int) O_FILLCOLOR);
	emit_integer(red);
	emit_integer(green);
	emit_integer(blue);
	emit_terminator();
	return 0;
}

int Plotter::fillmod(const char *s)
{
	emit_byte((int) O_FILLMOD);
	emit_string(s);
	return 0;
}

int Plotter::filltype(int level)
{
	emit_byte((int) O_FILLTYPE);
	emit_integer(level);
	emit_terminator();
	return 0;
}

int Plotter::fline(double x0, double y0, double x1, double y1)
{
	emit_byte(meta_portable_output ? (int) O_LINE : (int) O_FLINE);
	emit_float(x0);
	emit_float(y0);
	emit_float(x1);
	emit_float(y1);
	emit_terminator();
	return 0;
}

int Plotter::flinedash(int n, const double *dashes, double offset)
{
	int i;

	// sanity checks
	if (n < 0 || (n > 0 && dashes == 0))
		return -1;
	for (i = 0; i < n; i++)
		if (dashes[i] < 0.0)
			return -1;

	emit_byte(meta_portable_output ? (int) O_LINEDASH : (int) O_LINEDASH);
	emit_integer(n);
	for (i = 0; i < n; i++)
		emit_float(dashes[i]);
	emit_float(offset);
	emit_terminator();
	return 0;
}

int Plotter::flinewidth(double new_line_width)
{
	emit_byte(meta_portable_output ? (int) O_LINEWIDTH : (int)
			  O_FLINEWIDTH);
	emit_float(new_line_width);
	emit_terminator();
	return 0;
}

int Plotter::fmove(double x, double y)
{
	emit_byte(meta_portable_output ? (int) O_MOVE : (int) O_FMOVE);
	emit_float(x);
	emit_float(y);
	emit_terminator();
	return 0;
}

int Plotter::fontname(const char *s)
{
	emit_byte((int) O_FONTNAME);
	emit_string(s);
	return 0;					// return 0, not current font size
}

int Plotter::fspace(double x0, double y0, double x1, double y1)
{
	emit_byte(meta_portable_output ? (int) O_SPACE : (int) O_FSPACE);
	emit_float(x0);
	emit_float(y0);
	emit_float(x1);
	emit_float(y1);
	emit_terminator();
	return 0;
}

int Plotter::joinmod(const char *s)
{
	emit_byte((int) O_JOINMOD);
	emit_string(s);
	return 0;
}

int Plotter::label(const char *s)
{
	char *t;
	bool was_clean;

	// copy since we may alter the string
	t = (char *) malloc(strlen(s) + 1);
	if (!t) {
		(*errstream) << "ERROR: Can't allocate memory" << endl;
		return 0;
	}
	strcpy_s(t,strlen(s), s);
	was_clean = clean_iso_string((unsigned char *) t);
	if (!was_clean)
		(*errstream) << "libplot: ignoring control character (e.g. CR or LF) in label" << endl;

	emit_byte((int) O_LABEL);
	emit_string(t);
	free(t);

	return 0;
}

int Plotter::linemod(const char *s)
{
	emit_byte((int) O_LINEMOD);
	emit_string(s);
	return 0;
}

int Plotter::openpl()
{
	bool not_previously_opened = !opened;	

	if (not_previously_opened)
		// emit metafile header
	{
		(*outstream) << "#PLOT";	// magic string

		// format type 1 = GNU binary, type 2 = GNU portable
		if (meta_portable_output)
			(*outstream) << " 2\n";
		else
			(*outstream) << " 1\n";
	}

	emit_byte((int) O_OPENPL);
	emit_terminator();

	opened = true;
	return 0;
}

int Plotter::pencolor(int red, int green, int blue)
{
	emit_byte((int) O_PENCOLOR);
	emit_integer(red);
	emit_integer(green);
	emit_integer(blue);
	emit_terminator();
	return 0;
}

int Plotter::restorestate()
{
	emit_byte((int) O_RESTORESTATE);
	emit_terminator();
	return 0;
}

int Plotter::savestate()
{
	emit_byte((int) O_SAVESTATE);
	emit_terminator();
	return 0;
}

#endif							// not HAVE_LIBPLOTTER

#ifdef HAVE_LIBPLOTTER
#ifdef OLD_LIBPLOTTER
// define a sink for libplotter warning messages, because in plotutils-2.2,
// libplotter had an unfortunate tendency to write error messages to the
// output stream rather than the error stream!
static int dummy_warning_handler(const char *)
{
	return 0;
}
#endif
#endif



// driver specific initializations
drvplot::derivedConstructor(drvplot):constructBase
{
 	bool portable_metafile = false;  // binary is not portable - ascii is

	if (strcmp(driverdesc.symbolicname, "gmfa") == 0) {
		const char t[] = "yes";
		(void)Plotter::parampl("META_PORTABLE", const_cast<char *>(t));
		portable_metafile = true;	// won't need to reopen outf in binary mode
	}

	// defaults (will try to determine page type from pagesize option)
	page_type = (int) DEFAULT_PAGE_TYPE;
	physical_page = false;
	// calculate pagesize based on global -pagesize option. 
	{
		const RSString & pagesize = getPageSize();
		if (pagesize != "") {
			bool matched = false;
			unsigned int j;

			for (j = 0; j < NUM_LIBPLOT_PAGE_SIZES; j++)
				if (strcmp(pagesize.c_str(), known_page_sizes[j].name) == 0
					||
					(known_page_sizes[j].alias
					 && strcmp(pagesize.c_str(), known_page_sizes[j].alias) == 0)) {
					matched = true;
					break;
				}
			if (matched)
				// set PAGESIZE parameter (it will be meaningful to the
				// Plotter only if its output format is one for which
				// pages are `physical')
			{
				page_type = j;
				const char *s = known_page_sizes[page_type].name;
				const size_t len = strlen(s) ;
				auto t = new char[len + 1];
				if (!t) {
					errf << "ERROR: Can't allocate memory " << endl;
					ctorOK = false;
					return;
				}
				strcpy_s(t,len+1, s);
				(void)Plotter::parampl("PAGESIZE", (void *) t);
				delete [] t; 
			} else
				errf << "unknown page size" << endl;
		}
	}
#if 0

		if (strcmp(d_argv[i], "page-size") == 0) {
#ifdef OLDPAGESIZE
			bla bla bla;
			if ((i + 1) < d_argc) {
				bool matched = false;
				unsigned int j;

				for (j = 0; j < NUM_LIBPLOT_PAGE_SIZES; j++)
					if (strcmp(d_argv[i + 1], known_page_sizes[j].name) ==
						0 || (known_page_sizes[j].alias
							  && strcmp(d_argv[i + 1], known_page_sizes[j].alias) == 0)) {
						matched = true;
						break;
					}
				if (matched)
					// set PAGESIZE parameter (it will be meaningful to the
					// Plotter only if its output format is one for which
					// pages are `physical')
				{
					page_type = j;
					const char *s = known_page_sizes[page_type].name;
					char *t = (char *) malloc(strlen(s) + 1);
					if (!t) {
						errf << "ERROR: Can't allocate memory " << endl;
						ctorOK = false;
						return;
					}
					strcpy(t, s);
					Plotter::parampl("PAGESIZE", (void *) t);
					free(t);
				} else
					errf << "unknown page size" << endl;
				i++;
				i++;
			} else {
				errf << "too few arguments for page-size option" << endl;
				i++;
			}
#else
			errf <<
				"Please use global -pagesize option instead driver specific -page-size option"
				<< endl;
#endif
		}

	for (unsigned int i = 0; i < d_argc;) {
		assert(d_argv && d_argv[i]);

		if (strcmp(driverdesc.symbolicname, "plot") == 0) {
			if ((i + 1) < d_argc) {
				if (strcmp(d_argv[i], "type") == 0)
					type = d_argv[i + 1];
				else {
					if (strcmp(d_argv[i], "META_PORTABLE") == 0
						&& strcmp(d_argv[i + 1], "yes") == 0)
						portable_metafile = true;
					// set Plotter parameter
					Plotter::parampl(d_argv[i], d_argv[i + 1]);
				}
				i++;
				i++;
			} else {
				errf << "too few parameters for libplot option" << endl;
				i++;
			}
		}

		else {
			errf << "unknown option " << d_argv[i] << endl;
			i++;
		}
	}

#endif

	// parse remaining args 
	if (d_argc) {
		const unsigned int remaining = DOptions_ptr->parseoptions(errf,d_argc,d_argv);
		if (Verbose()) { errf << "remaining options " << remaining << endl; }
		const char * * remaining_argv = DOptions_ptr->unhandledOptions.data();
		for (unsigned int i = 0; (i+1) < remaining; i+=2) {  // -1 because we also use argv[i+1]
			if (strcmp(remaining_argv[i], "META_PORTABLE") == 0
						&& strcmp(remaining_argv[i + 1], "yes") == 0)
						portable_metafile = true;
					// set Plotter parameter
			if (Verbose()) {
				errf << "adding Plotter parameter " << remaining_argv[i] << ":" << remaining_argv[i + 1] << endl;
			}
			(void)Plotter::parampl(remaining_argv[i], const_cast<char*>(remaining_argv[i + 1]));
		}
	}

#ifdef HAVE_LIBPLOTTER
#ifdef OLD_LIBPLOTTER
	// turn off all warning messages, to work around the plotutils-2.2
	// libplotter warning message bug (see above)
	libplotter_warning_handler = dummy_warning_handler;
#endif
#endif

	// now create the Plotter; if we're not linked with libplotter then we
	// instantiate the fake Plotter class instead

#ifdef HAVE_LIBPLOTTER
	if ((strcmp(driverdesc.symbolicname, "plot") == 0)
#ifdef WITHSHORTCUTS
		|| (strncmp(driverdesc.symbolicname, "plot-", 5) == 0)
#endif
		) {
#ifdef WITHSHORTCUTS
		if (strncmp(driverdesc.symbolicname, "plot-", 5) == 0) {
			options->type.value = driverdesc.symbolicname + 5;
			// cout << "type : " << type << endl;
		}
#endif
		if (false) {
		}
#ifndef NO_LIBPLOTTER_PNM_SUPPORT
		else if (options->type.value == "pnm") {
			plotter = new PNMPlotter(cin, outf, errf);
		}
#endif
#ifndef NO_LIBPLOTTER_GIF_SUPPORT
		else if (options->type.value == "gif") {
			ctorOK &= close_output_file_and_reopen_in_binary_mode();
			plotter = new GIFPlotter(cin, outf, errf);
		}
#endif
#ifndef NO_LIBPLOTTER_CGM_SUPPORT
		else if (options->type.value == "cgm") {
			ctorOK &= close_output_file_and_reopen_in_binary_mode();
			plotter = new CGMPlotter(cin, outf, errf);
			physical_page = false;
		}
#endif
		else if (options->type.value == "ai") {
			plotter = new AIPlotter(cin, outf, errf);
			physical_page = true;
#if defined(PL_LIBPLOT_VER) &&  (PL_LIBPLOT_VER > 400)
		} else if (options->type.value == "svg") {
			plotter = new SVGPlotter(cin, outf, errf);
			physical_page = true;
#endif
		} else if (options->type.value == "ps") {
			plotter = new PSPlotter(cin, outf, errf);
			physical_page = true;
		} else if (options->type.value == "fig") {
			plotter = new FigPlotter(cin, outf, errf);
			physical_page = true;
		} else if (options->type.value == "pcl") {
			ctorOK &= close_output_file_and_reopen_in_binary_mode();
			plotter = new PCLPlotter(cin, outf, errf);
			physical_page = true;
		} else if (options->type.value == "hpgl") {
			ctorOK &= close_output_file_and_reopen_in_binary_mode();
			plotter = new HPGLPlotter(cin, outf, errf);
			physical_page = false;	// unknown viewport position on page
		} else if (options->type.value == "tek") {
			ctorOK &= close_output_file_and_reopen_in_binary_mode();
			plotter = new TekPlotter(cin, outf, errf);
		}
#ifndef NO_LIBPLOTTER_X_SUPPORT
		else if (options->type.value == "X") {
			plotter = new XPlotter(cin, outf, errf);
		}
#endif
		else if (options->type.value == "meta") {
			if (!portable_metafile)
				ctorOK &= close_output_file_and_reopen_in_binary_mode();
			plotter = new MetaPlotter(cin, outf, errf);
		} else					// unknown type; use metafile default
		{
			errf << "unknown libplot output type, using GNU metafile" << endl;
			if (!portable_metafile)
				ctorOK &= close_output_file_and_reopen_in_binary_mode();
			plotter = new MetaPlotter(cin, outf, errf);
		}
	} else
#endif							// HAVE_LIBPLOTTER

		// Default case: instantiate the MetaPlotter class.  If we're not
		// linked with libplotter (so that "gmfa" and "gmfb" are our only
		// formats) then this is always done, and the MetaPlotter class that is
		// instantiated is really our fake Plotter class.
	{
		if (!portable_metafile)	//lint !e731
			ctorOK &= close_output_file_and_reopen_in_binary_mode();
		plotter = new MetaPlotter(cin, outf, errf);
	}
}

drvplot::~drvplot()
{
	// delete Plotter (either a genuine one from libplotter, or a fake one)
	delete plotter;
	options=nullptr;
}



// print a path via a sequence of fline(), fcont(), fbezier3() operations,
// terminating with a final endpath()
void drvplot::print_coords()
{
	Point lastpoint(0, 0);
	const Point & firstpoint = pathElement(0).getPoint(0);
	bool currently_at_lastpoint = false;
	bool last_was_endpath = false;

	// since libplot/libplotter doesn't (yet) support sub-paths,
	// all paths that we draw will be of the form
	// moveto {lineto,curveto}+  {closepath}
	// where {}+ means one or more repetitions, and {} means optional.

	for (unsigned int n = 0; n < numberOfElementsInPath(); n++) {
		const basedrawingelement & elem = pathElement(n);
		switch (elem.getType()) {
		case moveto:
			{
				const Point & p = elem.getPoint(0);
				lastpoint = p;
				currently_at_lastpoint = false;
				last_was_endpath = false;
			}
			break;
		case lineto:
			{
				const Point & p = elem.getPoint(0);
				if (currently_at_lastpoint)
					(void)plotter->fcont(p.x() + x_offset, p.y() + y_offset);
				else
					(void)plotter->fline(lastpoint.x() + x_offset,
								   lastpoint.y() + y_offset, p.x() + x_offset, p.y() + y_offset);
				lastpoint = p;
				currently_at_lastpoint = true;
				last_was_endpath = false;
			}
			break;
		case curveto:
			{
				const Point & p1 = lastpoint;
				const Point & p2 = elem.getPoint(0);
				const Point & p3 = elem.getPoint(1);
				const Point & p4 = elem.getPoint(2);

				(void)plotter->fbezier3(p1.x() + x_offset, p1.y() + y_offset,
								  p2.x() + x_offset, p2.y() + y_offset,
								  p3.x() + x_offset, p3.y() + y_offset,
								  p4.x() + x_offset, p4.y() + y_offset);
				lastpoint = p4;
				currently_at_lastpoint = true;
				last_was_endpath = false;
			}
			break;
		case closepath:
			if (currently_at_lastpoint)
				/* have drawn at least one segment */
			{
				(void)plotter->fcont(firstpoint.x() + x_offset, firstpoint.y() + y_offset);
				(void)plotter->endpath();
				currently_at_lastpoint = true;
				last_was_endpath = true;
			}
			break;
		default:
			errf << "\t\tFatal: unexpected case in drvlplot " << endl;
			abort();
			break;
		}
	}
	if (!last_was_endpath)
		(void)plotter->endpath();
}

void drvplot::open_page()
{
	const double POINTS_PER_INCH = 72.0;
	// height and width of a page of the specified type, in points; also size
	// of the square viewport that libplot places on such a page
	const double height = POINTS_PER_INCH * (double) known_page_sizes[page_type].height;
	const double width = POINTS_PER_INCH * (double) known_page_sizes[page_type].width;

	(void)plotter->openpl();
	if (physical_page)
		// output viewport is a square region of known size, centered on an
		// output page of known size; so scale the input PS page so that it
		// extends beyond the viewport, in such a way that it covers the entire
		// output page
	{
		const double viewport_size = POINTS_PER_INCH * (double) known_page_sizes[page_type].viewport_size;
		(void)plotter->fspace(0.5 * (width - viewport_size),
						0.5 * (height - viewport_size),
						0.5 * (width + viewport_size), 0.5 * (height + viewport_size));
	} else
		// output viewport is non-physical, of an unknown size, or in an
		// unknown location; so scale the input PS page so that its largest
		// dimension fits neatly within the viewport
	{
		if (height > width)
			(void)plotter->fspace(-0.5 * (height - width), 0.0, 0.5 * (height + width), height);
		else
			(void)plotter->fspace(0.0, -0.5 * (width - height), width, 0.5 * (width + height));
	}

	// erase the output device (some Plotters need this because they have
	// `persistent' graphics)
	(void)plotter->erase();
}

void drvplot::close_page()
{
	(void)plotter->closepl();
}

// convert to libplot's 16-bit representation for R, G, B intensities
static constexpr int plotcolor(const float f)
{
	return (int) (65535 * f);
}

void drvplot::show_text(const TextInfo & textinfo)
{
	if (strlen(textinfo.thetext.c_str()) > 0) {
		(void)plotter->ffontsize(textinfo.currentFontSize);
		(void)plotter->fontname(textinfo.currentFontName.c_str());
		(void)plotter->pencolor(plotcolor(textinfo.currentR),
						  plotcolor(textinfo.currentG), plotcolor(textinfo.currentB));
		const float *matrix = getCurrentFontMatrix();
		double sinv;
		if (textinfo.currentFontSize != 0.0)
			sinv = 1.0 / textinfo.currentFontSize;
		else
			sinv = 0.0;
		(void)plotter->savestate();
		(void)plotter->fconcat(sinv * (double) matrix[0],
						 sinv * (double) matrix[1],
						 sinv * (double) matrix[2],
						 sinv * (double) matrix[3], textinfo.x() + x_offset, textinfo.y() + y_offset);
		(void)plotter->fmove(0.0, 0.0);
		(void)plotter->label(textinfo.thetext.c_str());
		(void)plotter->restorestate();
	}
}

// set libplot's cap style, join style, line style
void drvplot::set_line_style()
{
	// set cap style and join style
	(void)plotter->capmod(currentLineCap() == 0 ? "butt" :
					currentLineCap() == 1 ? "round" :
					currentLineCap() == 2 ? "projecting" : "butt");
	(void)plotter->joinmod(currentLineJoin() == 0 ? "miter" :
					 currentLineJoin() == 1 ? "round" : currentLineJoin() == 2 ? "bevel" : "miter");

	// set old-fashioned line style
	const char *linestyle;
	switch (currentLineType()) {
	case solid:
	default:
		linestyle = "solid";
		break;
	case dashed:
		linestyle = "longdashed";
		break;
	case dashdot:
		linestyle = "dotdashed";
		break;
	case dotted:
		linestyle = "dotted";
		break;
	case dashdotdot:
		linestyle = "dotdotdashed";
		break;
	}
	(void)plotter->linemod(linestyle);

	// set dashing pattern, which most types of Plotter understand
	DashPattern dash_pattern(dashPattern());
	std::unique_ptr<double[]> numbers ( new double[dash_pattern.nrOfEntries]);
	for (int i = 0; i < dash_pattern.nrOfEntries; i++)
		numbers[i] = (double) dash_pattern.numbers[i];
	(void)plotter->flinedash(dash_pattern.nrOfEntries, numbers.get(), (double) dash_pattern.offset);
}

// Set libplot's filling-and-edging style.  May set the line width to zero
// to turn off edging.  In libplot, a 0-width line is as narrow a line as
// can be drawn.
void drvplot::set_filling_and_edging_style()
{
	switch (currentShowType()) {
	case drvbase::stroke:
		(void)plotter->flinewidth(currentLineWidth());
		(void)plotter->pencolor(plotcolor(currentR()), plotcolor(currentG()), plotcolor(currentB()));
		(void)plotter->filltype(0);	// no filling
		break;

	case drvbase::fill:
		if (pathWasMerged()) {
			(void)plotter->flinewidth(currentLineWidth());
			(void)plotter->pencolor(plotcolor(edgeR()), plotcolor(edgeG()), plotcolor(edgeB()));
			(void)plotter->fillcolor(plotcolor(fillR()), plotcolor(fillG()), plotcolor(fillB()));
		} else {
			(void)plotter->flinewidth(0.0);	// little or no edging
			(void)plotter->pencolor(plotcolor(currentR()), plotcolor(currentG()), plotcolor(currentB()));
			(void)plotter->fillcolor(plotcolor(currentR()), plotcolor(currentG()), plotcolor(currentB()));
		}
		(void)plotter->filltype(1);
		(void)plotter->fillmod("winding");
		break;

	case drvbase::eofill:
		if (pathWasMerged()) {
			(void)plotter->flinewidth(currentLineWidth());
			(void)plotter->pencolor(plotcolor(edgeR()), plotcolor(edgeG()), plotcolor(edgeB()));
			(void)plotter->fillcolor(plotcolor(fillR()), plotcolor(fillG()), plotcolor(fillB()));
		} else {
			(void)plotter->flinewidth(0.0);	// little or no edging
			(void)plotter->pencolor(plotcolor(currentR()), plotcolor(currentG()), plotcolor(currentB()));
			(void)plotter->fillcolor(plotcolor(currentR()), plotcolor(currentG()), plotcolor(currentB()));
		}
		(void)plotter->filltype(1);
		(void)plotter->fillmod("even-odd");
		break;

	default:
		// cannot happen
		errf << "unexpected ShowType " << (int) currentShowType();
		break;
	}
}

void drvplot::show_path()
{
	set_line_style();
	set_filling_and_edging_style();
	print_coords();
}

void drvplot::show_rectangle(const float llx, const float lly, const float urx, const float ury)
{
	set_line_style();
	set_filling_and_edging_style();
	(void)plotter->fbox(llx, lly, urx, ury);
}

void drvplot::show_image(const PSImage& /*imageinfo*/)
{
	// not implemented
}

static DriverDescriptionT < drvplot > D_plot_meta_a("gmfa", "ASCII GNU metafile", "", "meta", false,	// backend does not support subpaths
													true,	// backend supports curves
													true,	// backend supports filled elements with edges 
													true,	// backend supports text
													DriverDescription::imageformat::noimage,
													DriverDescription::opentype::normalopen, true,	// format supports multiple pages in one file
													false  /*clipping */
													);

static DriverDescriptionT < drvplot > D_plot_meta_b("gmfb", "binary GNU metafile", "", "meta", false,	// backend does not support subpaths
													true,	// backend supports curves
													true,	// backend supports filled elements with edges 
													true,	// backend supports text
													DriverDescription::imageformat::noimage,
													DriverDescription::opentype::binaryopen, true,	// format supports multiple pages in one file
													false  /*clipping */
													);

#ifdef HAVE_LIBPLOTTER
static DriverDescriptionT < drvplot > D_plot("plot", "GNU libplot output types, e.g. plot:-plotformat X", "", "plot", false,	// backend does not support subpaths
											 true,	// backend supports curves
											 true,	// backend supports filled elements with edges 
											 true,	// backend supports text
											 DriverDescription::imageformat::noimage,
											 DriverDescription::opentype::normalopen,	// may close, reopen as binary
											 true,	// format supports multiple pages in one file
											 false  /*clipping */
											 );

#ifdef WITHSHORTCUTS
// create shortcuts to the libplot drivers. So instead of -f "plot:type xxx" 
// one can say -f plot-xxx.

#ifndef NO_LIBPLOTTER_PNM_SUPPORT
static DriverDescriptionT < drvplot > D_plotpnm("plot-pnm", "pnm via GNU libplot", "", "pnm", false,	// backend does not support subpaths
												true,	// backend supports curves
												true,	// backend supports filled elements with edges 
												true,	// backend supports text
												DriverDescription::imageformat::noimage,
												DriverDescription::opentype::normalopen,	// may close, reopen as binary
												true,	// format supports multiple pages in one file
												false /*clipping */ 
												);
#endif
//#ifndef NO_LIBPLOTTER_GIF_SUPPORT
//      else if (strcmp (type, "gif") == 0)

//#endif
#ifndef NO_LIBPLOTTER_CGM_SUPPORT
static DriverDescriptionT < drvplot > D_plotcgm("plot-cgm", "cgm via GNU libplot", "", "cgm", false,	// backend does not support subpaths
												true,	// backend supports curves
												true,	// backend supports filled elements with edges 
												true,	// backend supports text
												DriverDescription::imageformat::noimage,
												DriverDescription::opentype::normalopen,	// may close, reopen as binary
												true,	// format supports multiple pages in one file
												false  /*clipping */
												);
#endif

static DriverDescriptionT < drvplot > D_plotai("plot-ai", "ai via GNU libplot","",  "ai", false,	// backend does not support subpaths
											   true,	// backend supports curves
											   true,	// backend supports filled elements with edges 
											   true,	// backend supports text
											   DriverDescription::imageformat::noimage,
											   DriverDescription::opentype::normalopen,	// may close, reopen as binary
											   true,	// format supports multiple pages in one file
											   false   /*clipping */
											   );

#if defined(PL_LIBPLOT_VER) &&  (PL_LIBPLOT_VER > 400)
static DriverDescriptionT < drvplot > D_plotsvg("plot-svg", "svg via GNU libplot", "", "svg", false,	// backend does not support subpaths
												true,	// backend supports curves
												true,	// backend supports filled elements with edges 
												true,	// backend supports text
												DriverDescription::imageformat::noimage,
												DriverDescription::opentype::normalopen,	// may close, reopen as binary
												true,	// format supports multiple pages in one file
												false  /*clipping */
												);
#endif

static DriverDescriptionT < drvplot > D_plotps("plot-ps", "ps via GNU libplot", "", "ps", false,	// backend does not support subpaths
											   true,	// backend supports curves
											   true,	// backend supports filled elements with edges 
											   true,	// backend supports text
											   DriverDescription::imageformat::noimage,
											   DriverDescription::opentype::normalopen,	// may close, reopen as binary
											   true,	// format supports multiple pages in one file
											   false /*clipping */ 
											   );

static DriverDescriptionT < drvplot > D_plotfig("plot-fig", "fig via GNU libplot", "", "fig", false,	// backend does not support subpaths
												true,	// backend supports curves
												true,	// backend supports filled elements with edges 
												true,	// backend supports text
												DriverDescription::imageformat::noimage,
												DriverDescription::opentype::normalopen,	// may close, reopen as binary
												true,	// format supports multiple pages in one file
												false  /*clipping */
												);

static DriverDescriptionT < drvplot > D_plotpcl("plot-pcl", "pcl via GNU libplot", "", "pcl", false,	// backend does not support subpaths
												true,	// backend supports curves
												true,	// backend supports filled elements with edges 
												true,	// backend supports text
												DriverDescription::imageformat::noimage,
												DriverDescription::opentype::normalopen,	// may close, reopen as binary
												true,	// format supports multiple pages in one file
												false  /*clipping */
												);

static DriverDescriptionT < drvplot > D_plothpgl("plot-hpgl", "hpgl via GNU libplot", "", "hpgl", false,	// backend does not support subpaths
												 true,	// backend supports curves
												 true,	// backend supports filled elements with edges 
												 true,	// backend supports text
												 DriverDescription::imageformat::noimage,
												 DriverDescription::opentype::normalopen,	// may close, reopen as binary
												 true,	// format supports multiple pages in one file
												 false  /*clipping */ 
												 );

static DriverDescriptionT < drvplot > D_plottek("plot-tek", "tek via GNU libplot","",  "tek", false,	// backend does not support subpaths
												true,	// backend supports curves
												true,	// backend supports filled elements with edges 
												true,	// backend supports text
												DriverDescription::imageformat::noimage,
												DriverDescription::opentype::normalopen,	// may close, reopen as binary
												true,	// format supports multiple pages in one file
												false /*clipping */ 
												);

#ifndef NO_LIBPLOTTER_X_SUPPORT
static DriverDescriptionT < drvplot > D_plotX("plot-X", "X via GNU libplot", "", "X", false,	// backend does not support subpaths
											  true,	// backend supports curves
											  true,	// backend supports filled elements with edges 
											  true,	// backend supports text
											  DriverDescription::imageformat::noimage,
											  DriverDescription::opentype::normalopen,	// may close, reopen as binary
											  true,	// format supports multiple pages in one file
											  false /*clipping */
											  );
#endif

#endif							// WITHSHORTCUTS

#endif							// HAVE_LIBPLOTTER
 
 
