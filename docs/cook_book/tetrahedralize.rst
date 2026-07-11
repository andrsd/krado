Tetrahedralize a mesh
=====================

In this context, tetraheralization refers to the process of converting
existing 3D mesh elements like hexahedras or pyramids into tetrahedras.

.. code-block:: python

   import krado

   mesh = krado.import_mesh("path/to/mesh.exo")

   # Tetrahedralize the mesh
   mesh_tet = krado.tetrahedralize(mesh)

   mesh_tet.set_up()
   krado.export_mesh(mesh_tet, "path/to/tetrahedralized_mesh.exo")
