# pstoedit

pstoedit - translates PostScript graphics into other vector formats.
Emphasis is on **vector** formats. There are many other tools based on pure raster image formats but conversion to vector formats retaining high level vector information is more complex and not many tools are available for this type of conversion.

The architecture of pstoedit consists of a PostScript frontend which
needs to call a PostScript interpreter like Ghostscript and the
individual backends which are plugged into a kind of framework.

This framework can be used independently from the PostScript frontend
from any other program. The framework provides a uniform interface to
all different backends. Get in contact with the author if you need
more information on how to use this framework.

pstoedit works reasonable with PostScript files containing
* line drawings
* text with standard fonts

Try to run it on golfer.ps or tiger.ps that comes with Ghostscript, e.g.,
pstoedit -f <your format> <local path where GhostScript is installed>/examples/tiger.ps tiger.<suffix>

Since version 4.0, there is also a GUI for pstoedit - PstoeditQtGui

Some features that are not supported by every backend of pstoedit:
* bitmap images (just for some backends and only a subset of the PostScript image operators)
* general fill patterns
* clipping (only partially via the -sclip option)


## Dependencies
### Build time Dependencies

The configure script checks for the availability of several third party components and configures the build process accordingly. The following components are being checked:

* pdflatex for the generation of the manual - Mandatory
* libplot and libplotter for using the additional backends provided by those libs (apt name libplot-dev) - Optional. When avaailable adds a bunch of more backend options via the libplotter.
* QT for GUI - Optional
* libzip for ppt backend (apt name libzip-dev) - Optional. When available the PPTX (PowerPoint) backend is enabled.
* libemf for enhanced windows metafiles (apt name libemf-dev) - Optional. Needed for the EMF backend.
* libmagic for Magic drivers - Optional. Provides access to drivers provided by the libmagic.

see also the [recommended apt packages](doc/pstoedit.linux.prerequisites.md)


### Run time Dependencies

The most important one is GhostScript. You need a working installation of this PostScript interpreter.

## Installation
### Installation in Linux

To build pstoedit from source code run
```bash
  sh autogen.sh
  ./configure
  make; make install
```
from the root directory of the project.
The first command will first update all the autotools related files needed by the subsequent configure and make.
pstoedit still uses the classical autotools instead of Cmake and the like.

### Cleanup after build

After a full build you can use the following steps to remove all the files which were generated during the build.
```bash
  make clean
  sh rm_generated.sh
```

### Installation in Windows

```sh
  winget install pstoedit
```

So far a binary is provided only for Windows, i.e no make system despite the availability of the source code. This is because the provided binary is based on several third party libraries and building them upfront is sometimes quite complex.
## License

[GNU GPLv2](https://choosealicense.com/licenses/gpl-2.0/)

## Test

You can run some examples provided in the examples directory.

## Extending pstoedit

To implement a new backend you can start from drvsampl.cpp.
Please don't forget to send any new backend that might be of interest
for others as well to the author (wglunz35_AT_pstoedit.net) so that
it can be incorporated into future versions of pstoedit. Such
new backends will then be available with the GPL as well.

## Acknowledgements

See manual page in [pstoedit.htm](http://www.pstoedit.com/pstoedit.htm) and the [changelog.htm](http://www.pstoedit.com/changelog.htm) for a list of contributors.


## Web site

See here for more information - [pstoedit.com](https://pstoedit.com/).

## Support

For support, email wglunz35_AT_pstoedit.net.

If you want to support the development of pstoedit any further or just find the program very useful consider a small donation.
