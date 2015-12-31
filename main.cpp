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

#include <sstream>
#include <iomanip>
#include <boost/filesystem.hpp>
#include <tiffio.h>

int main()
{
  std::vector<unsigned char> image_data;

  std::size_t i = 0;
  uint32 w, h;
  bool file_exists = true;
  while(file_exists)
    {
      std::stringstream s;
      s << "muntjac_femur-";
      s << std::setfill('0') << std::setw(4) << i;
      s << ".tif";

      std::cout << s.str() << "\n";

      file_exists = boost::filesystem::exists(s.str());
      if (file_exists)
	{
          TIFF* tif = TIFFOpen(s.str().c_str(), "r");
          if (tif) 
	    {
	      std::size_t npixels;
	      uint32* raster;
	      
	      TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
	      TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
	      std::cout << w << "*" << h << "\n";
	      npixels = w * h;
	      raster = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
	      if (raster != NULL)
		{
		  if (TIFFReadRGBAImage(tif, w, h, raster, 0))
		    {
		      image_data.insert(image_data.end(), raster, raster + npixels);
		    }
		  _TIFFfree(raster);
		}
	      TIFFClose(tif);
	    }
	}
      ++i;	
    }
  // Got 3D image
  
  int nx=w, ny=h, nz=i;
  double vx=1.0, vy=1.0, vz=1.0;

  std::cout << nx << "x" << ny << "x" << nz << std::endl;
  
  CGAL::Image_3 image(_createImage(nx, ny, nz, 1,
				   vx, vy, vz, 1,
				   WK_FIXED, SGN_UNSIGNED));

  // Invert image
  for (auto &p : image_data)
    p = 255 - p;

  unsigned char *ptr = (unsigned char *)(image.data());
  std::copy(image_data.begin(), image_data.end(), ptr);

  std::cout << "max=" << (int)(*std::max_element(ptr, ptr+nx*ny*nz)) << "\n";
  std::cout << "min=" << (int)(*std::min_element(ptr, ptr+nx*ny*nz)) << "\n";

  // Domain
  Mesh_domain domain(image);

  Sizing_field size(8);

  // Mesh criteria
  Mesh_criteria criteria(facet_angle=30, facet_size=6, facet_distance=2,
                         cell_radius_edge_ratio=3, cell_size=size);
  // Meshing
  C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria);
  // Output
  std::ofstream medit_file("out.mesh");
  c3t3.output_to_medit(medit_file);
  return 0;
}
