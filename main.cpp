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

#include <cassert>
#include <tiffio.h>

int main(int argc, char *argv[])
{
  std::string filename = "muntjac_femur.tif";
 
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

  std::cout << nx << "x" << ny << "x" << nz << std::endl;

  CGAL::Image_3 image(_createImage(nx, ny, nz, 1,
				   vx, vy, vz, 1,
				   WK_FIXED, SGN_UNSIGNED));

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

  // Invert image
  unsigned char *p = (unsigned char *)(image.data());
  for (std::size_t i = 0; i != (nx*ny*nz); ++i)
  {
    *p = 255 - *p;
    ++p;
  }

  // Domain
  Mesh_domain domain(image);

  // Something to do with cell size...
  Sizing_field size(8);

  // Mesh criteria - can be experimented with...
  Mesh_criteria criteria(facet_angle=30, facet_size=6, facet_distance=2,
                         cell_radius_edge_ratio=3, cell_size=size);
  // Meshing
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);
  // Output
  std::ofstream medit_file("out.mesh");
  c3t3.output_to_medit(medit_file);
  return 0;
}
