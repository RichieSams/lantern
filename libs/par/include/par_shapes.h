// SHAPES :: https://github.com/prideout/par
// Simple C library for creation and manipulation of triangle meshes.
//
// The API is divided into three sections:
//
//   - Generators.  Create parametric surfaces, platonic solids, etc.
//   - Queries.     Ask a mesh for its axis-aligned bounding box, etc.
//   - Transforms.  Rotate a mesh, merge it with another, add normals, etc.
//
// In addition to the comment block above each function declaration, the API
// has informal documentation here:
//
//     http://github.prideout.net/shapes/
//
// For our purposes, a "mesh" is a list of points and a list of triangles; the
// former is a flattened list of three-tuples (32-bit floats) and the latter is
// also a flattened list of three-tuples (16-bit uints).  Triangles are always
// oriented such that their front face winds counter-clockwise.
//
// Optionally, meshes can contain 3D normals (one per vertex), and 2D texture
// coordinates (one per vertex).  That's it!  If you need something fancier,
// look elsewhere.
//
// The MIT License
// Copyright (c) 2015 Philip Rideout

#ifndef PAR_SHAPES_H
#define PAR_SHAPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#ifndef PAR_SHAPES_T
#define PAR_SHAPES_T uint32_t
#endif

typedef struct par_shapes_mesh_s {
    float* points;           // Flat list of 3-tuples (X Y Z X Y Z...)
    int npoints;             // Number of points
    PAR_SHAPES_T* triangles; // Flat list of 3-tuples (I J K I J K...)
    int ntriangles;          // Number of triangles
    float* normals;          // Optional list of 3-tuples (X Y Z X Y Z...)
    float* tcoords;          // Optional list of 2-tuples (U V U V U V...)
} par_shapes_mesh;

void par_shapes_free_mesh(par_shapes_mesh*);

// Generators ------------------------------------------------------------------

// Instance a cylinder that sits on the Z=0 plane using the given tessellation
// levels across the UV domain.  Think of "slices" like a number of pizza
// slices, and "stacks" like a number of stacked rings.  Height and radius are
// both 1.0, but they can easily be changed with par_shapes_scale.
par_shapes_mesh* par_shapes_create_cylinder(int slices, int stacks);

// Create a donut that sits on the Z=0 plane with the specified inner radius.
// The outer radius can be controlled with par_shapes_scale.
par_shapes_mesh* par_shapes_create_torus(int slices, int stacks, float radius);

// Create a sphere with texture coordinates and small triangles near the poles.
par_shapes_mesh* par_shapes_create_parametric_sphere(int slices, int stacks);

// Approximate a sphere with a subdivided icosahedron, which produces a nice
// distribution of triangles, but no texture coordinates.  Each subdivision
// level scales the number of triangles by four, so use a very low number.
par_shapes_mesh* par_shapes_create_subdivided_sphere(int nsubdivisions);

// More parametric surfaces.
par_shapes_mesh* par_shapes_create_klein_bottle(int slices, int stacks);
par_shapes_mesh* par_shapes_create_trefoil_knot(int slices, int stacks,
    float radius);
par_shapes_mesh* par_shapes_create_hemisphere(int slices, int stacks);
par_shapes_mesh* par_shapes_create_plane(int slices, int stacks);

// Create a parametric surface from a callback function that consumes a 2D
// point in [0,1] and produces a 3D point.
typedef void (*par_shapes_fn)(float const*, float*, void*);
par_shapes_mesh* par_shapes_create_parametric(par_shapes_fn, int slices,
    int stacks, void* userdata);

// Generate points for a 20-sided polyhedron that fits in the unit sphere.
// Texture coordinates and normals are not generated.
par_shapes_mesh* par_shapes_create_icosahedron();

// Generate points for a 12-sided polyhedron that fits in the unit sphere.
// Again, texture coordinates and normals are not generated.
par_shapes_mesh* par_shapes_create_dodecahedron();

// More platonic solids.
par_shapes_mesh* par_shapes_create_octohedron();
par_shapes_mesh* par_shapes_create_tetrahedron();
par_shapes_mesh* par_shapes_create_cube();

