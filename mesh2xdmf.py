#!/usr/bin/python
from dolfin import *
import subprocess
import numpy

subprocess.call(["dolfin-convert", "out.mesh", "out.xml"])

mesh = Mesh("out.xml")

xdmf = XDMFFile("mesh1.xdmf")
xdmf.write(mesh)

quit()


mf = CellFunction("size_t", mesh, 0)
mesh.init(2, 3)
mesh.init(3, 2)

def fill_neighbours(mesh, idx_list, mf, value):
    new_idx_list = set()
    for idx in idx_list:
        cell = Cell(mesh, idx)
        mf[idx] = value
        for f in facets(cell):
            if f.num_entities(3) == 2:
                cells = f.entities(3)
                c0 = int(cells[0])
                c1 = int(cells[1])
                if (c0 == idx and mf[c1] == 0):
                    new_idx_list.add(c1)
                elif mf[c0] == 0:
                    new_idx_list.add(c0)

    return list(new_idx_list)

# Fill mesh from first cell
idx_list = [0]
while len(idx_list) > 0:
    idx_list = fill_neighbours(mesh, idx_list, mf, 1)

cell_count = 0
idx_list = numpy.zeros(mesh.num_vertices())
for c in cells(mesh):
    if mf[c] == 1:
        for v in vertices(c):
            idx_list[v.index()] = 1
        cell_count += 1

# Renumber vertices
vertex_count = 0
for i in range(len(idx_list)):
    if idx_list[i] == 0:
        idx_list[i] = -1
    else:
        idx_list[i] = vertex_count
        vertex_count += 1

mesh2 = Mesh()
ed = MeshEditor()
ed.open(mesh2, 3, 3)

print cell_count
ed.init_cells(cell_count)
cc = 0
for c in cells(mesh):
    if mf[c] == 1:
        vidx = numpy.zeros(4, dtype='uintp')
        for i,v in enumerate(vertices(c)):
            vidx[i] = idx_list[v.index()]
        ed.add_cell(cc, vidx)
        cc += 1

ed.init_vertices(vertex_count)
vc = 0
for i in range(len(idx_list)):
    if idx_list[i] != -1:
        ed.add_vertex(vc, Vertex(mesh, i).point())
        vc += 1

print cc, cell_count
print vc, vertex_count
ed.close()

xdmf = XDMFFile("out.xdmf")
xdmf.write(mf)

xdmf = XDMFFile("outmesh.xdmf")
xdmf.write(mesh2)
