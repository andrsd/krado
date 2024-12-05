Mesh extrusion
==============

Extruding a mesh is a common operation in mesh processing. It is used to create
a 3D mesh from a 2D mesh by extruding the 2D mesh in the z direction.

.. code-block:: python

   import krado

   mesh2d = krado.import_mesh("path/to/mesh2d.exo")

   # Extrude the 2D mesh in the z direction, creating 2 layers with a thickness of 10.0
   dir = krado.Vector(0, 0, 1)
   mesh3d = krado.extrude(mesh2d, dir, 2, 10.0)

   mesh3d.set_up()
   krado.export_mesh(mesh3d, "path/to/mesh3d.exo")


If you need to control the thickness of each layer, you can pass a list of thicknesses
instead of a single value.

.. code-block:: python

   import krado

   mesh2d = krado.import_mesh("path/to/mesh2d.exo")

   # Extrude the 2D mesh in the z direction, creating 2 layers with thicknesses
   # of 5.0 and 10.0
   dir = krado.Vector(0, 0, 1)
   thicknesses = [5.0, 10.0]
   mesh3d = krado.extrude(mesh2d, dir, thicknesses)

   mesh3d.set_up()
   krado.export_mesh(mesh3d, "path/to/mesh3d.exo")
