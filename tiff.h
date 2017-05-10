
#include <string>
#include <CGAL/Image_3.h>

// Read a multi-page tiff file (containing voxel data)
// Input: tiff filename
// Output: CGAL::Image_3
//
void read_tif(const std::string filename, CGAL::Image_3& image);
