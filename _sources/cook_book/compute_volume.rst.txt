Compute block volumes
=====================

To compute volume of a mesh or its individual blocks, use ``compute_volume``
function.
The function returns back a dictionary of block IDs and their volumes.
If there are no blocks, it returns a dictionary with ID ``0`` and the computed
volume of the entire mesh.

.. code-block:: python

   import krado

   mesh = krado.import_mesh("path/to/mesh.exo")
   vols = krado.compute_volume(mesh)
   print(vols)
