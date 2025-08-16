Merging meshes
==============

Merging meshes is a common operation in mesh processing. It is used to combine
two or more meshes into a single one. This can be useful when you want to
combine meshes from different sources.

.. code-block:: python

   import krado

   mesh1 = krado.import_mesh("path/to/mesh1.exo")
   mesh2 = krado.import_mesh("path/to/mesh2.exo")

   merged_mesh = mesh1.add(mesh2)

   merged_mesh.set_up()
   krado.export_mesh(merged_mesh, "path/to/merged_mesh.exo")


Note that if the meshes share nodes, the nodes will be duplicated in the merged
mesh. To remove duplicate nodes, you can use the :meth:`krado.Mesh.remove_duplicate_points`

.. code-block:: python

   merged_mesh.remove_duplicate_points()

   merged_mesh.set_up()
   krado.export_mesh(merged_mesh, "path/to/merged_mesh.exo")
