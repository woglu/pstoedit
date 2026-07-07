/* Copyright (C) 1998, Russell Lang.  All rights reserved.
   Copyright (C) 2005, Wolfgang Glunz. All rights reserved. 
   Wolfgang Glunz implemented the C++ wrapper and the adaptation to the new iapi.h
  
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

#if defined (_WIN32)
#include <windows.h>
// the next define is needed in gsdll.h
#define _Windows

#endif

#include I_stdio
#include I_string_h
#ifdef OS_WIN32_WCE
#include "WinCEAdapter.h"
#else
#include <fcntl.h>
#endif

#include <io.h>
#include "iapi.h"

typedef int (GSDLLCALLPTR gs_write_callback_funcptr) (void * cb_data, const char* text, int length); // length needs to be int because of gs api

static int 
GSDLLCALL default_gs_addmess(void * /* cb_data */ , const char* text, int length)
{
	// this default is just used in test environment. In real version, the output handler
	// is set via pstoedit.c
	// from pstoedit 3.50 on we redirect all stdout of ghostscript to stderr in order to avoid problems
	// in case pstoedit's output is written to stdout (e.g. this is done if being called from inkscape)
	if (text) {
		fwrite(text,1,length, stderr);
		fflush(stderr);
		return length;
	} else {
		return 0; 
	}
}

static gs_write_callback_funcptr current_write_callback = default_gs_addmess;
static void* global_cb_data = nullptr;

void set_gs_write_callback(void* cbData, gs_write_callback_funcptr new_cb) {
	//  cout << "CB set " << endl;
	global_cb_data = cbData;
	current_write_callback = new_cb;
}


static char messagebuffer[1000]; // just for local formatting

static void writemessage(const char *msg = messagebuffer) {
	(void)current_write_callback(global_cb_data,msg,(int) strlen(msg));
}
static int GSDLLCALL std_inHandler(void * /*caller_handle*/, char *  buf , int  len ) // len needs to be int because of gs api
{ 
	// return 0; 
	const size_t result = fread(buf,1,len,stdin);
	return (int) result;
}

/* main handler for the GS DLL */
class GSDLL {
public:
	GSDLL()	:
	 hmodule(nullptr),	
	 new_instance(0),
	 delete_instance(0),
	 set_stdio(0),
	 init_with_args(0),
	 run_string(0),
	 run_file(0),
	 exit(0),
	 minst(0) {
	 }

	~GSDLL() {
	 gs_load_dll_cleanup();
	 new_instance=(0);
	 delete_instance=(0);
	 set_stdio=(0);
	 init_with_args=(0);
	 run_string=(0);
	 run_file=(0);
	 exit=(0);
	 minst=(0) ;
	 hmodule=0;
	 }

	void gs_load_dll_cleanup(void)
	{
	  	if (hmodule) (void) FreeLibrary(hmodule);
		hmodule=(0);	
	}



/* load GS DLL if not already loaded */
/* return TRUE if OK */
bool
gs_load_dll(const char * szDllName)
{

	// long version;
#ifdef OS_WIN32_WCE
	hmodule = LoadLibrary(LPSTRtoLPWSTR(szDllName).c_str());
#else
	hmodule = LoadLibrary(szDllName);
#endif
    if (hmodule < (HINSTANCE)HINSTANCE_ERROR) {
		/* fprintf(stderr,"loading  %s failed\n", szDllName); */
		return false;
	}

	// fprintf(stderr, "loaded %s\n", szDllName);

    /* DLL is now loaded */
    /* Get pointers to functions */

//TODO    if ( (gsapi.revision = (PFN_gsdll_revision) GetProcAddress(gsapi.hmodule, "gsdll_revision")) == NULL)
//TODO	return gs_load_dll_cleanup();
	
#ifdef GS_REVISION
    /* check DLL version  [OPTIONAL] */
    revision(NULL, NULL, &version, NULL);
    if (version != (long)GS_REVISION) {
	sprinft(messagebuffer,"Wrong version of DLL found.\n  Found version %ld\n\
  Need version  %ld\n", version, (long)GS_REVISION);
	write_message();
	return gs_load_dll_cleanup();
    }
#endif

    /* continue loading other functions */
    if ( (new_instance = (PFN_gsapi_new_instance) 	GetProcAddress(hmodule, TEXT("gsapi_new_instance"))) == NULL)
		{ writemessage("could not load gsapi_new_instance\n");  gs_load_dll_cleanup(); return false; }
    if ( (delete_instance = (PFN_gsapi_delete_instance) 	GetProcAddress(hmodule, TEXT("gsapi_delete_instance"))) == NULL)
		{ writemessage("could not load gsapi_delete_instance\n");  gs_load_dll_cleanup();  return false;}
    if ( (set_stdio = (PFN_gsapi_set_stdio) 	GetProcAddress(hmodule, TEXT("gsapi_set_stdio"))) == NULL)
		{ writemessage("could not load gsapi_set_stdio\n");  gs_load_dll_cleanup();  return false;}
    if ( (init_with_args = (PFN_gsapi_init_with_args) 	GetProcAddress(hmodule, TEXT("gsapi_init_with_args"))) == NULL)
		{ writemessage("could not load gsapi_init_with_args\n");  gs_load_dll_cleanup(); return false;}
    if ( (run_string = (PFN_gsapi_run_string) 	GetProcAddress(hmodule, TEXT("gsapi_run_string"))) == NULL)
		{ writemessage("could not load gsapi_run_string\n");  gs_load_dll_cleanup(); return false;}
    if ( (run_file = (PFN_gsapi_run_file) 	GetProcAddress(hmodule, TEXT("gsapi_run_file"))) == NULL)
		{ writemessage("could not load gsapi_run_file\n");  gs_load_dll_cleanup(); return false;}
 	if ( (exit = (PFN_gsapi_exit) 	GetProcAddress(hmodule, TEXT("gsapi_exit"))) == NULL)
		{ writemessage("could not load gsapi_exit\n");  gs_load_dll_cleanup(); return false;}
 
    return true;
}


private:
	HINSTANCE	hmodule;	/* handle to module */

public: 
	// yes - I make these public - Of course one could add real methods without the 
	// minst argument which just forward the call to the pointer adding the minst argument
	// but that would mean a lot of code duplication for little added value (W. Glunz)

