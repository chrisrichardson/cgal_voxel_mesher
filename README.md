# README #

This is a simple code to convert a voxel mesh in tiff format to a tetrahedral mesh.
First extract the tiff to multiple files (maybe not needed but I couldn't figure out how else to do it quickly).

```
convert muntjac_femur.tif muntjac_femur-%04d.tif
```

The main code is configured with cmake. Needs libtiff, CGAL and boost.
The output is in `.mesh` format which is a bit obscure. To convert to dolfin format (using dolfin-convert), you will
have to edit it a bit. The line "Dimension 3" has to be replaced with "Dimension" (newline) "3" (newline). And all triangles will need to be deleted. This is annoying, and should be fixed in 
dolfin-convert