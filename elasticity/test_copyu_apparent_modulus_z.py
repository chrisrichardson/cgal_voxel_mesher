#!/usr/bin/python
from dolfin import *
import numpy as np

# Set backend to PETSC
parameters["linear_algebra_backend"] = "PETSc"

def build_nullspace(V, x, m):
    """Function to build null space for 3D elasticity"""

    # Create list of vectors for null space
    nullspace_basis = [x.copy() for i in range(6)]

    # Build translational null space basis
    V.sub(0).dofmap().set(nullspace_basis[0], 1.0);
    V.sub(1).dofmap().set(nullspace_basis[1], 1.0);
    V.sub(2).dofmap().set(nullspace_basis[2], 1.0);

    # Build rotational null space basis
    V.sub(0).dofmap().set_x(nullspace_basis[3], -1.0, 1, m);
    V.sub(1).dofmap().set_x(nullspace_basis[3],  1.0, 0, m);
    V.sub(0).dofmap().set_x(nullspace_basis[4],  1.0, 2, m);
    V.sub(2).dofmap().set_x(nullspace_basis[4], -1.0, 0, m);
    V.sub(2).dofmap().set_x(nullspace_basis[5],  1.0, 1, m);
    V.sub(1).dofmap().set_x(nullspace_basis[5], -1.0, 2, m);

    for x in nullspace_basis:
        x.apply("insert")

    return VectorSpaceBasis(nullspace_basis)


set_log_level(0)

mesh = Mesh("copyu.xml")

#r0 = Point(-351, -89, 234)
#n = Point(1,0,1)
#d0 = r0.dot(n)
marker = FacetFunction("size_t", mesh, 0)
mesh.init(2, 3)
zdirection = np.array((0.0, 0.0, 1.0))
for f in facets(mesh):
    if(f.num_entities(3) == 1):
	n = f.normal()
	nvec = np.array((n.x(), n.y(), n.z()))
        if (f.midpoint().z() < 5 and np.arccos(np.dot(nvec,-zdirection))<0.7):		
            marker[f] = 1 #non-null neumann/non-null dirichlet
        if (f.midpoint().z() > 3150 and np.arccos(np.dot(nvec,zdirection))<0.7):
            marker[f] = 2 #dirichlet


xdmf = File("marker.xdmf")
xdmf << marker

# Elasticity parameters
E = 20000
nu = 0.3
mu = E/(2.0*(1.0 + nu))
lmbda = E*nu/((1.0 + nu)*(1.0 - 2.0*nu))

# Stress computation
def sigma(v):
    return 2.0*mu*sym(grad(v))+lmbda*tr(sym(grad(v)))*Identity(len(v))
def meanpressure(sigma):
        return 1.0/3.0*tr(sigma)
def deviatoricstress(sigma):
        return sigma-meanpressure(sigma)*Identity(3)
def vonMises(v):
        localsigma = sigma(v)
        devsigma = deviatoricstress(localsigma)
        return sqrt(3.0/2*inner(devsigma,devsigma))


# Create function space
V = VectorFunctionSpace(mesh, "Lagrange", 1)
Q = FunctionSpace(mesh, "Lagrange", 1)

# Applied force on femoral head
ds = Measure("ds", domain=mesh, subdomain_data=marker)
f = Constant((0.0, 0.0, 0.0))

# Define variational problem
u = TrialFunction(V)
v = TestFunction(V)
a = inner(sigma(u), grad(v))*dx
L = inner(f, v)*ds(1)

# Set up boundary condition on distal end
c1 = Constant((0.0, 0.0, 0.0))
bc1 = DirichletBC(V, c1, marker, 2)

c2=Constant((0.0, 0.0, 31.7))
bc2 = DirichletBC(V, c2, marker, 1)


# Assemble system, applying boundary conditions and preserving
# symmetry)
A, b = assemble_system(a, L, [bc1,bc2])

# Create solution function
u = Function(V)

# Create near null space basis (required for smoothed aggregation
# AMG). The solution vector is passed so that it can be copied to
# generate compatible vectors for the nullspace.
null_space = build_nullspace(V, u.vector(), mesh)

# Create PETSC smoothed aggregation AMG preconditioner and attach near
# null space
pc = PETScPreconditioner("petsc_amg")
pc.set_nullspace(null_space)

# Create CG Krylov solver and turn convergence monitoring on
solver = PETScKrylovSolver("cg", pc)
solver.parameters["monitor_convergence"] = True

# Set matrix operator
solver.set_operator(A);

# Compute solution
solver.solve(u.vector(), b);

soln = File("solution.pvd")
soln << u



