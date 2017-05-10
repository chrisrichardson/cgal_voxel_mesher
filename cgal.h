// voxel_mesher
//
// Copyright 2017 Chris Richardson
//
// This software is licenced under the Gnu LGPLv3 https://www.gnu.org/licenses/lgpl-3.0
//

#ifndef _VOXELMESHER_CGAL_H
#define _VOXELMESHER_CGAL_H

#include <CGAL/Image_3.h>
#include <string>

void cgal_make_mesh(const CGAL::Image_3& image,
                    std::string filename,
                    int cell_size);

#endif
