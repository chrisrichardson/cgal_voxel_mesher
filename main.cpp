// voxel_mesher
//
// Copyright 2017 Chris Richardson
//
// This software is licenced under the Gnu LGPLv3 https://www.gnu.org/licenses/lgpl-3.0
//
// Usage:
//
// # Read a raw file (see voreen for example) and use a key value 3 (i.e. voxels with value 3 are solid)
// voxel_mesher -k 3 -i file.dat
//
// # Read a multipage TIFF file as a voxel image, thresholding solid values above 128
// voxel_mesher -t 128 -i file.tif
//
// Output is in the "medit" format, as this is what CGAL produces. It can be converted to other
// formats using "meshio" or another similar tool.
//
//

#include "cgal.h"
#include "raw.h"
#include "tiff.h"

#define CGAL_MESH_3_VERBOSE
#include <CGAL/Image_3.h>

#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <cassert>
#include <fstream>
#include <string>

namespace po = boost::program_options;

//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "describe arguments")
    ("input,i", po::value<std::string>()->required(), "voxel data file (in .dat or .tif format)")
    ("output,o", po::value<std::string>(), "output mesh file (in .mesh format) - defaults to out.mesh")
    ("key,k", po::value<int>(), "Pixel value to use as a key (e.g. for segmented image)")
    ("threshold,t", po::value<int>(), "Pixel value to threshold from (i.e. values above are solid, use negative value to set values below as solid)")
    ("size,s", po::value<int>(), "Cell sizing, as defined by CGAL");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);

  if (vm.count("help") or argc == 1)
  {
    std::cout << desc << "\n";
    return 1;
  }
  po::notify(vm);

  if (vm.count("key") and vm.count("threshold"))
  {
    throw std::runtime_error("Cannot set key and threshold together");
  }

  int key = -1;
  if (vm.count("key"))
  {
    key = vm["key"].as<int>();
    std::cout << "key = " << key << "\n";
  }

  int threshold = 0;
  if (vm.count("threshold"))
  {
    threshold = vm["threshold"].as<int>();
    std::cout << "threshold = " << threshold << "\n";
  }

  int cell_size = 4;
  if (vm.count("size"))
  {
    cell_size = vm["size"].as<int>();
    std::cout << "cell_size = " << cell_size << "\n";
  }

  std::string out_filename = "out.mesh";
  if (vm.count("output"))
  {
    out_filename = vm["output"].as<std::string>();
  }
  std::cout << "output filename = " << out_filename << "\n";

  // Required input filename
  std::string filename = vm["input"].as<std::string>();
  std::cout << "input filename = " << filename << "\n";

  CGAL::Image_3 image;

  // FIXME: check suffix and switch between readers
  std::vector<std::string> f_line;
  boost::split(f_line, filename, boost::is_any_of("."));
  std::string format = f_line.back();

  if (format == "dat")
    read_raw(filename, image);
  else if (format == "tif")
    read_tif(filename, image);
  else
    throw std::runtime_error("format not supported");

  int nx = image.xdim();
  int ny = image.ydim();
  int nz = image.zdim();

  // Threshold or key image

  unsigned char *p = (unsigned char *)(image.data());
  if (key >= 0)
  {
    std::cout << "Selecting based on key value: " << key << "\n";
    for (std::size_t i = 0; i != nx*ny*nz; ++i)
    {
      if (*p == key)
        *p = 255;
      else
        *p = 0;
      ++p;
    }
  }

  if (threshold != 0)
  {
    std::cout << "Selecting based on threshold value: " << threshold << "\n";
    for (std::size_t i = 0; i != nx*ny*nz; ++i)
    {
      if (threshold > 0)
        *p = (*p > threshold) ? 255 : 0;
      else
        *p = (*p > -threshold) ? 0 : 255;
      ++p;
    }
  }

  cgal_make_mesh(image, out_filename, cell_size);

  return 0;
}
