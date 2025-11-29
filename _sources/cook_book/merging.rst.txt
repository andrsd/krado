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

   merged_mesh.remove_duplicate_points(1e-10)

   merged_mesh.set_up()
   krado.export_mesh("path/to/merged_mesh.exo", merged_mesh)

The parameter ``1e-10`` is the (absolute) tolerance that decides if two nodes are identical or not.
To see if nodes were actaully merged, turn on diagnostic report:

.. code-block:: python

   krado.log.set_verbosity(2)      # 2 or higher number will print the dianostic report

Then, you get a report like so:

.. code-block:: shell

   [info] Diagnostics:
   [info]   Total close pairs: 2
   [info]   Histogram (0.0 = identical, 1.0 = exactly at tolerance)
   [info]   0.0 – 0.1 | 2 ++++++++++++++++++++++++++++++++++++++++++++++++++
   [info]   0.1 – 0.2 | 0
   [info]   0.2 – 0.3 | 0
   [info]   0.3 – 0.4 | 0
   [info]   0.4 – 0.5 | 0
   [info]   0.5 – 0.6 | 0
   [info]   0.6 – 0.7 | 0
   [info]   0.7 – 0.8 | 0
   [info]   0.8 – 0.9 | 0
   [info]   0.9 – 1.0 | 0
   [info]   Smallest separation: 0

Large counts in the first bins indicate tight node alignment.
Large counts in the last bin suggest the tolerance may be too small.
