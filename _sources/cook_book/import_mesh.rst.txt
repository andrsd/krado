Import/Export
=============

Importing a mesh from an ExodusII file
--------------------------------------

This example demonstrates how to import a mesh from an ExodusII file.

.. code-block:: python

   import krado

   mesh = krado.import_mesh("path/to/mesh.exo")


Exporting a mesh to an ExodusII file
------------------------------------


This example demonstrates how to export a mesh to an ExodusII file.

.. code-block:: python

   import krado

   mesh = krado.Mesh()

   # Fill the mesh with nodes and elements

   # This is important to call for the export to work correctly
   mesh.set_up()
   krado.export_mesh(mesh, "path/to/mesh.exo")