	/* pointers to DLL functions */
	PFN_gsapi_new_instance new_instance;
	PFN_gsapi_delete_instance delete_instance;
	PFN_gsapi_set_stdio set_stdio;
	PFN_gsapi_init_with_args init_with_args;
	PFN_gsapi_run_string run_string;
	PFN_gsapi_run_file run_file;
	PFN_gsapi_exit exit;

	// opaque pointer to the GhostScript instance (but there can only be one anyway)
	void* minst;

	NOCOPYANDASSIGN(GSDLL)
};


// can be static - since it is #included into callgs.cpp
static int
callgsDLL(int argc, char *argv[], void * cb_data)
{
	global_cb_data = cb_data; // for writemessage
	GSDLL gsapi;
#ifndef OS_WIN32_WCE
	/*
	This change is done to avoid a console popup appearing in WindowsCE
	1. We get the console popup only in WindowsCE, and not in Windows 32/64
	2. CreateProcess with CREATE_NO_WINDOW returns error=87(i.e "Invalid parameter") in WindowsCE, because CREATE_NO_WINDOW is not supported in WindowsCE
	3. Also tried with CreateProcess() with STARTUPINFOW::wShowWindow=SW_HIDE, this does not hide the console window.
	4. In Windows CE, when there is FILENO(stdin), it opens a console. This works fine in Windows x86/x64.
	   Tested by commenting this code, all functionality seems to be working fine.
	*/

    (void) SETMODE(FILENO(stdin), O_BINARY);
#endif

	// const char szDllName[] = "gsdll32.dll";
	const char * const szDllName = argv[0]; // 

    if (!gsapi.gs_load_dll(szDllName)) {
		sprintf_s(TARGETWITHLEN(messagebuffer,1000), "Can't load %s (possibly due to 32/64 bit mix - will try gswin32c.exe instead\n", szDllName);
		writemessage();
		return -1;
    }
	const int e_Quit = -101;  // see ierrors.h
	
	int code = gsapi.new_instance(&(gsapi.minst), cb_data);
	if (code < 0) {
		writemessage("new_instance failed\n");
		return 1;
	}

	(void)gsapi.set_stdio(gsapi.minst,std_inHandler,current_write_callback,current_write_callback); // params: in, out, err

    code = gsapi.init_with_args(gsapi.minst, argc, argv);

	int exit_code;
	const char start_string[] = "systemdict /start get exec\n";
	/* 
	cerr << "args to GS DLL: " << argc << endl;
	for (int i = 0; i < argc; i++) {
		cerr << i << ": '" << argv[i] << "'" << endl;
	}
    */
	if (code == 0) 	code = gsapi.run_string(gsapi.minst, start_string, 0, &exit_code);

    const int code1 = gsapi.exit(gsapi.minst);
    
	if ((code == 0) || (code == e_Quit)) code = code1;

    gsapi.delete_instance(gsapi.minst);

	if ((code == 0) || (code == e_Quit)) {
		// sprintf(messagebuffer,"return OK\n"); writemessage();
		return 0;
	} else {
		// sprintf(messagebuffer,"return FAIL %d\n",code);writemessage();
		return 1;
	}

}
