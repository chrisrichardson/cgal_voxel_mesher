#ifndef _VOXELMESHER_RAW_H
#define _VOXELMESHER_RAW_H

#include <CGAL/Image_3.h>
#include <string>

// Read 'raw' data file (e.g. the walnut from voreen) which have a '.dat' file
// with them to describe the contents
// Input: dat filename
// Output: CGAL::Image_3
//
void read_raw(const std::string filename, CGAL::Image_3& image);

#endif
