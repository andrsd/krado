A simple library for mesh manipulation
======================================

**Mesh format**

The primary mesh format is ExodusII.
This format is used for import and export of meshes.
The library design allows to add other mesh formats in future if needed.

**Simplicity**

*krado* is designed to be simple and easy to use.

**Performance**

The library should be able handle very large meshes.
It uses 64-bit integers for indexing nodes and elements.
However, ExodusII support is currently limited to 32-bit signed integers, which allows
for meshes with up to 2 billion nodes and elements only.
The internals of the library are **not** parallelized.

**Languages**

The core of the library is written in C++17 and the Python API is generated using pybind11.
This allows for great flexibility that comes with the power of Python.



.. toctree::
   :hidden:

   getting_started

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
