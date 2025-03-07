// voxel_mesher
//
// Copyright 2017 Chris Richardson
//
// This software is licenced under the Gnu LGPLv3
// https://www.gnu.org/licenses/lgpl-3.0
//

#define CGAL_MESH_3_VERBOSE
// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Image_3.h>
// #include <CGAL/Labeled_mesh_domain_3.h>
// #include <CGAL/Mesh_complex_3_in_triangulation_3.h>
// #include <CGAL/Mesh_constant_domain_field_3.h>
// #include <CGAL/Mesh_criteria_3.h>
// #include <CGAL/Mesh_triangulation_3.h>
// #include <CGAL/make_mesh_3.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Mesh_triangulation_3.h>

#include <CGAL/IO/File_binary_mesh_3.h>
#include <CGAL/Image_3.h>
#include <CGAL/Labeled_mesh_domain_3.h>
#include <CGAL/Mesh_3/generate_label_weights.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/tags.h>

// Domain
using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using Mesh_domain = CGAL::Labeled_mesh_domain_3<K>;

// Triangulation
using Tr = CGAL::Mesh_triangulation_3<Mesh_domain, CGAL::Default,
                                      CGAL::Parallel_if_available_tag>::type;
using C3t3 = CGAL::Mesh_complex_3_in_triangulation_3<Tr>;

// Criteria
using Mesh_criteria = CGAL::Mesh_criteria_3<Tr>;

// // Domain
// typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef CGAL::Labeled_mesh_domain_3<CGAL::Image_3, K> Mesh_domain;
// // Triangulation
// #ifdef CGAL_CONCURRENT_MESH_3
// typedef CGAL::Mesh_triangulation_3<
//     Mesh_domain,
//     CGAL::Kernel_traits<Mesh_domain>::Kernel, // Same as sequential
//     CGAL::Parallel_tag                        // Tag to activate parallelism
//     >::type Tr;
// #else
// typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
// #endif
// typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;
// // Criteria
// typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
// typedef CGAL::Mesh_constant_domain_field_3<Mesh_domain::R,
// Mesh_domain::Index>
//     Sizing_field;
// // To avoid verbose function and named parameters call

using namespace CGAL::parameters;

#include <CGAL/ImageIO.h>

#include "cgal.h"
#include "xdmf.h"

void cgal_make_mesh(const CGAL::Image_3 &image, std::string filename,
                    int cell_sizing) {
  // Domain
  Mesh_domain domain(image);

  // Something to do with cell size...
  Sizing_field size(cell_sizing);

  // Mesh criteria - can be experimented with...
  Mesh_criteria criteria(facet_angle = 30, facet_size = 6, facet_distance = 2,
                         cell_radius_edge_ratio = 3, cell_size = size);

  // Meshing
  // FIXME: put all the parameters under user control
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, lloyd(), odt(),
                                      perturb(), exude(0.0, 0.0));

  // Output
  std::stringstream medit_stream;
  c3t3.output_to_medit(medit_stream);

  medit_to_xdmf(medit_stream, filename);
}
