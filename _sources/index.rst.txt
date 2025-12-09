krado: A Mesh Manipulation Package
==================================

*krado* is designed to be simple and easy to use.

The library is capable of handling very large meshes.
It uses 64-bit integers for indexing nodes and elements internally.

The primary mesh format supported by *krado* is **ExodusII**.
It is used for both importing and exporting meshes.
However, ExodusII currently supports only 32-bit signed integers, which limits mesh size to approximately **2 billion** nodes and elements.
The library is designed to be extensible, so additional mesh formats can be added in the future if needed.

The internal implementation is **not** parallelized.

The core of the library is written in **C++17**, and the **Python API** is generated using **pybind11**.
This provides the performance of C++ with the flexibility and ease of use of Python.


.. toctree::
   :caption: Gettting started
   :hidden:

   install

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: Cook book
   :glob:

   cook_book/*

.. toctree::
   :maxdepth: 1
   :hidden:
   :caption: C++ API
   :glob:

   devel/classes/*