// Generate an orientable disk shape in 3-space.  Does not include normals or
// texture coordinates.
par_shapes_mesh* par_shapes_create_disk(float radius, int slices,
    float const* center, float const* normal);

// Create an empty shape.  Useful for building scenes with merge_and_free.
par_shapes_mesh* par_shapes_create_empty();

// Generate a rock shape that sits on the Y=0 plane, and sinks into it a bit.
// This includes smooth normals but no texture coordinates.  Each subdivision
// level scales the number of triangles by four, so use a very low number.
par_shapes_mesh* par_shapes_create_rock(int seed, int nsubdivisions);

// Create trees or vegetation by executing a recursive turtle graphics program.
// The program is a list of command-argument pairs.  See the unit test for
// an example.  Texture coordinates and normals are not generated.
par_shapes_mesh* par_shapes_create_lsystem(char const* program, int slices,
    int maxdepth);

// Queries ---------------------------------------------------------------------

// Dump out a text file conforming to the venerable OBJ format.
void par_shapes_export(par_shapes_mesh const*, char const* objfile);

// Take a pointer to 6 floats and set them to min xyz, max xyz.
void par_shapes_compute_aabb(par_shapes_mesh const* mesh, float* aabb);

// Make a deep copy of a mesh.  To make a brand new copy, pass null to "target".
// To avoid memory churn, pass an existing mesh to "target".
par_shapes_mesh* par_shapes_clone(par_shapes_mesh const* mesh,
    par_shapes_mesh* target);

// Transformations -------------------------------------------------------------

void par_shapes_merge(par_shapes_mesh* dst, par_shapes_mesh const* src);
void par_shapes_translate(par_shapes_mesh*, float x, float y, float z);
void par_shapes_rotate(par_shapes_mesh*, float radians, float const* axis);
void par_shapes_scale(par_shapes_mesh*, float x, float y, float z);
void par_shapes_merge_and_free(par_shapes_mesh* dst, par_shapes_mesh* src);

// Reverse the winding of a run of faces.  Useful when drawing the inside of
// a Cornell Box.  Pass 0 for nfaces to reverse every face in the mesh.
void par_shapes_invert(par_shapes_mesh*, int startface, int nfaces);

// Remove all triangles whose area is less than minarea.
void par_shapes_remove_degenerate(par_shapes_mesh*, float minarea);

// Dereference the entire index buffer and replace the point list.
// This creates an inefficient structure, but is useful for drawing facets.
// If create_indices is true, a trivial "0 1 2 3..." index buffer is generated.
void par_shapes_unweld(par_shapes_mesh* mesh, bool create_indices);

// Merge colocated verts, build a new index buffer, and return the
// optimized mesh.  Epsilon is the maximum distance to consider when
// welding vertices. The mapping argument can be null, or a pointer to
// npoints integers, which gets filled with the mapping from old vertex
// indices to new indices.
par_shapes_mesh* par_shapes_weld(par_shapes_mesh const*, float epsilon,
    PAR_SHAPES_T* mapping);

// Compute smooth normals by averaging adjacent facet normals.
void par_shapes_compute_normals(par_shapes_mesh* m);

#ifndef PAR_PI
#define PAR_PI (3.14159265359f)
#define PAR_MIN(a, b) (a > b ? b : a)
#define PAR_MAX(a, b) (a > b ? a : b)
#define PAR_CLAMP(v, lo, hi) PAR_MAX(lo, PAR_MIN(hi, v))
#define PAR_SWAP(T, A, B) { T tmp = B; B = A; A = tmp; }
#define PAR_SQR(a) ((a) * (a))
#endif

#ifndef PAR_MALLOC
#define PAR_MALLOC(T, N) ((T*) malloc(N * sizeof(T)))
#define PAR_CALLOC(T, N) ((T*) calloc(N * sizeof(T), 1))
#define PAR_REALLOC(T, BUF, N) ((T*) realloc(BUF, sizeof(T) * N))
#define PAR_FREE(BUF) free(BUF)
#endif

#ifdef __cplusplus
}
#endif

// -----------------------------------------------------------------------------
// END PUBLIC API
// -----------------------------------------------------------------------------

#endif // PAR_SHAPES_H
