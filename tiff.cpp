// voxel_mesher
//
// Copyright 2017 Chris Richardson
//
// This software is licenced under the Gnu LGPLv3 https://www.gnu.org/licenses/lgpl-3.0

#include "tiff.h"

#include <CGAL/Image_3.h>

#include <boost/algorithm/string.hpp>
#include <cassert>
#include <tiffio.h>
#include <fstream>
#include <string>

//-----------------------------------------------------------------------------
void read_tif(const std::string filename, CGAL::Image_3& image)
{

  TIFF* tif = TIFFOpen(filename.c_str(), "r");
  int ndir = 1;
  uint32 w = -1, h = -1;

  if (tif)
    {
      bool tif_ok = true;
      while (tif_ok)
      {
	if (tif_ok = TIFFReadDirectory(tif))
	{
	  ++ndir;
	  uint32 wtmp, htmp;
	  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &wtmp);
	  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &htmp);

	  if (w==-1 and h==-1)
	  {
	    w = wtmp;
	    h = htmp;
	  }
	  else
	  {
	    assert(w == wtmp);
	    assert(h == htmp);
	  }
	}
      }
      TIFFClose(tif);
    }

  int nx = w, ny = h, nz = ndir;
  double vx = 1.0, vy = 1.0, vz = 1.0;

  std::cout << "Got tiff: " << nx << "x" << ny << "x" << nz << std::endl;

  image = _createImage(nx, ny, nz, 1,
                       vx, vy, vz, 1,
                       WK_FIXED, SGN_UNSIGNED);

  unsigned char *ptr = (unsigned char *)(image.data());
  const std::size_t npixels = w*h;
  tif = TIFFOpen(filename.c_str(), "r");
  assert(tif);
  for (int i = 0 ; i < ndir; ++i)
  {
    uint32* raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
    if (raster != NULL)
    {
      if (TIFFReadRGBAImage(tif, w, h, raster, 0))
      {
	std::copy(raster, raster + npixels, ptr);
	ptr += npixels;
      }
      _TIFFfree(raster);
    }
    TIFFReadDirectory(tif);
  }
}
