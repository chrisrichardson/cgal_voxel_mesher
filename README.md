# README #

This is a simple code to convert a voxel mesh in tiff format to a tetrahedral mesh.


The main code is configured with cmake. Needs libtiff, boost and CGAL.


The output is in `.mesh` format which is a bit obscure, but can be converted with `dolfin-convert` and
`meshio`. It is not guaranteed that the output will be a good quality mesh, or even properly connected.
You will have to do some checks, diagnostics and corrections to be sure.

Usage:
```
voxel_mesher --help
Allowed options:
  -h [ --help ]          describe arguments
  -i [ --input ] arg     voxel data file (in .dat or .tif format)
  -k [ --key ] arg       Pixel value to use as a key (e.g. for segmented image)
  -t [ --threshold ] arg Pixel value to threshold from (i.e. values above are
                         solid, use negative value to set values below as
	                 solid)
  -s [ --size ] arg      Cell sizing, as defined by CGAL
```
