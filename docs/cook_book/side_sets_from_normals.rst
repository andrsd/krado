Create Side Sets from Outward Normals
=====================================

For some meshes, side sets can be generated based on **outward normals**,
especially when the boundary sides have well-defined directions.

While side sets technically only need numeric IDs, it is strongly **recommended**
to also assign descriptive names for easier reference. The example below
shows how to do this in code.

Before calling ``Mesh::boundary_edges``, make sure to first call ``Mesh::set_up``.
This step is essential---it initializes the edge IDs that will later be used
to form side sets.

The general approach is straightforward:

1. Loop over all boundary edges.
2. Compute the **outward normal** for each edge.
3. Group edges with similar normals into "buckets." Each bucket represents a
   potential side set.

Once the buckets are created, you can register each one as a side set by calling:

.. code-block::

   Mesh.set_side_set(id, edges)

Here, ``id`` must be a **positive integer**.

To give the side set a meaningful name, use:

.. code-block::

   Mesh.set_side_set_name(id, name)

This links the numeric ID to a user-friendly label, making your mesh definition
easier to read and maintain.

Example:

.. code-block:: python

   import krado
   import math

   # create a mesh, for example via:
   #   mesh = krado.import_mesh("mesh.exo")

   # This must be called so we can use `boundary_edges` and `outward_normal`
   mesh.set_up()

   face_sets = {
      "left": [],
      "right": [],
      "bottom": [],
      "other": []
   }
   bnd_edges = mesh.boundary_edges()
   for edge in bnd_edges:
       n = mesh.outward_normal(edge)
       if n.is_equal(krado.Vector(1, 0)):
           face_sets["right"].append(edge)
       elif n.is_equal(krado.Vector(-1, 0)):
           face_sets["left"].append(edge)
       elif n.is_equal(krado.Vector(0, -1)):
           face_sets["bottom"].append(edge)
       else:
           face_sets["other"].append(edge)

   for (idx, name) in enumerate(face_sets):
       # pick some ID
       id = 1000 + idx
       mesh.set_side_set(id, face_sets[name])
       mesh.set_side_set_name(id, name)

   # export mesh here
