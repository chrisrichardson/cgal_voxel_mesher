
#include "cgal.h"
#include "raw.h"

#define CGAL_MESH_3_VERBOSE
#include <CGAL/Image_3.h>

#include <boost/algorithm/string.hpp>
#include <cassert>
#include <fstream>
#include <string>

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  std::string filename;
  if (argc == 2)
    filename = argv[1];
  else
    filename = "input.dat";

  CGAL::Image_3 image;

  // FIXME: check suffix and switch between readers
  read_raw(filename, image);
  // load_tif(filename, image);

  int nx = image.xdim();
  int ny = image.ydim();
  int nz = image.zdim();

  // Threshold image
  unsigned char *p = (unsigned char *)(image.data());
  for (std::size_t k = 0; k != nz; ++k)
    for (std::size_t j = 0; j != ny; ++j)
      for (std::size_t i = 0; i != nx; ++i)
      {
        if (*p == 2)
          *p = 255;
        else
          *p = 0;
        ++p;
      }

  cgal_make_mesh(image, "out.mesh", 1);

  return 0;
}
