# README #

This is a simple code to convert a voxel mesh in tiff format to a tetrahedral mesh.
First extract the tiff to multiple files (maybe not needed but I couldn't figure out how else to do it quickly).

```
convert muntjac_femur.tif muntjac_femur-%05d.tif
```

The main code is configured with cmake. Needs libtiff, CGAL and boost.

 

