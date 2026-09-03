Create Edge/Face Sets Programatically
=====================================

For some meshes, side sets can be generated based on the geometrical information,
especially when the boundary sides have well-defined directions.

While side sets technically only need numeric IDs, it is strongly **recommended**
to also assign descriptive names for easier reference. The example below
shows how to do this in code.

Register each side set by calling:

.. code-block::

   Mesh.set_edge_set(id, edges)
   # or
   Mesh.set_face_set(id, faces)

Here, ``id`` must be a **positive integer**.

To give the side set a meaningful name, use:

.. code-block::

   Mesh.set_edge_set_name(id, name)
   # or
   Mesh.set_face_set_name(id, name)

This links the numeric ID to a user-friendly label, making your mesh definition
easier to read and maintain.


Using Outward Normals
---------------------

The general approach is straightforward:

1. Loop over all boundary edges/faces.
2. Compute the **outward normal** for each edge/face.
3. Group edges/faces with similar normals into "buckets." Each bucket represents a
   potential side set.

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
       mesh.set_edge_set(id, face_sets[name])
       mesh.set_edge_set_name(id, name)

   # export mesh here

Using Facet Centroids
---------------------

Another way to form side sets is to use ``krado.SolidClassier`` class which can determine
if a point lies inside of a shape or not.  ``SolidClassifer`` needs a solid shape that it will
operate on. Then, using :meth:`krado.SolidClassier.inside` or :meth:`krado.SolidClassier.outside`
we can determine is the point lies inside of the shape or not.

The general approach is as follows:

1. Loop over all boundary edges/facets.
2. Compute the **centroid** for each edge/facet.
3. Store edge/face IDs in a list to create an edge/face set.

Example:

.. code-block:: python

   import krado

   # create a mesh, for example via:
   #   mesh = krado.import_mesh("mesh.exo")

   # This must be called so we can use `boundary_faces` and `compute_centroid`
   mesh.set_up()

   # Create a cylindrical shape
   center = krado.Axis2(krado.Point(1.0, 0.0, 0.0), krado.Vector(0.0, 0.0, 1))
   radius = 0.5
   height = 2.
   cylinder = krado.Cylinder(center, radius, height)
   # Classfier will determine if a point lies inside or outside of the cylinder
   clsf = krado.SolidClassifier(cylinder)

   inside_facets = []
   facet_ids = mesh.boundary_faces()
   for id in facet_ids:
       centroid = mesh.compute_centroid(id)
       # if `centroid` falls inside the `cylinder`, we store its ID
       if clsf.inside(centroid):
           inside_facets.append(id)
   # Store the face set
   id = 1000
   mesh.set_face_set(id, inside_facets)
   mesh.set_face_set_name(id, "inside")
