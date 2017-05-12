// voxel_mesher
//
// Copyright 2017 Chris Richardson
//
// This software is licenced under the Gnu LGPLv3 https://www.gnu.org/licenses/lgpl-3.0
//

#define CGAL_MESH_3_VERBOSE
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Mesh_constant_domain_field_3.h>
#include <CGAL/Labeled_image_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/Image_3.h>
// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Labeled_image_mesh_domain_3<CGAL::Image_3,K> Mesh_domain;
// Triangulation
#ifdef CGAL_CONCURRENT_MESH_3
typedef CGAL::Mesh_triangulation_3<
  Mesh_domain,
  CGAL::Kernel_traits<Mesh_domain>::Kernel, // Same as sequential
  CGAL::Parallel_tag                        // Tag to activate parallelism
  >::type Tr;
#else
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
#endif
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;
// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;
typedef CGAL::Mesh_constant_domain_field_3<Mesh_domain::R,
                                           Mesh_domain::Index> Sizing_field;
// To avoid verbose function and named parameters call
using namespace CGAL::parameters;

#include <CGAL/ImageIO.h>

void cgal_make_mesh(const CGAL::Image_3& image, std::string filename,
                    int cell_sizing)
{
  // Domain
  Mesh_domain domain(image);

  // Something to do with cell size...
  Sizing_field size(cell_sizing);

  // Mesh criteria - can be experimented with...
  Mesh_criteria criteria(facet_angle=30, facet_size=6, facet_distance=2,
                         cell_radius_edge_ratio=3, cell_size=size);

  // Meshing
  // FIXME: put all the parameters under user control
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria,
                                      lloyd(),
                                      odt(),
                                      perturb(),
                                      exude(0.0, 0.0));

  // Output
  std::ofstream medit_file(filename);
  c3t3.output_to_medit(medit_file);
}
