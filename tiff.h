// voxel_mesher
//
// Copyright 2017 Chris Richardson
//
// This software is licenced under the Gnu LGPLv3 https://www.gnu.org/licenses/lgpl-3.0

#ifndef _VOXELMESHER_TIFF_H
#define _VOXELMESHER_TIFF_H

#include <string>
#include <CGAL/Image_3.h>

// Read a multi-page tiff file (containing voxel data)
// Input: tiff filename
// Output: CGAL::Image_3
//
void read_tif(const std::string filename, CGAL::Image_3& image);

#endif
